#include <fstream>
#include <vector>

#include "../include/config.hpp"
#include "../include/logger.hpp"

#include "../include/libraries/spdlog/logger.h"
#include "../include/http_endpoint_monitor.hpp"
#include "../include/health_check_runner.hpp"


#define CONFIG_FILE_NAME "config.json"

std::unique_ptr<IEndpointMonitor> create_http_monitor(
    const Config::App& app,
    const std::shared_ptr<spdlog::logger>& logger
) {
    IEndpointMonitor::Settings settings;
    settings.url = app.url;
    settings.path = app.path;
    settings.timeout_seconds = app.timeout_seconds;
    settings.max_retries = app.max_retries;
    settings.healthy_response_status_code = app.healthy_response_status_code;

    auto type = IEndpointMonitor::parse_request_type(app.request_type);
    if (!type) {
        throw std::runtime_error("Invalid request type");
    }

    settings.request_type = *type;

    return std::make_unique<HttpEndpointMonitor>(logger, settings);
}

int main() {
    std::shared_ptr<spdlog::logger> logger;
    std::unique_ptr<Config> config;

    try {
        config = std::make_unique<Config>(Config::loadConfig("config.json"));
        logger = Logger::initialize(
            config->get_logs_path(),
            config->get_max_logs_filesize_byte()
        );
        logger->info("Config loaded successfully");

        HealthCheckRunner runner(
            logger,
            [&](const Config::App& app) { return create_http_monitor(app, logger); }
        );
        logger->info("Runner created successfully");

        runner.run(*config);

    } catch (const std::exception &e) {
        std::cerr<<"Init failed: "<<e.what()<<std::endl;
        return 1;
    }




    return 0;
}