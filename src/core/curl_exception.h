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

#ifndef CURL_EXCEPTION_H
#define CURL_EXCEPTION_H

#include <curl/curl.h>
#include <exception>
#include <string>

namespace bot {
    namespace exception {
        class curl_exception : public std::exception {
        public:
            explicit curl_exception(const std::string &message, CURLcode error_code);
            virtual ~curl_exception() noexcept;
            const char* what() const noexcept override;

        private:
            const std::string m_message;
            const CURLcode m_error_code;
        };
    }
};

#endif // CURL_EXCEPTION_H
