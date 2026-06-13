#include "../include/email_sender.hpp"

std::size_t EmailSender::read_callback(
    char* payload_data_buffer_ptr,
    std::size_t buffer_elements_size,
    std::size_t available_buffers,
    void* upload_status_object_ptr
) {
    UploadStatus* upload_status = static_cast<UploadStatus*>(upload_status_object_ptr);

    const std::size_t payload_buffer_size = buffer_elements_size * available_buffers;
    const std::size_t remaining_payload_size = upload_status->payload.size() - upload_status->index;

    if (remaining_payload_size == 0) {
        return 0;
    }

    const std::size_t bytes_to_copy = remaining_payload_size < payload_buffer_size 
            ? remaining_payload_size
            : payload_buffer_size;

    std::memcpy(
        payload_data_buffer_ptr,
        upload_status->payload.c_str() + upload_status->index,
        bytes_to_copy
    );

    upload_status->index += bytes_to_copy;

    return bytes_to_copy;
}

bool EmailSender::send_email(
    const std::vector<std::string>& recipients,
    const std::string& subject,
    const std::string& body
) {
    if (recipients.empty()) {
        return false;
    }

    CURL* curl = curl_easy_init();

    if (!curl) {
        return false;
    }

    curl_slist* recipient_list = nullptr;

    for (const auto& recipient : recipients) {
        const std::string formatted_recipient = "<" + recipient + ">";

        recipient_list = curl_slist_append(
            recipient_list,
            formatted_recipient.c_str()
        );
    }

    std::string payload;

    payload += "From: <" + email_sender + ">\r\n";

    for (const auto& recipient : recipients) {
        payload += "To: <" + recipient + ">\r\n";
    }

    payload += "Subject: " + subject + "\r\n";
    payload += "Content-Type: text/plain; charset=UTF-8\r\n";
    payload += "\r\n";
    payload += body + "\r\n";

    UploadStatus upload_status;
    upload_status.payload = payload;

    const std::string formatted_sender = "<" + email_sender + ">";

    char error_buffer[CURL_ERROR_SIZE] = {0};

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    curl_easy_setopt(curl, CURLOPT_URL, smtp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, formatted_sender.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipient_list);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_status);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    const CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        logger->error(
            "Failed to send email. CURLcode: {} | Error: {}",
            static_cast<int>(result),
            error_buffer[0] ? error_buffer : curl_easy_strerror(result)
        );
    } else {
        logger->info("Email sent successfully");
    }

    curl_slist_free_all(recipient_list);
    curl_easy_cleanup(curl);

    return result == CURLE_OK;
}