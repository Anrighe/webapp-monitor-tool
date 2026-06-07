#include <iostream>
#include <fstream>
#include <vector>

#include "../include/config.hpp"
#include "../include/logger.hpp"

#include "../include/libraries/spdlog/logger.h"
#include "../include/http_endpoint_monitor.hpp"


#define CONFIG_FILE_NAME "config.json"

int main() {
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<Config> config;
    try {
        config = std::make_shared<Config>(Config::loadConfig(CONFIG_FILE_NAME));
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

    std::vector<std::unique_ptr<IEndpointMonitor>> endpoint_monitors;
    endpoint_monitors.reserve(config->get_apps().size());

    std::vector<Config::App> config_apps = config->get_apps();
    for (const Config::App &app : config_apps) {

        HttpEndpointMonitor::Settings settings;

        settings.host = app.url;
        settings.timeout_seconds = app.timeout_seconds;
        settings.max_retries = app.max_retries;
        settings.healthy_response_status_code = app.healthy_response_status_code;
        std::optional<IEndpointMonitor::RequestType> request_type = IEndpointMonitor::parse_request_type(app.request_type);

        endpoint_monitors.emplace_back(
            std::make_unique<HttpEndpointMonitor>(
                logger,
                settings
            )
        );
    }

    return 0;
}