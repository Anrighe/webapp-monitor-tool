#include "../include/http_endpoint_monitor.hpp"

HttpEndpointMonitor::HttpEndpointMonitor(
    const std::shared_ptr<spdlog::logger>& logger, 
    const Settings& settings
): IEndpointMonitor(settings), logger(logger) {}

HttpEndpointMonitor::Response HttpEndpointMonitor::perform_request(const std::string &url, const std::string &path) {
    httplib::Client client(url);

    client.set_connection_timeout(settings.timeout_seconds * 1000);
    client.set_read_timeout(settings.timeout_seconds);
    client.set_write_timeout(settings.timeout_seconds);

    // TODO: handle request type
    httplib::Result result = client.Get(path);

    if (!result) {
        Response response;
        response.status_message = httplib::to_string(result.error());
        return response;
    }

    return parse_response(result);
}

HttpEndpointMonitor::Response HttpEndpointMonitor::parse_response(const httplib::Result &result) {
    
    if (result->body != "" && !result->body.empty()) {
        json parsed_body;
        
        try {
            parsed_body = json::parse(result->body);
        } catch (const json::parse_error &exception) {
            return Response{
                .success = false,
                .status_code = result->status,
                .body = json::object(),
                .status_message = exception.what()
            };
        }
    }

    return Response {
        .success = result->status == settings.healthy_response_status_code,
        .status_code = result->status,
        .body = json::object(),
        .status_message = ""
    };
}