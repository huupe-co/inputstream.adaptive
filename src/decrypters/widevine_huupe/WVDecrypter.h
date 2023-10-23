/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "../IDecrypter.h"
#include "WVCencSingleSampleDecrypter.h"

#include <vector>

#include <bento4/Ap4.h>

using namespace DRM;
using namespace kodi::tools;

/*********************************************************************************************/

class ATTR_DLL_LOCAL CWVDecrypter : public IDecrypter
{
public:
  CWVDecrypter() : m_decodingDecrypter(nullptr){};
  virtual ~CWVDecrypter() override{

  };

  virtual bool Initialize() override { return true; };

  virtual const char* SelectKeySytem(const char* keySystem) override
  {
    if (strcmp(keySystem, "com.widevine.alpha"))
      return nullptr;

    return "urn:uuid:EDEF8BA9-79D6-4ACE-A3C8-27DCD51D21ED";
  };

  virtual bool OpenDRMSystem(const char* licenseURL,
                             const AP4_DataBuffer& serverCertificate,
                             const uint8_t config) override
  {
    m_licenseURL = licenseURL;
    m_serverCertificate.reserve(serverCertificate.GetBufferSize());
    memcpy(&m_serverCertificate[0], serverCertificate.GetData(), serverCertificate.GetBufferSize());
    m_config = config;
    return true;
  }

  virtual Adaptive_CencSingleSampleDecrypter* CreateSingleSampleDecrypter(
      AP4_DataBuffer& pssh,
      std::string_view optionalKeyParameter,
      std::string_view defaultKeyId,
      bool skipSessionMessage,
      CryptoMode cryptoMode) override
  {
    m_Decrypters.push_back(CWVCencSingleSampleDecrypter(pssh, optionalKeyParameter, defaultKeyId,
                                                        skipSessionMessage, cryptoMode, this));
    return &m_Decrypters[m_Decrypters.size() - 1];
  }

  virtual void DestroySingleSampleDecrypter(Adaptive_CencSingleSampleDecrypter* decrypter) override
  {
    printf("Unhandled call");
    return;
  }

  virtual void GetCapabilities(Adaptive_CencSingleSampleDecrypter* decrypter,
                               std::string_view keyId,
                               uint32_t media,
                               IDecrypter::DecrypterCapabilites& caps) override
  {
    printf("Unhandled call");
    return;
  }
  virtual bool HasLicenseKey(Adaptive_CencSingleSampleDecrypter* decrypter,
                             std::string_view keyId) override
  {
    printf("Unhandled call");
    return true;
  }
  virtual bool IsInitialised() override { return isOpen; }
  virtual std::string GetChallengeB64Data(Adaptive_CencSingleSampleDecrypter* decrypter) override
  {
    printf("Unhandled call");
    return "";
  }
  virtual bool OpenVideoDecoder(Adaptive_CencSingleSampleDecrypter* decrypter,
                                const VIDEOCODEC_INITDATA* initData) override
  {
    printf("Unhandled call");
    return false;
  }
  virtual VIDEOCODEC_RETVAL DecryptAndDecodeVideo(kodi::addon::CInstanceVideoCodec* codecInstance,
                                                  const DEMUX_PACKET* sample) override
  {
    printf("Unhandled call");
    return VIDEOCODEC_RETVAL::VC_ERROR;
  }
  virtual VIDEOCODEC_RETVAL VideoFrameDataToPicture(kodi::addon::CInstanceVideoCodec* codecInstance,
                                                    VIDEOCODEC_PICTURE* picture) override
  {
    printf("Unhandled call");
    return VIDEOCODEC_RETVAL::VC_ERROR;
  }
  virtual void ResetVideo() override {}
  virtual void SetLibraryPath(const char* libraryPath) override { m_strLibraryPath = libraryPath; };
  virtual void SetProfilePath(const std::string& profilePath) override
  {
    m_strProfilePath = profilePath;
  };
  virtual void SetDebugSaveLicense(bool isDebugSaveLicense) override
  {
    m_isDebugSaveLicense = isDebugSaveLicense;
  }
  virtual bool GetBuffer(void* instance, VIDEOCODEC_PICTURE& picture) { return false; };
  virtual void ReleaseBuffer(void* instance, void* buffer){};
  virtual const char* GetLibraryPath() const override { return m_strLibraryPath.c_str(); }
  virtual const char* GetProfilePath() const override { return m_strProfilePath.c_str(); }
  virtual const bool IsDebugSaveLicense() const override { return m_isDebugSaveLicense; }

private:
  std::vector<CWVCencSingleSampleDecrypter> m_Decrypters;
  CWVCencSingleSampleDecrypter* m_decodingDecrypter;

  std::string m_strProfilePath;
  std::string m_strLibraryPath;

  std::string m_licenseURL;
  bool isOpen = false;
  std::vector<uint8_t> m_serverCertificate;
  uint8_t m_config;

  bool m_isDebugSaveLicense;
};
