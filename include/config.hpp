#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

#include "../include/text_formatter.hpp"

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
        bool active;
        std::vector<std::string> admins;
        std::string request_type;
        int healthy_response_status_code;
        int timeout_seconds;
        int max_retries;
    };

    /**
     * Loads config from the default path (config.json in the current directory)
     * @return Parsed and validated Config instance
     */
    static Config loadConfig();

    /**
     * Loads config from the given path
     * @param path Path to the JSON config file
     * @return Parsed and validated Config instance
     */
    static Config loadConfig(const std::filesystem::path &path);

    /** @return Maximum log file size in bytes */
    int get_max_logs_filesize_byte() const;

    /** @return Path to the log file */
    std::filesystem::path get_logs_path() const;

    /** @return SMTP configuration */
    const Smtp &get_smtp() const;

    /** @return List of configured apps to monitor */
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
    static std::filesystem::path resolve_path_from_current_directory(const std::filesystem::path &path);
};