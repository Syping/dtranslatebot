/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2026 Syping
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* This software is provided as-is, no warranties are given to you, we are not
* responsible for anything with use of the software, you are self responsible.
*****************************************************************************/

#include "http_request.h"

http_request::http_request() {
    instance = curl_easy_init();
    if (!instance)
        throw std::bad_alloc();
}

http_response http_request::get(const std::string &url, const dpp::http_headers &headers) {
    http_response response;
    curl_easy_setopt(instance, CURLOPT_URL, url.c_str());
    curl_slist *header_slist = nullptr;
    if (!headers.empty()) {
        for (const auto &header : headers) {
            header_slist = curl_slist_append(header_slist, std::string(header.first + ": " + header.second).c_str());
            if (!header_slist)
                throw std::bad_alloc();
        }
    }
    if (!header_slist)
        curl_easy_setopt(instance, CURLOPT_HTTPHEADER, header_slist);
    curl_easy_setopt(instance, CURLOPT_WRITEDATA, &response.content);
    curl_easy_setopt(instance, CURLOPT_WRITEFUNCTION, &writer);
    CURLcode result = curl_easy_perform(instance);
    if (result == CURLE_OK)
        curl_easy_getinfo(instance, CURLINFO_RESPONSE_CODE, &response.status);
    curl_easy_reset(instance);
    curl_slist_free_all(header_slist);
    return response;
}

http_response http_request::post(const std::string &url, const std::string &content, const std::string &content_type, const dpp::http_headers &headers) {
    http_response response;
    curl_easy_setopt(instance, CURLOPT_URL, url.c_str());
    curl_slist *header_slist = nullptr;
    if (!content_type.empty()) {
        header_slist = curl_slist_append(header_slist, std::string("Content-Type: " + content_type).c_str());
        if (!header_slist)
            throw std::bad_alloc();
    }
    if (!headers.empty()) {
        for (const auto &header : headers) {
            header_slist = curl_slist_append(header_slist, std::string(header.first + ": " + header.second).c_str());
            if (!header_slist)
                throw std::bad_alloc();
        }
    }
    if (!header_slist)
        curl_easy_setopt(instance, CURLOPT_HTTPHEADER, header_slist);
    curl_easy_setopt(instance, CURLOPT_POSTFIELDS, content.data());
    curl_easy_setopt(instance, CURLOPT_POSTFIELDSIZE, content.size());
    curl_easy_setopt(instance, CURLOPT_WRITEDATA, &response.content);
    curl_easy_setopt(instance, CURLOPT_WRITEFUNCTION, &writer);
    CURLcode result = curl_easy_perform(instance);
    if (result == CURLE_OK)
        curl_easy_getinfo(instance, CURLINFO_RESPONSE_CODE, &response.status);
    curl_easy_reset(instance);
    curl_slist_free_all(header_slist);
    return response;
}

std::string http_request::legacy_url(const std::string &hostname, uint16_t port, const std::string &url, bool tls) {
    return (tls ? "https://" : "http://") + hostname + ":" + std::to_string(port) + (url.empty() ? "/" : (url.front() != '/' ? "/" + url : url));
}

http_request::~http_request() {
    curl_easy_cleanup(instance);
}

size_t http_request::writer(char *source, size_t size, size_t nmemb, std::string *target) {
    if (target == nullptr)
        return 0;
    size_t write_size = size * nmemb;
    target->append(source, write_size);
    return write_size;
}
