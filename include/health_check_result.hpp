#pragma once

#include <string>
#include <vector>

struct HealthCheckResult {
    std::string url;
    std::string path;
    bool success;
    std::vector<int> status_codes;
    int retries = 0;
    std::vector<double> latencies_ms;
    std::vector<std::string> status_messages;
};