#pragma once

#include <filesystem>
#include <memory>

#include "libraries/spdlog/logger.h"

class Logger {
public:

    /**
     * Creates a logger with console and rotating file sinks
     * @param log_path Path to the log file
     * @param max_size_bytes Maximum size in bytes before the log file rotates
     * @return Shared logger instance registered with spdlog
     */
    static std::shared_ptr<spdlog::logger> initialize(
        const std::filesystem::path& log_path,
        int max_size_bytes
    );

private:
    static constexpr int MAX_LOG_FILES = 3;

    static void ensure_log_file_exists(const std::filesystem::path& log_path);
};