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

#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H
#include <curl/curl.h>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace bot {
    namespace http {
        typedef std::pair<std::string, std::string> http_header;

        class http_headers {
        public:
            http_headers();
            http_headers(const std::string &field, const std::string &value);
            http_headers(const http_header &header);
            http_headers(const std::initializer_list<http_header> &headers);
            http_headers(const std::vector<http_header> &headers);
            ~http_headers();
            void add(const std::string &field, const std::string &value);
            void add(const http_header &header);
            void add(const std::initializer_list<http_header> &headers);
            void add(const std::vector<http_header> &headers);
            void remove(const std::string &field);
            void remove(const std::vector<std::string> &fields);
            const curl_slist* data() const;

        private:
            curl_slist *instance;
        };
    }
}

#endif // HTTP_HEADERS_H
