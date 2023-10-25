#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std::chrono;

class Timer {
public:
  Timer(int64_t delay_ms, widevine::Cdm::ITimer::IClient* client, void* context)
  : delay_ms_(delay_ms)
  , client_(client)
  , context_(context) {
    start();
  }

  ~Timer() {
    stop();
  }

private:

  void start() {
    auto f = [&] {
      std::unique_lock<std::mutex> lock(mutex_);
      running_ = true;
      cv_.wait_for(lock, delay_ms_*1ms, [this]{return !running_;});
      if(running_) {
        client_->onTimerExpired(context_);
      }
      running_ = false;
    };

    t_ = std::unique_ptr<std::thread>(new std::thread(f));
  }

  void stop() {
    if(t_) {
      std::unique_lock<std::mutex> lock(mutex_);
      if(running_) {
        running_ = false;
        cv_.notify_all();
      }
      lock.unlock();
      t_->join();
    }
  }
  std::unique_ptr<std::thread> t_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool running_;

  int64_t delay_ms_;
  widevine::Cdm::ITimer::IClient *client_;
  void *context_;
};