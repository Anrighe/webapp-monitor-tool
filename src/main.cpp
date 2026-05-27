#include <iostream>
#include <fstream>

#include "../include/config.hpp"
#include "../include/logger.hpp"

#include "../include/libraries/spdlog/logger.h"

#define CONFIG_FILE_NAME "config.json"

int main() {
    std::shared_ptr<spdlog::logger> logger;

    try {
        std::shared_ptr<Config> config = std::make_shared<Config>(Config::loadConfig(CONFIG_FILE_NAME));
        logger = Logger::initialize(
            config->get_logs_path(),
            config->get_max_logs_filesize_byte()
        );
        
        logger->info("Config loaded successfully");
    } catch (const std::exception& error) {
        if (logger) {
            logger->error("Failed to load config: {}", error.what());
        } else {
            std::cerr<<"Failed before logger initialization: "<< error.what()<<std::endl;
        }

        return 1;
    }

    return 0;
}