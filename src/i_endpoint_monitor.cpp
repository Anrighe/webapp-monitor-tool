#include "../include/i_endpoint_monitor.hpp"

void IEndpointMonitor::increase_current_retry() {
    ++current_retry;
}

bool IEndpointMonitor::has_exceeded_max_retries() const {
    return current_retry > settings.max_retries;
}

void IEndpointMonitor::reset_current_retry() {
    current_retry = 0;
}

bool IEndpointMonitor::is_response_valid(const Response& response) const {
    return response.success && is_response_status_code_healthy(response.status_code);
}

bool IEndpointMonitor::is_response_status_code_healthy(int response_status_code) const {
    return response_status_code == settings.healthy_response_status_code;
}

bool IEndpointMonitor::health_check(const std::string& path)
{
    reset_current_retry();
    bool first_request = true;

    do
    {
        if (!first_request) {
            apply_timeout();
        } else {
            first_request = false;
        }

        increase_current_retry();

        const Response response = perform_request(path);

        if (is_response_valid(response)) {
            return true;
        }

    } while (!has_exceeded_max_retries());

    return false;
}