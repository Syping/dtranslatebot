/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2023-2024 Syping
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
#include "translator_libretranslate.h"
using namespace bot::translator;
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
    std::vector<language> languages;

    try {
        dpp::https_client http_request(m_hostname, m_port, m_url + "languages", "GET", {}, {}, !m_tls);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_array()) {
                for (const auto &json_language : response) {
                    if (json_language.is_object()) {
                        language language;

                        auto json_lang_code = json_language.find("code");
                        if (json_lang_code != json_language.end())
                            language.code = json_lang_code.value();

                        auto json_lang_name = json_language.find("name");
                        if (json_lang_name != json_language.end())
                            language.name = json_lang_name.value();

                        if (!language.code.empty() && !language.name.empty())
                            languages.push_back(std::move(language));
                    }
                }
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[dtranslatebot] Exception: " << exception.what() << std::endl;
    }

    return languages;
}

const std::string libretranslate::translate(const std::string &text, const std::string &source, const std::string &target)
{
    const dpp::http_headers http_headers = {
        {"Content-Type"s, "application/json"s}
    };

    dpp::json json_body = {
        {"q"s, text},
        {"source"s, source},
        {"target"s, target},
        {"format"s, "text"s}
    };

    if (!m_apiKey.empty())
        json_body["apiKey"] = m_apiKey;

    try {
        dpp::https_client http_request(m_hostname, m_port, m_url + "translate", "POST", json_body.dump(), http_headers, !m_tls);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_object()) {
                auto tr_text = response.find("translatedText");
                if (tr_text != response.end())
                    return tr_text.value();
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[dtranslatebot] Exception: " << exception.what() << std::endl;
    }

    return text;
}
