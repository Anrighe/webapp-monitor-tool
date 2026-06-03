#pragma once

#include <string>

class IEndpointMonitor
{
public:
    struct Response
    {
        bool success;
        int status_code;
        std::string body;
        std::string error_message;
        int attempts_used;
    };

    struct Settings
    {
        std::string host;
        int timeout_seconds;
        int max_retries;
        int healthy_response_status_code;
    };

protected:
    int current_retry = 0;
    Settings settings;

public:
    virtual ~IEndpointMonitor() = default;

    IEndpointMonitor() = default;

    // Forbidding copying and moving this base class
    IEndpointMonitor(const IEndpointMonitor &other) = delete;
    IEndpointMonitor& operator=(const IEndpointMonitor& other) = delete;

    IEndpointMonitor(IEndpointMonitor &&other) = delete;
    IEndpointMonitor& operator=(IEndpointMonitor &&other) = delete;

    bool health_check(const std::string &path);

protected:
    virtual Response perform_request(const std::string &path) = 0;
    virtual Response parse_response(const std::string &response);
    virtual bool is_response_valid(const Response &response) const;

private:
    void increase_current_retry();
    void reset_current_retry();
    bool has_exceeded_max_retries() const;
    void apply_timeout() const;
    bool is_response_status_code_healthy(const int response_status_code) const;
};