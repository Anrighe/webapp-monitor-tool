#include <fstream>
#include <vector>

#include "../include/config.hpp"
#include "../include/logger.hpp"

#include "../include/libraries/spdlog/logger.h"
#include "../include/http_endpoint_monitor.hpp"
#include "../include/health_check_runner.hpp"
#include "../include/text_formatter.hpp"
#include "../include/email_sender.hpp"


#define CONFIG_FILE_NAME "config.json"

/**
 * Factory function that constructs an HttpEndpointMonitor from a Config::App
 * @param app App configuration to build the monitor settings from
 * @param logger Shared logger instance passed to the monitor
 * @return Owning pointer to the constructed IEndpointMonitor
 * @throws std::runtime_error if app.request_type is not a valid RequestType
 */
std::unique_ptr<IEndpointMonitor> create_http_monitor(
    const Config::App &app,
    const std::shared_ptr<spdlog::logger> &logger
) {
    IEndpointMonitor::Settings settings;
    settings.url = app.url;
    settings.path = app.path;
    settings.timeout_seconds = app.timeout_seconds;
    settings.max_retries = app.max_retries;
    settings.healthy_response_status_code = app.healthy_response_status_code;

    std::optional<IEndpointMonitor::RequestType> request_type = IEndpointMonitor::parse_request_type(app.request_type);
    if (!request_type) {
        throw std::runtime_error("Invalid request type");
    }

    settings.request_type = *request_type;

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

        std::shared_ptr<EmailSender> email_sender = std::make_shared<EmailSender>(
            config->get_smtp().url,
            config->get_smtp().username,
            config->get_smtp().password,
            config->get_smtp().from,
            logger
        );

        HealthCheckRunner runner(
            logger,
            [&](const Config::App &app) { return create_http_monitor(app, logger); },
            email_sender
        );
        logger->info("Runner created successfully");

        runner.run(*config);

    } catch (const std::exception &e) {
        std::cerr<<"Init failed: "<<TextFormatter::format_red(e.what())<<std::endl;
        return 1;
    }

    return 0;
}