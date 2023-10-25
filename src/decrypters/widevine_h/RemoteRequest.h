#include <functional>
#include <string>
#include <thread>
#include <vector>

#include <curl/curl.h>
// #include <gst/gst.h>
#include "utils/log.h"

class RemoteRequest
{
public:
  using RemoteResponseCallback = std::function<void(uint32_t code, const std::string& body)>;

  RemoteRequest() = delete;

  RemoteRequest(const std::string& url, RemoteResponseCallback callback)
    : _url(url), _body(std::vector<uint8_t>()), _hs(nullptr), _callback(callback)
  {
  }

  RemoteRequest(const std::string& url,
                const std::vector<uint8_t>& body,
                RemoteResponseCallback callback)
    : _url(url), _body(body), _hs(nullptr), _callback(callback)
  {
  }

  RemoteRequest(const std::string& url,
                const std::vector<uint8_t>& body,
                std::vector<std::string> headers,
                RemoteResponseCallback callback)
    : _url(url), _body(body), _hs(nullptr), _callback(callback)
  {
    for (auto& header : headers)
    {
      _hs = curl_slist_append(_hs, header.c_str());
    }
  }

  RemoteRequest(RemoteRequest&) = default;
  RemoteRequest& operator=(const RemoteRequest&) = default;
  RemoteRequest(RemoteRequest&&) = default;
  RemoteRequest& operator=(RemoteRequest&&) = default;

  void SubmitAsync()
  {
    _requestThread = std::unique_ptr<std::thread>(new std::thread([&]() { Perform(); }));
  }

  void SubmitSync() { Perform(); }

  ~RemoteRequest()
  {
    if (_requestThread)
    {
      _requestThread->join();
    }
  };

private:
  static size_t CurlWriteCallback(char* contents, size_t size, size_t nmemb, void* userp)
  {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
  }

  void Perform()
  {
    _curlHandle = curl_easy_init();
    if (_curlHandle)
    {
      std::string outputBuffer;
      curl_easy_setopt(_curlHandle, CURLOPT_URL, _url.c_str());
      curl_easy_setopt(_curlHandle, CURLOPT_POSTFIELDS, _body.data());
      curl_easy_setopt(_curlHandle, CURLOPT_POSTFIELDSIZE, _body.size());
      curl_easy_setopt(_curlHandle, CURLOPT_HTTPHEADER, _hs);
      curl_easy_setopt(_curlHandle, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
      curl_easy_setopt(_curlHandle, CURLOPT_WRITEDATA, &outputBuffer);
      curl_easy_setopt(_curlHandle, CURLOPT_TIMEOUT, 10);
      curl_easy_perform(_curlHandle);

      long responseCode = 0;
      curl_easy_getinfo(_curlHandle, CURLINFO_RESPONSE_CODE, &responseCode);
      curl_easy_cleanup(_curlHandle);

      _callback(responseCode, outputBuffer);
    }
    else
    {
      LOG::LogF(LOGDEBUG, "Could not initialize libcurl.");
    }
  }

  const std::string _url;
  const std::vector<uint8_t> _body;
  const RemoteResponseCallback _callback;

  curl_slist* _hs;
  std::unique_ptr<std::thread> _requestThread;
  CURL* _curlHandle;
};
