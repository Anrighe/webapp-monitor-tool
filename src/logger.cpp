#include "../include/logger.hpp"

#include "../include/libraries/spdlog/spdlog.h"
#include "../include/libraries/spdlog/sinks/stdout_color_sinks.h"
#include "../include/libraries/spdlog/sinks/rotating_file_sink.h"

#include <format>
#include <fstream>
#include <stdexcept>

std::shared_ptr<spdlog::logger> Logger::initialize(
    const std::filesystem::path &log_path,
    int max_size_bytes
) {
    if (max_size_bytes <= 0) {
        throw std::runtime_error("max_size_bytes must be greater than 0");
    }

    ensure_log_file_exists(log_path);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_path.string(),
        static_cast<std::size_t>(max_size_bytes),
        MAX_LOG_FILES
    );

    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] %v");
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread:%t] [%n] [%l] %v");

    auto logger = std::make_shared<spdlog::logger>(
        "webapp-monitor-tool",
        spdlog::sinks_init_list{console_sink, file_sink}
    );

    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::warn);

    spdlog::register_logger(logger);

    return logger;
}

void Logger::ensure_log_file_exists(const std::filesystem::path &log_path) {
    const std::filesystem::path parent_path = log_path.parent_path();

    if (!parent_path.empty()) {
        std::filesystem::create_directories(parent_path);
    }

    if (!std::filesystem::exists(log_path)) {
        std::ofstream file(log_path, std::ios::app);

        if (!file) {
            throw std::runtime_error(
                std::format("Could not create log file: {}", log_path.string())
            );
        }
    }
}