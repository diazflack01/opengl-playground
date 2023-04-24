#include "ScopedTimer.hpp"

#include <fmt/chrono.h>

ScopedTimer::ScopedTimer(std::string name): mTimerName{std::move(name)}, mTimerStarted{std::chrono::high_resolution_clock::now()} {

}

ScopedTimer::~ScopedTimer() {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto elapsedTime = now - mTimerStarted;
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime);
    const auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);
    const auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime);
    const auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime);
    fmt::println("ScopedTimer elapsed time -  {} sec / {} ms / {} us", sec, millisec, microsec);
}
