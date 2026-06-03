#pragma once

#include <memory>
#include <string>
#include <vector>


#include "i_endpoint_monitor.hpp"

#include "../include/libraries/spdlog/logger.h"
#include "../include/libraries/httplib/httplib.h"
#include "../include/libraries/nlohmann/json.hpp"

using json = nlohmann::json;

class HttpEndpointMonitor : public IEndpointMonitor
{
public: 

    HttpEndpointMonitor(
        const std::shared_ptr<spdlog::logger>& logger,
        const Settings& settings
    );

    virtual ~HttpEndpointMonitor() override = default;

protected:
    virtual Response perform_request(const std::string &path) override;
    virtual Response parse_response(const std::string &response) override;
    virtual bool is_response_valid(const Response &response) const override;
};