#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "libraries/nlohmann/json.hpp"
#include "health_check_result.hpp"

using json = nlohmann::json;

class IEndpointMonitor
{
public:
    enum class RequestType {
        HTTP_GET,
        HTTP_POST,
        HTTP_PUT,
        HTTP_DELETE,
        HTTP_PATCH,
        HTTP_OPTIONS,
        HTTP_HEAD
    };

    struct Response {
        bool success = false;
        int status_code = 0;
        json body = json::object();
        std::string status_message = "";
    };

    struct Settings {
        std::string url;
        std::string path;
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
        {"GET", RequestType::HTTP_GET},
        {"POST", RequestType::HTTP_POST},
        {"PUT", RequestType::HTTP_PUT},
        {"DELETE", RequestType::HTTP_DELETE},
        {"PATCH", RequestType::HTTP_PATCH},
        {"OPTIONS", RequestType::HTTP_OPTIONS},
        {"HEAD", RequestType::HTTP_HEAD}
    };

    inline static const std::unordered_map<RequestType, std::string> request_types_to_string_map = {
        {RequestType::HTTP_GET, "GET"},
        {RequestType::HTTP_POST, "POST"},
        {RequestType::HTTP_PUT, "PUT"},
        {RequestType::HTTP_DELETE, "DELETE"},
        {RequestType::HTTP_PATCH, "PATCH"},
        {RequestType::HTTP_OPTIONS, "OPTIONS"},
        {RequestType::HTTP_HEAD, "HEAD"}
    };

public:
    virtual ~IEndpointMonitor() = default;

    IEndpointMonitor() = default;
    IEndpointMonitor(const Settings &s);

    // Forbidding copying and moving this base class
    IEndpointMonitor(const IEndpointMonitor &other) = delete;
    IEndpointMonitor &operator=(const IEndpointMonitor &other) = delete;

    IEndpointMonitor(IEndpointMonitor &&other) = delete;
    IEndpointMonitor &operator=(IEndpointMonitor &&other) = delete;

    /**
     * Runs the full health check loop, retrying up to max_retries with timeouts between attempts
     * @return Aggregated result containing status codes, latencies, and overall success
     */
    HealthCheckResult health_check();

    /**
     * Parses a string (case-insensitive) into a RequestType enum value
     * @param request_type String representation e.g. "GET", "post"
     * @return The matching RequestType, or nullopt if unrecognised
     */
    static std::optional<RequestType> parse_request_type(const std::string &request_type);

    /**
     * Converts a RequestType enum value back to its string representation
     * @param requestType The enum value to convert
     * @return The matching string, or nullopt if unmapped
     */
    static std::optional<std::string> request_type_to_string(const RequestType requestType);

protected:

    /**
     * Sends a single HTTP request to the given url and path
     * @param url  Base URL including scheme and host
     * @param path Request path appended to the base URL
     * @return Parsed Response with status code, body, and success flag
     */
    virtual Response perform_request(const std::string &url, const std::string &path) = 0;

    /**
     * Returns true if the response is successful and its status code matches the configured healthy code
     * @param response The response to validate
     */    
    virtual bool is_response_valid(const Response &response) const;

private:
    void increase_current_retry();
    void reset_current_retry();
    bool has_exceeded_max_retries() const;
    void apply_timeout() const;
    bool is_response_status_code_healthy(const int response_status_code) const;
};