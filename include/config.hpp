#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

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
        std::string path;
        std::vector<std::string> admins;
        std::string request_type;
        int healthy_response_status_code;
        int timeout_seconds;
        int max_retries;
    };

    static Config loadConfig();
    static Config loadConfig(const std::filesystem::path &path);


    int get_max_logs_filesize_byte() const;
    std::filesystem::path get_logs_path() const;

    const Smtp &get_smtp() const;
    const std::vector<App> &get_apps() const;

private:
    std::string application_name_ = APPLICATION_NAME;

    int max_logs_filesize_byte_;
    std::filesystem::path logs_path_;

    Smtp smtp_;
    std::vector<App> apps_;

    static void validate(const Config &config);
    static void validate_app(const App &app);
    static bool is_email_valid(const std::string &email);
    static bool is_valid_request_type(const std::string &request_type);
    static std::filesystem::path resolve_path_from_current_directory(const std::filesystem::path& path);
};