/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WVCdmAdapter.h"

#include "WVCencSingleSampleDecrypter.h"
#include "WVDecrypter.h"
#include "decrypters/Helpers.h"
#include "utils/FileUtils.h"
#include "utils/log.h"

#include <kodi/Filesystem.h>
#include <widevine/cdm.h>

using namespace UTILS;


CWVCdmAdapter::CWVCdmAdapter(std::string_view licenseURL,
                             const std::vector<uint8_t>& serverCert,
                             const uint8_t config,
                             CWVDecrypter* host)
  : m_licenseUrl(licenseURL), m_host(host)
{
  std::string strLibPath = m_host->GetLibraryPath();

  if (licenseURL.empty())
  {
    LOG::LogF(LOGERROR, "No license URL path specified");
    return;
  }

  // The license url come from license_key kodi property
  // we have to kept only the url without the parameters specified after pipe "|" char
  std::string licUrl = m_licenseUrl;
  const size_t urlPipePos = licUrl.find('|');
  if (urlPipePos != std::string::npos)
    licUrl.erase(urlPipePos);

  // Build up a CDM path to store decrypter specific stuff, each domain gets it own path
  // the domain name is hashed to generate a short folder name
  std::string basePath = FILESYS::PathCombine(m_host->GetProfilePath(), "widevine");
  basePath = FILESYS::PathCombine(basePath, DRM::GenerateUrlDomainHash(licUrl));
  basePath += FILESYS::SEPARATOR;

  Initialize();
  // if (!serverCert.empty())
  //   wv_adapter->SetServerCertificate(0, serverCert.data(), serverCert.size());

  // For backward compatibility: If no | is found in URL, use the most common working config
  if (m_licenseUrl.find('|') == std::string::npos)
    m_licenseUrl += "|Content-Type=application%2Foctet-stream|R{SSM}|";

  //wv_adapter->GetStatusForPolicy();
  //wv_adapter->QueryOutputProtectionStatus();
}

void CWVCdmAdapter::Initialize()
{
  //curl_global_init(CURL_GLOBAL_DEFAULT);
  if (widevine::Cdm::kSuccess == widevine::Cdm::initialize(widevine::Cdm::kNoSecureOutput, &_host,
                                                           &_host, &_host, widevine::Cdm::kInfo))
  {
    wv_adapter.reset(widevine::Cdm::create(this, &_host, false));
    if (!_host.exists("cert.bin"))
    {
      Provision(wv_adapter.get());
    }
  }
}

CWVCdmAdapter::~CWVCdmAdapter()
{
  if (wv_adapter)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto it : ssds)
    {
      it->CloseSessionId();
    }
    LOG::Log(LOGERROR, "Instances: %u", wv_adapter.use_count());
    wv_adapter = nullptr;
  }
}

void CWVCdmAdapter::onMessage(const std::string& session_id,
                              widevine::Cdm::MessageType f_messageType,
                              const std::string& f_message)
{

  std::lock_guard<std::mutex> lock(_mutex);
  std::vector<CWVCencSingleSampleDecrypter*>::iterator res =
      std::find_if(ssds.begin(), ssds.end(),
                   [session_id](auto it) { return std::string(it->GetSessionId()) == session_id; });
  if (res != ssds.end())
    (*res)->onMessage(f_messageType, f_message);
}

void CWVCdmAdapter::onKeyStatusesChange(const std::string& session_id, bool has_new_usable_key)
{
}

void CWVCdmAdapter::onRemoveComplete(const std::string& session_id)
{

  std::lock_guard<std::mutex> lock(_mutex);

  // SessionMap::iterator index(_sessions.find(session_id));

  // if (index != _sessions.end())
  //   index->second->onRemoveComplete();
}
