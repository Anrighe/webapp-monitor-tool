#include "config.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

#include "../include/libraries/nlohmann/json.hpp"

using json = nlohmann::json;

#define DEFAULT_CHECK_INTERVAL_SECONDS 30
#define DEFAULT_HTTP_TIMEOUT_SECONDS 5
#define DEFAULT_FAILURE_THRESHOLD 3
#define DEFAULT_MAX_LOG_FILESIZE_BYTE 1048576
#define DEFAULT_LOGS_PATH "/logs"
#define DEFAULT_CONFIG_PATH "config.json"

std::filesystem::path Config::resolve_path_from_current_directory(
    const std::filesystem::path& path
) {
    if (path.is_absolute()) {
        return path;
    }

    return std::filesystem::current_path() / path;
}

void require_field(const json &object, const std::string &field_name) {
    if (!object.contains(field_name)) {
        throw std::runtime_error(std::format("Missing required config field: {}", field_name));
    }
}

Config::Smtp parse_smtp(const json& smtp_json) {
    require_field(smtp_json, "url");
    require_field(smtp_json, "username");
    require_field(smtp_json, "password");
    require_field(smtp_json, "from");

    Config::Smtp smtp;
    smtp.url = smtp_json.at("url").get<std::string>();
    smtp.username = smtp_json.at("username").get<std::string>();
    smtp.password = smtp_json.at("password").get<std::string>();
    smtp.from = smtp_json.at("from").get<std::string>();

    return smtp;
}

Config::App parse_app(const json &app_json) {
    require_field(app_json, "name");
    require_field(app_json, "url");
    require_field(app_json, "admins");

    Config::App app;
    app.name = app_json.at("name").get<std::string>();
    app.url = app_json.at("url").get<std::string>();
    app.admins = app_json.at("admins").get<std::vector<std::string>>();

    return app;
}

Config Config::loadConfig() {
    return Config::loadConfig(DEFAULT_CONFIG_PATH);
}

Config Config::loadConfig(const std::filesystem::path &path) {
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error(std::format("Could not open config file: {}", path.string()));
    }

    json root;

    try {
        file>>root;
    } catch (const json::parse_error& error) {
        throw std::runtime_error(std::format("Invalid JSON in config file '{}': {}", path.string(), std::string(error.what())));
    }

    Config config;

    config.check_interval_seconds_ = root.value("check_interval_seconds", DEFAULT_CHECK_INTERVAL_SECONDS);
    config.http_timeout_seconds_ = root.value("http_timeout_seconds", DEFAULT_HTTP_TIMEOUT_SECONDS);
    config.failure_threshold_ = root.value("failure_threshold", DEFAULT_FAILURE_THRESHOLD);
    config.max_logs_filesize_byte_ = root.value("max_logs_filesize_byte", DEFAULT_MAX_LOG_FILESIZE_BYTE);
    config.logs_path_ = resolve_path_from_current_directory(root.value("logs_path", DEFAULT_LOGS_PATH));

    require_field(root, "smtp");
    require_field(root, "apps");

    config.smtp_ = parse_smtp(root.at("smtp"));

    json &appsJson = root.at("apps");

    if (!appsJson.is_array()) {
        throw std::runtime_error("Config field 'apps' must be an array");
    }

    for (const json& app_json : appsJson) {
        config.apps_.push_back(parse_app(app_json));
    }

    validate(config);

    return config;
}

int Config::get_check_interval_seconds() const {
    return check_interval_seconds_;
}

int Config::get_http_timeout_seconds() const {
    return http_timeout_seconds_;
}

int Config::get_failure_threshold() const {
    return failure_threshold_;
}

int Config::get_max_logs_filesize_byte() const {
    return max_logs_filesize_byte_;
}

std::filesystem::path Config::get_logs_path() const {
    return logs_path_;
}

const Config::Smtp& Config::get_smtp() const {
    return smtp_;
}

const std::vector<Config::App>& Config::get_apps() const {
    return apps_;
}

void Config::validate(const Config& config) {
    if (config.check_interval_seconds_ <= 0) {
        throw std::runtime_error("check_interval_seconds must be greater than 0");
    }

    if (config.http_timeout_seconds_ <= 0) {
        throw std::runtime_error("http_timeout_seconds must be greater than 0");
    }

    if (config.failure_threshold_<= 0) {
        throw std::runtime_error("failure_threshold must be greater than 0");
    }

    if (config.smtp_.url.empty()) {
        throw std::runtime_error("smtp.url cannot be empty");
    }

    if (config.smtp_.username.empty()) {
        throw std::runtime_error("smtp.username cannot be empty");
    }

    if (config.smtp_.password.empty()) {
        throw std::runtime_error("smtp.password cannot be empty");
    }

    if (config.smtp_.from.empty()) {
        throw std::runtime_error("smtp.from cannot be empty");
    }

    if (config.apps_.empty()) {
        throw std::runtime_error("Config must contain at least one app");
    }

    for (const App& app : config.apps_) {
        validate_app(app);
    }
}

void Config::validate_app(const Config::App &app) {
    if (app.name.empty()) {
        throw std::runtime_error("Each app must have a non-empty name");
    }

    if (app.url.empty()) {
        throw std::runtime_error(std::format("App '{}' has an empty url", app.name));
    }

    if (app.admins.empty()) {
        throw std::runtime_error(std::format("App '{}' must have at least one admin email", app.name));
    }

    for (const std::string& admin : app.admins) {
        if (admin.empty()) {
            throw std::runtime_error(std::format("App '{}' has an empty admin email", app.name));
        }
    }
}