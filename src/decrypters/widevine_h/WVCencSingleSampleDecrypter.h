/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../IDecrypter.h"
#include "common/AdaptiveCencSampleDecrypter.h"

#include <list>
#include <mutex>
#include <optional>

#include <widevine/cdm.h>

class CWVDecrypter;
class CWVCdmAdapter;

// namespace media
// {
// class CdmVideoFrame;
// }

using namespace DRM;

class ATTR_DLL_LOCAL CWVCencSingleSampleDecrypter : public Adaptive_CencSingleSampleDecrypter
{
public:
  // methods
  CWVCencSingleSampleDecrypter(CWVCdmAdapter& drm,
                               std::vector<uint8_t>& pssh,
                               std::string_view defaultKeyId,
                               bool skipSessionMessage,
                               CryptoMode cryptoMode,
                               CWVDecrypter* host);
  virtual ~CWVCencSingleSampleDecrypter();

  void GetCapabilities(std::string_view keyId,
                       uint32_t media,
                       IDecrypter::DecrypterCapabilites& caps);
  virtual const char* GetSessionId() override;
  void CloseSessionId();
  AP4_DataBuffer GetChallengeData();

  void SetSession(const char* session, uint32_t sessionSize, const uint8_t* data, size_t dataSize);

  void AddSessionKey(const uint8_t* data, size_t dataSize, uint32_t status);
  bool HasKeyId(std::string_view keyid);

  virtual AP4_Result SetFragmentInfo(AP4_UI32 poolId,
                                     const std::vector<uint8_t>& keyId,
                                     const AP4_UI08 nalLengthSize,
                                     AP4_DataBuffer& annexbSpsPps,
                                     AP4_UI32 flags,
                                     CryptoInfo cryptoInfo) override;
  virtual AP4_UI32 AddPool() override;
  virtual void RemovePool(AP4_UI32 poolId) override;

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
      const AP4_UI32* bytesOfEncryptedData) override;

  // bool OpenVideoDecoder(const VIDEOCODEC_INITDATA* initData);
  // VIDEOCODEC_RETVAL DecryptAndDecodeVideo(kodi::addon::CInstanceVideoCodec* codecInstance,
  //                                         const DEMUX_PACKET* sample);
  // VIDEOCODEC_RETVAL VideoFrameDataToPicture(kodi::addon::CInstanceVideoCodec* codecInstance,
  //                                           VIDEOCODEC_PICTURE* picture);
  // void ResetVideo();

  bool Decrypt(const DEMUX_PACKET* sampleIn);

  void SetDefaultKeyId(std::string_view keyId) override;
  void AddKeyId(std::string_view keyId) override;

  bool Decrypt(const widevine::Cdm::EncryptionScheme encryptionScheme,
               const widevine::Cdm::Pattern& pattern,
               const uint8_t* f_pbIV,
               uint32_t f_cbIV,
               uint8_t* f_pbData,
               uint32_t f_cbData,
               uint32_t* f_pcbOpaqueClearContent,
               uint8_t** f_ppbOpaqueClearContent,
               const uint8_t keyIdLength,
               const uint8_t* keyId,
               bool initWithLast15);


  widevine::Cdm::KeyStatus Status(const uint8_t keyIDLength, const uint8_t keyId[]) const;

  // Callback Interfaces from widevine::IClientNotification
  // -------------------------------------------------------
  void onMessage(widevine::Cdm::MessageType f_messageType, const std::string& f_message);
  void onKeyStatusChange();
  void onRemoveComplete();

private:
  void CheckLicenseRenewal();
  bool SendSessionMessage();

  CWVCdmAdapter& m_wvCdmAdapter;
  std::string m_strSession;
  std::vector<uint8_t> m_pssh;
  AP4_DataBuffer m_challenge;
  std::string m_defaultKeyId;
  struct WVSKEY
  {
    bool operator==(WVSKEY const& other) const { return m_keyId == other.m_keyId; };
    std::string m_keyId;
    widevine::Cdm::KeyStatus status;
  };
  std::vector<WVSKEY> m_keys;
  uint8_t m_IV[16];

  AP4_UI16 m_hdcpVersion;
  int m_hdcpLimit;
  int m_resolutionLimit;

  AP4_DataBuffer m_decryptIn;
  AP4_DataBuffer m_decryptOut;

  struct FINFO
  {
    std::vector<uint8_t> m_key;
    AP4_UI08 m_nalLengthSize;
    AP4_UI16 m_decrypterFlags;
    AP4_DataBuffer m_annexbSpsPps;
    CryptoInfo m_cryptoInfo;
  };
  std::vector<FINFO> m_fragmentPool;
  void LogDecryptError(const widevine::Cdm::KeyStatus, const std::vector<uint8_t>& keyId);
  void SetCdmSubsamples(std::vector<widevine::Cdm::Subsample>& subsamples, bool isCbc);
  void RepackSubsampleData(AP4_DataBuffer& dataIn,
                           AP4_DataBuffer& dataOut,
                           size_t& startPos,
                           size_t& cipherPos,
                           const unsigned int subsamplePos,
                           const AP4_UI16* bytesOfCleartextData,
                           const AP4_UI32* bytesOfEncryptedData);
  void UnpackSubsampleData(AP4_DataBuffer& dataIn,
                           size_t& startPos,
                           const unsigned int subsamplePos,
                           const AP4_UI16* bytesOfCleartextData,
                           const AP4_UI32* bytesOfEncryptedData);
  void SetInput(widevine::Cdm::InputBuffer& cdmInputBuffer,
                const AP4_DataBuffer& inputData,
                const unsigned int subsampleCount,
                const uint8_t* iv,
                const FINFO& fragInfo,
                const std::vector<widevine::Cdm::Subsample>& subsamples);
  uint32_t m_promiseId;
  bool m_isDrained;

  // std::list<media::CdmVideoFrame> m_videoFrames;
  std::mutex m_renewalLock;
  CryptoMode m_EncryptionMode;

  // std::optional<cdm::VideoDecoderConfig_3> m_currentVideoDecConfig;
  CWVDecrypter* m_host;
};
