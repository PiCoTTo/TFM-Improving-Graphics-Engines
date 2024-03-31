#pragma once

#include <memory>
#include "spdlog/spdlog.h"

namespace nimo
{
    class Log{
    public:
        enum class Level : unsigned char {
            Debug = 0,
            Info,
            Warn,
            Error,
            Critical,
        };
        static void Initialize();
        static void Cleanup();
	    template<typename... Args>
        static void Print(Level level, Args&&... args);
    private:
        static std::shared_ptr<spdlog::logger> m_engineLogger;
    };
} // namespace nimo

// Logging macros
#define NIMO_DEBUG(...)  ::nimo::Log::Print(::nimo::Log::Level::Debug, __VA_ARGS__)
#define NIMO_INFO(...)   ::nimo::Log::Print(::nimo::Log::Level::Info, __VA_ARGS__)
#define NIMO_WARN(...)   ::nimo::Log::Print(::nimo::Log::Level::Warn, __VA_ARGS__)
#define NIMO_ERROR(...)  ::nimo::Log::Print(::nimo::Log::Level::Error, __VA_ARGS__)
#define NIMO_CRITICAL(...)  ::nimo::Log::Print(::nimo::Log::Level::Critical, __VA_ARGS__)

#include "events/LogEvents.h"
#include "events/EventManager.h"

template<typename... Args>
void nimo::Log::Print(Level level, Args&&... args)
{
    if (!m_engineLogger) return;
    auto msg = fmt::format(std::forward<Args>(args)...);
    switch (level)
    {
    case Level::Debug:
        m_engineLogger->trace(msg);
        break;
    case Level::Info:
        m_engineLogger->info(msg);
        break;
    case Level::Warn:
        m_engineLogger->warn(msg);
        break;
    case Level::Error:
        m_engineLogger->error(msg);
        break;
    case Level::Critical:
        m_engineLogger->critical(msg);
        break;
    }
    EventManager::Publish(LogEvent(level, msg));
}
