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
        const std::shared_ptr<spdlog::logger> &logger,
        const Settings &settings
    ): logger(logger), settings(settings) {};

    virtual ~HttpEndpointMonitor() override = default;

protected:
    virtual Response perform_request(const std::string &path) override;
    virtual bool is_response_valid(const Response &response) const override;

private: 
    virtual Response parse_response(const httplib::Result &result);

private:
    std::shared_ptr<spdlog::logger> logger;
    Settings settings;
};