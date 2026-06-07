#pragma once

#include <memory>
#include <string>
#include <vector>


#include "i_endpoint_monitor.hpp"

#include "../include/libraries/spdlog/logger.h"
#include "../include/libraries/httplib/httplib.h"

class HttpEndpointMonitor : public IEndpointMonitor
{
public: 

    /**
     * @param logger Shared logger instance
     * @param settings Monitor configuration (url, timeouts, retries, etc.)
     */
    HttpEndpointMonitor(
        const std::shared_ptr<spdlog::logger> &logger,
        const Settings &settings
    );

    virtual ~HttpEndpointMonitor() override = default;

protected:

    /**
     * Sends an HTTP request using the configured method and parses the response
     * @param url  Base URL including scheme and host
     * @param path Request path appended to the base URL
     * @return Parsed Response with status code, body, and success flag
     */
    virtual Response perform_request(const std::string &url, const std::string &path) override;

private:

    /**
     * Parses an httplib result into a Response struct
     * @param result The raw httplib result to parse
     * @return Response with status code, body, and success flag
     */
    virtual Response parse_response(const httplib::Result &result);

    /**
     * Returns a callable that performs the configured request type on the given client
     * @param client The httplib client to bind the request to
     * @return A function taking a path and returning an httplib::Result
     */    
    std::function<httplib::Result(const std::string&)> get_request_based_on_type(httplib::Client &client);

private:
    std::shared_ptr<spdlog::logger> logger;
};