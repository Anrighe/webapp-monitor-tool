#pragma once

#include <string>
#include <vector>
#include <filesystem>

#define APPLICATION_NAME "Webapp monitor tool"

class Config {
public:
    struct Smtp {
        std::string url;
        std::string username;
        std::string password;
        std::string from;
    };

    struct App {
        std::string name;
        std::string url;
        std::vector<std::string> admins;
    };

    static Config loadConfig();
    static Config loadConfig(const std::filesystem::path &path);

    int get_check_interval_seconds() const;
    int get_http_timeout_seconds() const;
    int get_failure_threshold() const;

    int get_max_logs_filesize_byte() const;
    std::filesystem::path get_logs_path() const;

    const Smtp &get_smtp() const;
    const std::vector<App> &get_apps() const;

private:
    std::string application_name = APPLICATION_NAME;

    int check_interval_seconds_;
    int http_timeout_seconds_;
    int failure_threshold_;

    int max_logs_filesize_byte_;
    std::filesystem::path logs_path_;

    Smtp smtp_;
    std::vector<App> apps_;

    static void validate(const Config &config);
    static void validate_app(const App &app);
    static std::filesystem::path resolve_path_from_current_directory(const std::filesystem::path& path);
};