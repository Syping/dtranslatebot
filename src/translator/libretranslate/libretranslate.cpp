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

#include <dpp/json.h>
#include <dpp/httpsclient.h>
#include "../../core/http_request.h"
#include "libretranslate.h"
using namespace bot::translator;
using namespace std::chrono_literals;
using namespace std::string_literals;

libretranslate::libretranslate(const std::string &hostname, uint16_t port, const std::string &url, bool tls, const std::string apiKey) :
    m_hostname(hostname), m_port(port), m_url(url), m_tls(tls), m_apiKey(apiKey)
{
}

libretranslate::~libretranslate()
{
}

const std::vector<language> libretranslate::get_languages()
{
    if (!m_languages.languages.empty()) {
        auto current_time = std::chrono::system_clock::now();
        auto threshold_time = m_languages.query_time + 24h;
        if (current_time <= threshold_time)
            return m_languages.languages;
    }

    try {
        http_request request;
        http_response response = request.get(http_request::legacy_url(m_hostname, m_port, m_url + "languages"s, m_tls));
        if (response.status == 200) {
            const dpp::json json_response = dpp::json::parse(response.content);
            if (json_response.is_array()) {
                m_languages.languages.clear();
                for (auto json_language = json_response.begin(); json_language != json_response.end(); json_language++) {
                    if (json_language->is_object()) {
                        language language;

                        auto json_lang_code = json_language->find("code");
                        if (json_lang_code != json_language->end())
                            language.code = *json_lang_code;

                        auto json_lang_name = json_language->find("name");
                        if (json_lang_name != json_language->end())
                            language.name = *json_lang_name;

                        if (!language.code.empty() && !language.name.empty())
                            m_languages.languages.push_back(std::move(language));
                    }
                }
                m_languages.query_time = std::chrono::system_clock::now();
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }

    return m_languages.languages;
}

const std::string libretranslate::translate(const std::string &text, const std::string &source, const std::string &target)
{
    const dpp::http_headers http_headers = {
        {"Content-Type"s, "application/json"s}
    };

    dpp::json json_body = {
        {"q"s, text},
        {"source"s, source.empty() ? "auto"s : source},
        {"target"s, target},
        {"format"s, "text"s}
    };

    if (!m_apiKey.empty())
        json_body["apiKey"] = m_apiKey;

    try {
        http_request request;
        http_response response = request.post(http_request::legacy_url(m_hostname, m_port, m_url + "translate"s, m_tls), json_body.dump(), "application/json");
        if (response.status == 200) {
            const dpp::json json_response = dpp::json::parse(response.content);
            if (json_response.is_object()) {
                auto tr_text = json_response.find("translatedText");
                if (tr_text != json_response.end())
                    return *tr_text;
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }

    return text;
}
