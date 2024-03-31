#pragma once
#include <chrono>

namespace nimo
{
class Timer
{
public:
    Timer()
    {
        Reset();
    }

    void Timer::Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }
    void Timer::Stop()
    {
        m_end = std::chrono::high_resolution_clock::now();
    }

    float Timer::Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float Timer::ElapsedMillis()
    {
        return Elapsed() * 1000.0f;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
};
}