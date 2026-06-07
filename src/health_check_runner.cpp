#include "health_check_runner.hpp"

HealthCheckRunner::HealthCheckRunner(
    std::shared_ptr<spdlog::logger> logger,
    MonitorFactory monitor_factory
): logger(std::move(logger)), monitor_factory(monitor_factory) {}

void HealthCheckRunner::run(const Config &config) {
    std::vector<std::unique_ptr<IEndpointMonitor>> monitors = build_monitors(config);

    std::vector<HealthCheckResult> results;
    results.reserve(monitors.size());

    for (const std::unique_ptr<IEndpointMonitor> &monitor : monitors) {
        const HealthCheckResult result = monitor->health_check();
        results.push_back(result);
    }

    for (const HealthCheckResult &result : results) {
        if (result.success) {
            logger->info(
                "['{}{}'] OK | status codes: '{}' | retries: '{}' | latencies: '{}' ms",
                result.url,
                result.path,
                result.status_codes | std::views::transform([](int code) { return std::to_string(code); }) | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>(),
                result.retries,
                result.latencies_ms | std::views::transform([](double latency_ms) { return std::to_string(latency_ms); }) | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>()
            );
        } else {
            logger->warn(
                "['{}{}'] FAIL | status codes='{}' | retries: '{}' | latencies: '{}' ms | status messages: '{}'",
                result.url,
                result.path,
                result.status_codes | std::views::transform([](int code) { return std::to_string(code); }) | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>(),
                result.retries,
                result.latencies_ms | std::views::transform([](double latency_ms) { return std::to_string(latency_ms); }) | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>(),
                result.status_messages | std::views::filter([](const std::string& s) {  return !s.empty();})| std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>()
            );
        }
    }
    return;
}

std::vector<std::unique_ptr<IEndpointMonitor>> HealthCheckRunner::build_monitors(const Config &config) {

    std::vector<std::unique_ptr<IEndpointMonitor>> monitors;
    monitors.reserve(config.get_apps().size());

    for (const Config::App &app : config.get_apps()) {
        monitors.emplace_back(monitor_factory(app));
    }

    return monitors;
}