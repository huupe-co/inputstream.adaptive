/*
 *  Copyright (C) 2016 liberty-developer (https://github.com/liberty-developer)
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WVDecrypter.h"

#include "WVCdmAdapter.h"
#include "WVCencSingleSampleDecrypter.h"
#include "utils/Base64Utils.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include <kodi/Filesystem.h>


using namespace DRM;
using namespace UTILS;
using namespace kodi::tools;


CWVDecrypter::~CWVDecrypter()
{
  delete m_WVCdmAdapter;
  m_WVCdmAdapter = nullptr;
}

bool CWVDecrypter::Initialize()
{
  return true;
}

std::string CWVDecrypter::SelectKeySytem(std::string_view keySystem)
{
  if (keySystem == "com.widevine.alpha")
    return "urn:uuid:EDEF8BA9-79D6-4ACE-A3C8-27DCD51D21ED";

  return "";
}

bool CWVDecrypter::OpenDRMSystem(std::string_view licenseURL,
                                 const std::vector<uint8_t>& serverCertificate,
                                 const uint8_t config)
{
  m_WVCdmAdapter = new CWVCdmAdapter(licenseURL, serverCertificate, config, this);

  return m_WVCdmAdapter->GetCdmAdapter() != nullptr;
}

Adaptive_CencSingleSampleDecrypter* CWVDecrypter::CreateSingleSampleDecrypter(
    std::vector<uint8_t>& pssh,
    std::string_view optionalKeyParameter,
    std::string_view defaultKeyId,
    bool skipSessionMessage,
    CryptoMode cryptoMode)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter CreateSingleSampleDecrypter.");
  CWVCencSingleSampleDecrypter* decrypter = new CWVCencSingleSampleDecrypter(
      *m_WVCdmAdapter, pssh, defaultKeyId, skipSessionMessage, cryptoMode, this);
  if (!decrypter->GetSessionId())
  {
    delete decrypter;
    decrypter = nullptr;
  }
  return decrypter;
}

void CWVDecrypter::DestroySingleSampleDecrypter(Adaptive_CencSingleSampleDecrypter* decrypter)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter DestroySingleSampleDecrypter.");
  if (decrypter)
  {
    // close session before dispose
    static_cast<CWVCencSingleSampleDecrypter*>(decrypter)->CloseSessionId();
    delete static_cast<CWVCencSingleSampleDecrypter*>(decrypter);
  }
}

void CWVDecrypter::GetCapabilities(Adaptive_CencSingleSampleDecrypter* decrypter,
                                   std::string_view keyId,
                                   uint32_t media,
                                   IDecrypter::DecrypterCapabilites& caps)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter DestroySingleSampleDecrypter.");
  if (!decrypter)
  {
    caps = {0, 0, 0};
    return;
  }

  static_cast<CWVCencSingleSampleDecrypter*>(decrypter)->GetCapabilities(keyId, media, caps);
}

bool CWVDecrypter::HasLicenseKey(Adaptive_CencSingleSampleDecrypter* decrypter,
                                 std::string_view keyId)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter HasLicenseKey.");
  if (decrypter)
    return static_cast<CWVCencSingleSampleDecrypter*>(decrypter)->HasKeyId(keyId);
  return false;
}

std::string CWVDecrypter::GetChallengeB64Data(Adaptive_CencSingleSampleDecrypter* decrypter)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter GetChallengeB64Data.");
  if (!decrypter)
    return "";

  AP4_DataBuffer challengeData =
      static_cast<CWVCencSingleSampleDecrypter*>(decrypter)->GetChallengeData();
  return BASE64::Encode(challengeData.GetData(), challengeData.GetDataSize());
}

bool CWVDecrypter::OpenVideoDecoder(Adaptive_CencSingleSampleDecrypter* decrypter,
                                    const VIDEOCODEC_INITDATA* initData)
{
  // if (!decrypter || !initData)
  //   return false;

  // m_decodingDecrypter = static_cast<CWVCencSingleSampleDecrypter*>(decrypter);
  // return m_decodingDecrypter->OpenVideoDecoder(initData);
  return false;
}

VIDEOCODEC_RETVAL CWVDecrypter::DecryptAndDecodeVideo(
    kodi::addon::CInstanceVideoCodec* codecInstance, const DEMUX_PACKET* sample)
{
  // if (!m_decodingDecrypter)
  //   return VC_ERROR;

  // return m_decodingDecrypter->DecryptAndDecodeVideo(codecInstance, sample);
  return VC_ERROR;
}


bool CWVDecrypter::Decrypt(const DEMUX_PACKET* sampleIn)
{
  LOG::LogF(LOGDEBUG, "CWVDecrypter Decrypt.");
  if (!m_decodingDecrypter)
    return false;

  return m_decodingDecrypter->Decrypt(sampleIn);
}


VIDEOCODEC_RETVAL CWVDecrypter::VideoFrameDataToPicture(
    kodi::addon::CInstanceVideoCodec* codecInstance, VIDEOCODEC_PICTURE* picture)
{
  // if (!m_decodingDecrypter)
  //   return VC_ERROR;

  // return m_decodingDecrypter->VideoFrameDataToPicture(codecInstance, picture);
  return VC_ERROR;
}

void CWVDecrypter::ResetVideo()
{
  // if (m_decodingDecrypter)
  //   m_decodingDecrypter->ResetVideo();
}

void CWVDecrypter::SetLibraryPath(const char* libraryPath)
{
  m_strLibraryPath = libraryPath;

  const char* pathSep{libraryPath[0] && libraryPath[1] == ':' && isalpha(libraryPath[0]) ? "\\"
                                                                                         : "/"};

  if (m_strLibraryPath.size() && m_strLibraryPath.back() != pathSep[0])
    m_strLibraryPath += pathSep;
}

void CWVDecrypter::SetProfilePath(const std::string& profilePath)
{
  m_strProfilePath = profilePath;

  // const char* pathSep{profilePath[0] && profilePath[1] == ':' && isalpha(profilePath[0]) ? "\\"
  //                                                                                        : "/"};

  // if (m_strProfilePath.size() && m_strProfilePath.back() != pathSep[0])
  //   m_strProfilePath += pathSep;

  // //let us make cdm userdata out of the addonpath and share them between addons
  // m_strProfilePath.resize(m_strProfilePath.find_last_of(pathSep[0], m_strProfilePath.length() - 2));
  // m_strProfilePath.resize(m_strProfilePath.find_last_of(pathSep[0], m_strProfilePath.length() - 1));
  // m_strProfilePath.resize(m_strProfilePath.find_last_of(pathSep[0], m_strProfilePath.length() - 1) +
  //                         1);

  kodi::vfs::CreateDirectory(m_strProfilePath.c_str());
  m_strProfilePath += "cdm";
  m_strProfilePath += "/";
  kodi::vfs::CreateDirectory(m_strProfilePath.c_str());
}

bool CWVDecrypter::GetBuffer(void* instance, VIDEOCODEC_PICTURE& picture)
{
  // return instance ? static_cast<kodi::addon::CInstanceVideoCodec*>(instance)->GetFrameBuffer(
  //                       *reinterpret_cast<VIDEOCODEC_PICTURE*>(&picture))
  //                 : false;
  return false;
}

void CWVDecrypter::ReleaseBuffer(void* instance, void* buffer)
{
  // if (instance)
  //   static_cast<kodi::addon::CInstanceVideoCodec*>(instance)->ReleaseFrameBuffer(buffer);
}
