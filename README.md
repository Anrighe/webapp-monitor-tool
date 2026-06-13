# WebApp Monitor Tool

A lightweight health-check tool that monitors HTTP endpoints and logs their availability, response status, and latency

- [WebApp Monitor Tool](#webapp-monitor-tool)
  - [Features](#features)
  - [Requirements](#requirements)
    - [Install libcurl on Debian/Ubuntu](#install-libcurl-on-debianubuntu)
  - [Build](#build)
  - [Configuration](#configuration)
  - [How It Works](#how-it-works)
  - [Extensibility](#extensibility)
  - [Example Output](#example-output)

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
  - libcurl

### Install libcurl on Debian/Ubuntu

```bash
sudo apt install libcurl4-openssl-dev
```

The other dependencies are header-only and included in the `include/` directory, so no additional installation is required for them.

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
      "url": "http://localhost:8080",
      "path": "/q/health/ready",
      "active": true,
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

## Example Output

```console
user:~/C++/webapp-monitor-tool$ ./webapp-monitor-tool
-------------------------------- Parsing apps --------------------------------
Loaded App 'orders-api':
       url: 'http://localhost:8080'
       path: '/q/health/ready'
       active: 'true'
       admins: 'admin@example.com, test@example.com'
       request_type: 'GET'
       healthy_response_status_code: '200'
       timeout_seconds: '5'
       max_retries: '3'

[ OK ] Config file parsing 
[ OK ] Config validation
[2026-06-13 22:27:04] [webapp-monitor-tool] [info] Config loaded successfully
[2026-06-13 22:27:04] [webapp-monitor-tool] [info] Runner created successfully
[2026-06-13 22:27:04] [webapp-monitor-tool] [info] [ OK ] 'http://localhost:8080/q/health/ready' | status codes: '200' | retries: '1' | latencies: '0.591017' ms
```
