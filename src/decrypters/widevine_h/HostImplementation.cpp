//#include <gst/gst.h>
#include "HostImplementation.h"

#include "utils/log.h"

#include <widevine/properties.h>
#include <widevine/wv_cdm_types.h>

using namespace widevine;

namespace CDMi
{

HostImplementation::HostImplementation()
  : widevine::Cdm::IStorage(),
    widevine::Cdm::IClock(),
    widevine::Cdm::ITimer(),
    file_system_(new FileSystem())
{
  base_path_ = "/mnt/data/l3/";
  LOG::Log(LOGINFO, "Base Path %s\n", base_path_.c_str());
}

HostImplementation::~HostImplementation()
{
}

void HostImplementation::PreloadFile(const std::string& filename, std::string&& filecontent)
{
}

// widevine::Cdm::IStorage implementation
// ---------------------------------------------------------------------------
/* virtual */ bool HostImplementation::read(const std::string& name, std::string* data)
{
  const std::string path = base_path_ + name;
  LOG::Log(LOGINFO, "read file: %s", path.c_str());
  auto file = file_system_->Open(path, FileSystem::kReadOnly);
  ssize_t bytes_read = 0;
  if (file)
  {
    ssize_t file_size = file_system_->FileSize(path);
    char* buffer = (char*)malloc(file_size);
    bytes_read = file->Read(buffer, file_size);
    *data = std::string(buffer, bytes_read);
    free(buffer);
    return true;
  }
  return false;
}

/* virtual */ bool HostImplementation::write(const std::string& name, const std::string& data)
{
  const std::string path = base_path_ + name;
  LOG::Log(LOGINFO, "write file: %s", path.c_str());
  auto file = file_system_->Open(path, FileSystem::kCreate | FileSystem::kTruncate);
  ssize_t bytes_written = 0;
  if (file)
  {
    bytes_written = file->Write(data.c_str(), data.size());
  }
  return true;
}

/* virtual */ bool HostImplementation::exists(const std::string& name)
{
  const std::string path = base_path_ + name;
  LOG::Log(LOGINFO, "exists: %s", path.c_str());
  return file_system_->Exists(path);
}

/* virtual */ bool HostImplementation::remove(const std::string& name)
{
  const std::string path = base_path_ + name;
  LOG::Log(LOGINFO, "remove: %s", path.c_str());
  return file_system_->Remove(path);
}

/* virtual */ int32_t HostImplementation::size(const std::string& name)
{
  const std::string path = base_path_ + name;
  return file_system_->FileSize(path);
}

/* virtual */ bool HostImplementation::list(std::vector<std::string>* names)
{
  return file_system_->List(base_path_, names);
}

// widevine::Cdm::IClock implementation
// ---------------------------------------------------------------------------
int64_t HostImplementation::now()
{
  auto now = system_clock::now();
  auto time_since_epoch = now.time_since_epoch();
  return duration_cast<milliseconds>(time_since_epoch).count();
}

// widevine::Cdm::ITimer implementation
// ---------------------------------------------------------------------------
void HostImplementation::setTimeout(int64_t delay_ms, IClient* client, void* context)
{
  timers_[client].reset(new Timer(delay_ms, client, context));
}

void HostImplementation::cancel(IClient* client)
{
  timers_[client] = nullptr;
}

} // namespace CDMi
