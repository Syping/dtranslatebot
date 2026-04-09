/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2023-2026 Syping
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

#ifndef TRANSLATOR_LIBRETRANSLATE_H
#define TRANSLATOR_LIBRETRANSLATE_H

#include <cstdint>
#include "../../core/http_request.h"
#include "../../core/log.h"
#include "../../core/translator.h"

namespace bot {
    namespace translator {
        class libretranslate : public translator {

        public:
            explicit libretranslate(const std::string &hostname, uint16_t port, const std::string &url, bool tls, const std::string &api_key, const bot::log::log_message_callback &log_callback);
            ~libretranslate() override;
            const std::vector<language> get_languages() override;
            const std::string translate(const std::string &text, const std::string &source, const std::string &target) override;

        private:
            std::string m_apiKey;
            std::string m_hostname;
            bot::http::http_request m_http;
            supported_languages m_languages;
            uint16_t m_port;
            std::string m_url;
            bool m_tls;
            bot::log::log_message_callback m_logCallback;
        };
    }
}

#endif // TRANSLATOR_LIBRETRANSLATE_H
