#include "Provision.h"

#include "RemoteRequest.h"

#include <cstring>

#include <unistd.h>
#include <widevine/cdm.h>
// #include <gst/gst.h>

// Get device certificate
const std::string provisioning_server_ = "https://www.googleapis.com/"
                                         "certificateprovisioning/v1/devicecertificates/create"
                                         "?key=AIzaSyB-5OLKTx2iU5mko18DfdwK5611JIjbUhE";

const int kHttpOk = 200;

bool FetchCertificate(const std::string& url, std::string* response)
{
  bool status;
  auto callback = [&status, response](uint32_t status_code, const std::string& body)
  {
    *response = body;
    if (status_code == kHttpOk)
    {
      status = true;
    }
    else
    {
      LOG::Log(LOGERROR, "FetchCertificate status code = %d", status_code);
      status = false;
    }
  };
  RemoteRequest request(url, callback);
  request.SubmitSync();
  return status;
}

std::string GetProvisioningResponse(const std::string& message)
{
  std::string reply;
  std::string uri = provisioning_server_;

  LOG::Log(LOGDEBUG, "GetProvisioningResponse: URI: %s", uri.c_str());
  LOG::Log(LOGDEBUG, "GetProvisioningResponse: message:\n%s\n", message.c_str());

  uri += "&signedRequest=" + message;
  if (!FetchCertificate(uri, &reply))
  {
    LOG::Log(LOGERROR, "GetProvisioningResponse: Failed.");
    return "";
  }
  LOG::Log(LOGDEBUG, "GetProvisioningResponse: response:\n%s\n", reply.c_str());
  return reply;
}

void Provision(widevine::Cdm* cdm)
{
  std::string message;
  widevine::Cdm::Status status = cdm->getProvisioningRequest(&message);
  std::string reply = GetProvisioningResponse(message);
  status = cdm->handleProvisioningResponse(reply);
}
