#include "../include/http_endpoint_monitor.hpp"

HttpEndpointMonitor::HttpEndpointMonitor(
    const std::shared_ptr<spdlog::logger> &logger, 
    const Settings &settings
): IEndpointMonitor(settings), logger(logger) {}

std::function<httplib::Result(const std::string&)> HttpEndpointMonitor::get_request_based_on_type(httplib::Client &client) {
    std::function<httplib::Result(const std::string&)> request_type_function;

    switch (settings.request_type) {
        case RequestType::HTTP_GET:
            request_type_function = [&](const std::string &path) { return client.Get(path.c_str()); };
            break;

        case RequestType::HTTP_POST:
            request_type_function = [&](const std::string &path) { return client.Post(path.c_str()); };
            break;

        case RequestType::HTTP_PUT:
            request_type_function = [&](const std::string &path) { return client.Put(path.c_str()); };
            break;

        case RequestType::HTTP_PATCH:
            request_type_function = [&](const std::string &path) { return client.Patch(path.c_str()); };
            break;

        case RequestType::HTTP_DELETE:
            request_type_function = [&](const std::string &path) { return client.Delete(path.c_str()); };
            break;

        case RequestType::HTTP_HEAD:
            request_type_function = [&](const std::string &path) {
                return client.Head(path.c_str());
            };
            break;

        case RequestType::HTTP_OPTIONS:
            request_type_function = [&](const std::string &path) {
                return client.Options(path.c_str());
            };
            break;

        default:
            throw std::runtime_error("Unsupported request type");
    }

    return request_type_function;
}

HttpEndpointMonitor::Response HttpEndpointMonitor::perform_request(const std::string &url, const std::string &path) {
    httplib::Client client(url);

    client.set_connection_timeout(settings.timeout_seconds, 0);
    client.set_read_timeout(settings.timeout_seconds, 0);
    client.set_write_timeout(settings.timeout_seconds, 0);

    std::function<httplib::Result(const std::string&)> request_type_function = get_request_based_on_type(client);

    httplib::Result result = request_type_function(path);

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