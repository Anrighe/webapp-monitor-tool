#pragma once

#include <filesystem>
#include <memory>

#include "libraries/spdlog/logger.h"

class Logger {
public:
    static std::shared_ptr<spdlog::logger> initialize(
        const std::filesystem::path& log_path,
        int max_size_bytes
    );

private:
    static constexpr int MAX_LOG_FILES = 3;

    static void ensure_log_file_exists(const std::filesystem::path& log_path);
};