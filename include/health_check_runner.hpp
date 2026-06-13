#pragma once

#include <vector>
#include <memory>
#include <string>
#include <ranges>

#include "i_endpoint_monitor.hpp"
#include "health_check_result.hpp"
#include "spdlog/logger.h"
#include "config.hpp"
#include "text_formatter.hpp"
#include "../include/email_sender.hpp"

using MonitorFactory = std::function<std::unique_ptr<IEndpointMonitor>(const Config::App&)>;

class HealthCheckRunner {
public:

    /**
     * @param logger Shared logger instance
     * @param monitor_factory Factory function that creates a monitor per app
     */
    explicit HealthCheckRunner(std::shared_ptr<spdlog::logger> logger, MonitorFactory monitor_factory, std::shared_ptr<EmailSender> email_sender);

    /**
     * Builds monitors for all configured apps and runs a health check on each
     * @param config Loaded application configuration
     */
    void run(const Config &config);

private:
    std::shared_ptr<spdlog::logger> logger;
    MonitorFactory monitor_factory;
    std::shared_ptr<EmailSender> email_sender;

    /**
     * Instantiates a monitor for each app in the config using the monitor factory
     * @param config Loaded application configuration
     * @return List of owning pointers to the constructed monitors
     */
    std::vector<std::unique_ptr<IEndpointMonitor>> build_monitors(const Config &config);
};