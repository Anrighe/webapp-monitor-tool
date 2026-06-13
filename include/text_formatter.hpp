#pragma once

#include <string>
#include <format>

class TextFormatter {
private:
    static inline const std::string RESET = "\033[0m";
    static inline const std::string RED   = "\033[31m";
    static inline const std::string GREEN = "\033[32m";
    static inline const std::string ORANGE = "\033[38;5;208m";
    static inline const std::string LIGHT_BLUE = "\033[94m";

    static inline const std::string DEFAULT_SEPARATOR = "--------------------------------------------------------------------------------";

public:
    static std::string format_red(const std::string &text) {
        return std::format("{}{}{}", RED, text, RESET);
    }

    static std::string format_green(const std::string &text) {
        return std::format("{}{}{}", GREEN, text, RESET);
    }

    static std::string format_orange(const std::string &text) {
        return std::format("{}{}{}", ORANGE, text, RESET);
    }

    static std::string format_light_blue(const std::string &text) {
        return std::format("{}{}{}", LIGHT_BLUE, text, RESET);
    }

    static std::string make_separator(const std::string &text = "", std::size_t width = 80) {
        if (text == "") {
            return DEFAULT_SEPARATOR;
        }
        std::string message = std::format(" {} ", text);

        if (message.length() >= width) {
            return message;
        }

        int total_dashes = width - message.length();
        int left_dashes = total_dashes / 2 - 1;
        int right_dashes = total_dashes - left_dashes - 1;

        return std::format("{}{}{}", std::string(left_dashes, '-'), message, std::string(right_dashes, '-'));
    }
};