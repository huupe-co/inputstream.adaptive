/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../IDecrypter.h"
// #include "cdm/media/cdm/api/content_decryption_module.h"
#include "common/AdaptiveCencSampleDecrypter.h"

#include <list>
#include <mutex>
#include <optional>

class CWVDecrypter;

using namespace DRM;

class ATTR_DLL_LOCAL CWVCencSingleSampleDecrypter : public Adaptive_CencSingleSampleDecrypter
{
public:
  // methods
  CWVCencSingleSampleDecrypter(std::vector<uint8_t>& pssh,
                               std::string_view optionalKeyParameter,
                               std::string_view defaultKeyId,
                               bool skipSessionMessage,
                               CryptoMode cryptoMode,
                               CWVDecrypter* host)
    : m_pssh(pssh)
  {
    m_optionalKeyParameter = optionalKeyParameter;
    m_defaultKeyId = defaultKeyId;
    m_skipSessionMessage = skipSessionMessage;
    m_cryptoMode = cryptoMode;
    m_host = host;
  };
  virtual ~CWVCencSingleSampleDecrypter(){};

  void GetCapabilities(std::string_view keyId,
                       uint32_t media,
                       IDecrypter::DecrypterCapabilites& caps)
  {
    printf("Unhandled call");
    return;
  }
  virtual const char* GetSessionId() override
  {
    printf("Unhandled call");
    return "";
  };
  void CloseSessionId()
  {
    printf("Unhandled call");
    return;
  };
  AP4_DataBuffer GetChallengeData()
  {
    printf("Unhandled call");
    return {};
  }

  void SetSession(const char* session, uint32_t sessionSize, const uint8_t* data, size_t dataSize)
  {
    printf("Unhandled call");
    ;
  }

  void AddSessionKey(const uint8_t* data, size_t dataSize, uint32_t status)
  {
    printf("Unhandled call");
  }
  bool HasKeyId(std::string_view keyid)
  {
    printf("Unhandled call");
    return true;
  }

  virtual AP4_Result SetFragmentInfo(AP4_UI32 poolId,
                                     const std::vector<uint8_t>& keyId,
                                     const AP4_UI08 nalLengthSize,
                                     AP4_DataBuffer& annexbSpsPps,
                                     AP4_UI32 flags,
                                     CryptoInfo cryptoInfo) override
  {
    printf("Unhandled call");
    return {};
  };
  virtual AP4_UI32 AddPool() override
  {
    printf("Unhandled call");
    return 0;
  };
  virtual void RemovePool(AP4_UI32 poolId) override
  {
    printf("Unhandled call");
    return;
  };

  virtual AP4_Result DecryptSampleData(
      AP4_UI32 poolId,
      AP4_DataBuffer& dataIn,
      AP4_DataBuffer& dataOut,

      // always 16 bytes
      const AP4_UI08* iv,

      // pass 0 for full decryption
      unsigned int subsampleCount,

      // array of <subsample_count> integers. NULL if subsample_count is 0
      const AP4_UI16* bytesOfCleartextData,

      // array of <subsample_count> integers. NULL if subsample_count is 0
      const AP4_UI32* bytesOfEncryptedData) override
  {
    printf("Unhandled call");
    return 0;
  }

  bool OpenVideoDecoder(const VIDEOCODEC_INITDATA* initData)
  {
    printf("Unhandled call");
    return false;
  };
  VIDEOCODEC_RETVAL DecryptAndDecodeVideo(kodi::addon::CInstanceVideoCodec* codecInstance,
                                          const DEMUX_PACKET* sample)
  {
    printf("Unhandled call");
    return VIDEOCODEC_RETVAL::VC_ERROR;
  }
  VIDEOCODEC_RETVAL VideoFrameDataToPicture(kodi::addon::CInstanceVideoCodec* codecInstance,
                                            VIDEOCODEC_PICTURE* picture)
  {
    printf("Unhandled call");
    return VIDEOCODEC_RETVAL::VC_ERROR;
  }
  void ResetVideo() { printf("Unhandled call"); }
  void SetDefaultKeyId(std::string_view keyId) override
  {
    printf("Unhandled call");
    m_defaultKeyId = keyId;
  };
  void AddKeyId(std::string_view keyId) override
  {
    printf("Unhandled call");
    return;
  }

private:
  std::string m_strSession;
  std::vector<uint8_t> m_pssh;
  std::string m_optionalKeyParameter;
  AP4_DataBuffer m_challenge;
  std::string m_defaultKeyId;
  bool m_skipSessionMessage;
  CryptoMode m_cryptoMode;
  CWVDecrypter* m_host;
};
