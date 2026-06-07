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

std::optional<IEndpointMonitor::RequestType> IEndpointMonitor::parse_request_type(const std::string &request_type) {
    std::string normalized_request_type = request_type;

    std::transform(
        normalized_request_type.begin(),
        normalized_request_type.end(),
        normalized_request_type.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); }
    );

    return string_to_request_types_map.contains(normalized_request_type)
        ? std::optional<RequestType>{string_to_request_types_map.at(normalized_request_type)}
        : std::nullopt;
}

std::optional<std::string> IEndpointMonitor::request_type_to_string(const IEndpointMonitor::RequestType requestType){
    return request_types_to_string_map.contains(requestType)
        ? std::optional<std::string>{request_types_to_string_map.at(requestType)}
        : std::nullopt;
}

bool IEndpointMonitor::health_check(const std::string& path)
{
    reset_current_retry();
    bool first_request = true;

    do {
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

void IEndpointMonitor::apply_timeout() const
{
    std::this_thread::sleep_for(
        std::chrono::seconds(settings.timeout_seconds)
    );
}