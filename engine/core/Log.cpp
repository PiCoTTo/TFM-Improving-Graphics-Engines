#include "Log.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/dist_sink.h"

std::shared_ptr<spdlog::logger> nimo::Log::m_engineLogger;

void nimo::Log::Initialize()
{
    std::vector<spdlog::sink_ptr> sinks =
    {
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>("nimo.log", 1048576 * 100, 99),
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
    };
    sinks[0]->set_pattern("[%Y-%m-%d %T.%e] [%l] %n: %v%$");
    sinks[1]->set_pattern("%^[%Y-%m-%d %T.%e] %n: %v%$");
    auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>(sinks);
    m_engineLogger = std::make_shared<spdlog::logger>("nimo", dist_sink);
    m_engineLogger->set_level(spdlog::level::trace);
}
void nimo::Log::Cleanup()
{
    m_engineLogger->flush();
    spdlog::drop_all();
    m_engineLogger.reset();
}

