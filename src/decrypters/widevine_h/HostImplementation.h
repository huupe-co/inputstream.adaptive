#pragma once

#include <widevine/cdm.h>
#include "FileStore.h"
#include "Timer.h"
#include <assert.h>

using namespace widevine;
using namespace std::chrono;

namespace CDMi {

class HostImplementation :
  public widevine::Cdm::IStorage,
  public widevine::Cdm::IClock,
  public widevine::Cdm::ITimer {

public:

  HostImplementation(HostImplementation&) = delete;
  HostImplementation& operator= (HostImplementation&) = delete;

  HostImplementation();
  ~HostImplementation() override;

public:

  // note this method is not thread safe regarding simultanious widevine::Cdm::IStorage callbacks, make sure they cannot be not active when calling this
  void PreloadFile(const std::string& filename, std::string&& filecontent );

  //
  // widevine::Cdm::IStorage implementation
  // ---------------------------------------------------------------------------
  virtual bool read(const std::string& name, std::string* data) override;
  virtual bool write(const std::string& name, const std::string& data) override;
  virtual bool exists(const std::string& name) override;
  virtual bool remove(const std::string& name) override;
  virtual int32_t size(const std::string& name) override;
  virtual bool list(std::vector<std::string>* names) override;

  // widevine::Cdm::IClock implementation
  // ---------------------------------------------------------------------------
  virtual int64_t now() override;

  // widevine::Cdm::ITimer implementation
  // ---------------------------------------------------------------------------
  virtual void setTimeout(int64_t delay_ms, IClient* client, void* context) override;
  virtual void cancel(IClient* client) override;

private:
  std::string base_path_;
  std::unique_ptr<FileSystem> file_system_;

  std::map<IClient *, std::unique_ptr<Timer>> timers_;
};

} // namespace CDMi
