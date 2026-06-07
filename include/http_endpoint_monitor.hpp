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

    HttpEndpointMonitor(
        const std::shared_ptr<spdlog::logger>& logger,
        const Settings& settings
    );

    virtual ~HttpEndpointMonitor() override = default;

protected:
    virtual Response perform_request(const std::string &url, const std::string &path) override;

private: 
    virtual Response parse_response(const httplib::Result &result);

private:
    std::shared_ptr<spdlog::logger> logger;
};