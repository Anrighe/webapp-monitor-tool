#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "libraries/nlohmann/json.hpp"

using json = nlohmann::json;

class IEndpointMonitor
{
public:
    enum class RequestType {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH,
        OPTIONS,
        HEAD
    };

    struct Response {
        bool success = false;
        int status_code = 0;
        json body = json::object();
        std::string status_message = "";
    };

    struct Settings {
        std::string host;
        RequestType request_type;
        int timeout_seconds;
        int max_retries;
        int healthy_response_status_code;
    };

protected:
    int current_retry = 0;
    Settings settings;

private:
    inline static const std::unordered_map<std::string, RequestType> string_to_request_types_map = {
        {"GET", RequestType::GET},
        {"POST", RequestType::POST},
        {"PUT", RequestType::PUT},
        {"DELETE", RequestType::DELETE},
        {"PATCH", RequestType::PATCH},
        {"OPTIONS", RequestType::OPTIONS},
        {"HEAD", RequestType::HEAD}
    };

    inline static const std::unordered_map<RequestType, std::string> request_types_to_string_map = {
        {RequestType::GET, "GET"},
        {RequestType::POST, "POST"},
        {RequestType::PUT, "PUT"},
        {RequestType::DELETE, "DELETE"},
        {RequestType::PATCH, "PATCH"},
        {RequestType::OPTIONS, "OPTIONS"},
        {RequestType::HEAD, "HEAD"}
    };

public:
    virtual ~IEndpointMonitor() = default;

    IEndpointMonitor() = default;

    // Forbidding copying and moving this base class
    IEndpointMonitor(const IEndpointMonitor &other) = delete;
    IEndpointMonitor& operator=(const IEndpointMonitor& other) = delete;

    IEndpointMonitor(IEndpointMonitor &&other) = delete;
    IEndpointMonitor& operator=(IEndpointMonitor &&other) = delete;

    bool health_check(const std::string &path);

    static std::optional<RequestType> parse_request_type(const std::string &request_type);
    static std::optional<std::string> request_type_to_string(const RequestType requestType);

protected:
    virtual Response perform_request(const std::string &path) = 0;
    virtual bool is_response_valid(const Response &response) const;

private:
    void increase_current_retry();
    void reset_current_retry();
    bool has_exceeded_max_retries() const;
    void apply_timeout() const;
    bool is_response_status_code_healthy(const int response_status_code) const;
};