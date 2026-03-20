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

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include <curl/curl.h>
#include <cstdint>
#include <dpp/httpsclient.h>
#include "http_response.h"

class http_request
{
public:
    http_request();
    ~http_request();
    http_response get(const std::string &url, const dpp::http_headers &headers = {});
    http_response post(const std::string &url, const std::string &content, const std::string &content_type, const dpp::http_headers &headers = {});
    static std::string legacy_url(const std::string &hostname, uint16_t port, const std::string &url, bool tls);

private:
    static size_t writer(char *source, size_t size, size_t nmemb, std::string *target);
    CURL *instance;
};

#endif // HTTP_REQUEST_H
