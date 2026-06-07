#pragma once

#include <vector>
#include <memory>
#include <string>
#include <ranges>

#include "i_endpoint_monitor.hpp"
#include "health_check_result.hpp"
#include "spdlog/logger.h"
#include "config.hpp"

using MonitorFactory = std::function<std::unique_ptr<IEndpointMonitor>(const Config::App&)>;

class HealthCheckRunner {
public:
    explicit HealthCheckRunner(std::shared_ptr<spdlog::logger> logger, MonitorFactory monitor_factory);
    void run(const Config &config);

private:
    std::shared_ptr<spdlog::logger> logger;
    MonitorFactory monitor_factory;

    std::vector<std::unique_ptr<IEndpointMonitor>> build_monitors(const Config &config);
};