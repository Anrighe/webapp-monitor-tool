#pragma once

#include <curl/curl.h>

#include <cstring>
#include <string>
#include <vector>

#include "../include/libraries/spdlog/logger.h"

class EmailSender {
private:

    struct UploadStatus {
        std::size_t index = 0;
        std::string payload;
    };

    std::string smtp_url;
    std::string username;
    std::string password;
    std::string email_sender;
    std::shared_ptr<spdlog::logger> logger;

    /**
     * Callback used by libcurl to read the email payload.
     * Libcurl calls this function repeatedly while uploading the email body.
     * Each call copies the next part of the payload into the buffer provided by libcurl and advances the current read index
     * @param payload_data_buffer_ptr Buffer where the payload data must be copied
     * @param buffer_elements_size Size of each buffer element
     * @param available_buffers Number of buffer elements available
     * @param upload_status_object_ptr Pointer to the UploadStatus object containing the payload
     * @return Number of bytes copied into ptr.
     *         Returning 0 tells libcurl that there is no more data to upload
     */
    static std::size_t read_callback(
        char* payload_data_buffer_ptr, 
        std::size_t buffer_elements_size, 
        std::size_t available_buffers, 
        void* upload_status_object_ptr
    );

public:

    EmailSender(std::string smtp_url, std::string username, std::string password, std::string email_sender, std::shared_ptr<spdlog::logger> logger)
        :   smtp_url(std::move(smtp_url)),
            username(std::move(username)),
            password(std::move(password)),
            email_sender(std::move(email_sender)),
            logger(std::move(logger)) {}

    /**
     * Sends a plain-text email using SMTP through libcurl.
     * This function builds the email headers and body, configures the SMTP server,
     *  authentication credentials, sender, recipients, and TLS settings, then asks libcurl to upload the email payload
     * @param recipients List of recipient email addresses
     * @param subject Email subject
     * @param body Plain-text email body
     * @return true if libcurl reports that the email was sent successfully, false otherwise
     */
    bool send_email(
        const std::vector<std::string>& recipients,
        const std::string& subject,
        const std::string& body
    );
};