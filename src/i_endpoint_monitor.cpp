#include "../include/i_endpoint_monitor.hpp"

IEndpointMonitor::IEndpointMonitor(const Settings &settings): settings(settings) {}

void IEndpointMonitor::increase_current_retry() {
    ++current_retry;
}

bool IEndpointMonitor::has_exceeded_max_retries() const {
    return current_retry >= settings.max_retries;
}

void IEndpointMonitor::reset_current_retry() {
    current_retry = 0;
}

bool IEndpointMonitor::is_response_valid(const Response &response) const {
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

std::optional<std::string> IEndpointMonitor::request_type_to_string(const IEndpointMonitor::RequestType requestType) {
    return request_types_to_string_map.contains(requestType)
        ? std::optional<std::string>{request_types_to_string_map.at(requestType)}
        : std::nullopt;
}

HealthCheckResult IEndpointMonitor::health_check() {

    reset_current_retry();
    bool first_request = true;

    std::vector<int> response_status_codes;
    response_status_codes.reserve(settings.max_retries);

    std::vector<std::string> status_messages;
    status_messages.reserve(settings.max_retries);

    std::vector<double> latencies_ms;
    latencies_ms.reserve(settings.max_retries);

    do {
        if (!first_request) {
            apply_timeout();
        } else {
            first_request = false;
        }
        increase_current_retry();

        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

        const Response response = perform_request(settings.url, settings.path);
        
        std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
        
        response_status_codes.emplace_back(response.status_code);
        status_messages.emplace_back(response.status_message);

        latencies_ms.emplace_back(std::chrono::duration<double, std::milli>(end - start).count());

        if (is_response_valid(response)) {

            return {
                .url = settings.url,
                .path = settings.path,
                .success = true,
                .status_codes = response_status_codes,
                .retries = current_retry,
                .latencies_ms = latencies_ms,
                .status_messages = status_messages
            };
        }

    } while (!has_exceeded_max_retries());

    return {
        .url = settings.url,
        .path = settings.path,
        .success = false,
        .status_codes = response_status_codes,
        .retries = current_retry,
        .latencies_ms = latencies_ms,
        .status_messages = status_messages
    };
}

void IEndpointMonitor::apply_timeout() const {
    std::this_thread::sleep_for(
        std::chrono::seconds(settings.timeout_seconds)
    );
}