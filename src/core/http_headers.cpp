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

#include "http_headers.h"
using namespace bot::http;

http_headers::http_headers() {
    instance = nullptr;
}

http_headers::http_headers(const http_headers &headers) {
    instance = copy_from(headers.data());
}

http_headers::http_headers(const std::string &field, const std::string &value) {
    instance = nullptr;
    add(field, value);
}

http_headers::http_headers(const http_header &header) {
    instance = nullptr;
    add(header);
}

http_headers::http_headers(const std::initializer_list<http_header> &headers) {
    instance = nullptr;
    try {
        add(headers);
    }
    catch (const std::bad_alloc &exception) {
        curl_slist_free_all(instance);
        throw;
    }
}

http_headers::http_headers(const std::vector<http_header> &headers) {
    instance = nullptr;
    try {
        add(headers);
    }
    catch (const std::bad_alloc &exception) {
        curl_slist_free_all(instance);
        throw;
    }
}

http_headers::~http_headers() {
    curl_slist_free_all(instance);
}

http_headers& http_headers::operator=(const curl_slist *other) {
    if (this->data() == other)
        return *this;
    if (curl_slist *headers = copy_from(other)) {
        curl_slist_free_all(instance);
        instance = headers;
    }
    return *this;
}

http_headers& http_headers::operator=(const http_headers &other) {
    if (this == &other)
        return *this;
    if (curl_slist *headers = copy_from(other.data())) {
        curl_slist_free_all(instance);
        instance = headers;
    }
    return *this;
}

void http_headers::add(const std::string &field, const std::string &value) {
    const std::string header = field + ": " + value;
    curl_slist *headers = curl_slist_append(instance, header.c_str());
    if (!headers)
        throw std::bad_alloc();
    instance = headers;
}

void http_headers::add(const http_header &header) {
    add(header.first, header.second);
}

void http_headers::add(const std::initializer_list<http_header> &headers) {
    for (const auto &header : headers)
        add(header);
}

void http_headers::add(const std::vector<http_header> &headers) {
    for (const auto &header : headers)
        add(header);
}

void http_headers::remove(const std::string &field) {
    const std::string header = field + ":";
    curl_slist *headers = curl_slist_append(instance, header.c_str());
    if (!headers)
        throw std::bad_alloc();
    instance = headers;
}

void http_headers::remove(const std::vector<std::string> &fields) {
    for (const auto &field : fields)
        remove(field);
}

const curl_slist* http_headers::data() const {
    return instance;
}

curl_slist* http_headers::copy_from(const curl_slist *headers) {
    curl_slist *instance = nullptr;
    for (const curl_slist *i = headers; i; i = i->next) {
        curl_slist *headers = curl_slist_append(instance, i->data);
        if (!headers) {
            curl_slist_free_all(instance);
            throw std::bad_alloc();
        }
        instance = headers;
    }
    return instance;
}
