#include "../include/http_endpoint_monitor.hpp"

HttpEndpointMonitor::Response HttpEndpointMonitor::perform_request(const std::string &path) {
    httplib::Client client(settings.host);

    client.set_connection_timeout(settings.timeout_seconds);
    client.set_read_timeout(settings.timeout_seconds);
    client.set_write_timeout(settings.timeout_seconds);

    // TODO: handle request type
    httplib::Result result = client.Get("/");

    if (!result) {
        Response response;
        response.status_message = httplib::to_string(result.error());
        return response;
    }

    return parse_response(result);
}

HttpEndpointMonitor::Response HttpEndpointMonitor::parse_response(const httplib::Result &result) {
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

    return Response {
        .success = result->status == settings.healthy_response_status_code,
        .status_code = result->status,
        .body = json::object(),
        .status_message = ""
    };
}

bool HttpEndpointMonitor::is_response_valid(const Response &response) const {
    return response.status_code;
}