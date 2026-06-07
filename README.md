# WebApp Monitor Tool

A lightweight health-check tool that monitors HTTP endpoints and logs their availability, response status, and latency

## Features

- Periodic health checks for HTTP endpoints
- Configurable settings
- Retry & timeout support
- Latency tracking
- Logging
- Extensible monitor interface (`IEndpointMonitor`)

## Requirements

- C++23 compatible compiler
- Make (Makefile provided)
- Dependencies:
  - spdlog
  - nlohmann/json
  - cpp-httplib

## Build

```bash
make all
./webapp-monitor-tool
```

## Configuration

Example config.json:

```json
{
  "max_log_filesize": 1048576,
  "logs_path": "logs/webapp-monitor-tool.log",
  "smtp": {
    "url": "smtp.example.com",
    "username": "user",
    "password": "pass",
    "from": "from@example.com"
  },
  "apps": [
    {
      "name": "orders-api",
      "url": "http://localhost:8080/q/health/ready",
      "admins": [
        "admin@example.com"
      ],
      "request_type": "GET",
      "healthy_response_status_code": 200,
      "timeout_seconds": 1,
      "max_retries": 3
    }
  ]
}
```

## How It Works

1. Load configuration from JSON
2. Build endpoint monitors
3. Each monitor performs HTTP health checks
4. Results are logged (status, retries, latency)
5. Runner aggregates and reports results
6. Sends an email to the specified administrators if any health check fails after retries

## Extensibility

To support other protocols (e.g., HTTPS), implement:

```C++
class MyCustomMonitor : public IEndpointMonitor {
    Response perform_request(const std::string& path) override;
    HttpEndpointMonitor::parse_response(const httplib::Result &result) override;
};
```
