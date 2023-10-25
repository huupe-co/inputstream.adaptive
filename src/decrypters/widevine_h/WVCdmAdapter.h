/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "HostImplementation.h"
#include "Provision.h"

#include <bento4/Ap4.h>
#include <kodi/addon-instance/VideoCodec.h>

class CWVDecrypter;
class CWVCencSingleSampleDecrypter;

class ATTR_DLL_LOCAL CWVCdmAdapter : public widevine::Cdm::IEventListener
{
public:
  CWVCdmAdapter(std::string_view licenseURL,
                const std::vector<uint8_t>& serverCert,
                const uint8_t config,
                CWVDecrypter* host);
  virtual ~CWVCdmAdapter();

  void Initialize();

  virtual void onMessage(const std::string& session_id,
                         widevine::Cdm::MessageType f_messageType,
                         const std::string& f_message);

  virtual void onKeyStatusesChange(const std::string& session_id, bool has_new_usable_key);
  virtual void onRemoveComplete(const std::string& session_id);

  void insertssd(CWVCencSingleSampleDecrypter* ssd) { ssds.push_back(ssd); };
  void removessd(CWVCencSingleSampleDecrypter* ssd)
  {
    std::vector<CWVCencSingleSampleDecrypter*>::iterator res(
        std::find(ssds.begin(), ssds.end(), ssd));
    if (res != ssds.end())
      ssds.erase(res);
  };

  widevine::Cdm* GetCdmAdapter() { return wv_adapter.get(); };
  const std::string& GetLicenseURL() { return m_licenseUrl; };

private:
  std::shared_ptr<widevine::Cdm> wv_adapter;
  CDMi::HostImplementation _host;
  std::mutex _mutex;

  std::string m_licenseUrl;
  CWVDecrypter* m_host;
  std::vector<CWVCencSingleSampleDecrypter*> ssds;
};
