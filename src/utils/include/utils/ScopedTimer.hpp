#pragma once

#include <chrono>
#include <string>

class ScopedTimer {
public:
    ScopedTimer(std::string name);
    ~ScopedTimer();

private:
    std::string mTimerName;
    std::chrono::high_resolution_clock::time_point mTimerStarted;
};
