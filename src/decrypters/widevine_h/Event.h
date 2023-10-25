#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std::chrono;

class Event {
public:
    Event() : _flag(false) {}
    void SetEvent() {
        std::lock_guard<std::mutex> lock(_mutex);
        _flag = true;
        _cv.notify_all();
    }
    // timeout is in milliseconds
    // returns true if the flag has been set
    bool Lock(uint32_t timeout) {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_flag) {
            return true;
        }
        if(_cv.wait_for(lock, timeout*1ms, [&]{return _flag;})) {
            return true;
        }
        return false;
    }
    void ResetEvent() {
        std::lock_guard<std::mutex> lock(_mutex);
        _flag = false;
    }
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _flag;
};