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
#include "translate_libretranslate.h"

bot::translate::libretranslate::libretranslate(const std::string &hostname, uint16_t port, const std::string &url, bool tls, const std::string apiKey) :
    m_hostname(hostname), m_port(port), m_url(url), m_tls(tls), m_apiKey(apiKey)
{
}

const std::vector<bot::translate::language> bot::translate::libretranslate::get_languages()
{
    std::vector<bot::translate::language> languages;

    try {
        dpp::https_client http_request(m_hostname, m_port, m_url + "languages", "GET", {}, {}, !m_tls);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_array()) {
                for (auto json_language = response.begin(); json_language != response.end(); json_language++) {
                    if (json_language->is_object()) {
                        bot::translate::language language;

                        auto json_lang_code = json_language.value().find("code");
                        if (json_lang_code != json_language.value().end())
                            language.code = json_lang_code.value();

                        auto json_lang_name = json_language.value().find("name");
                        if (json_lang_name != json_language.value().end())
                            language.name = json_lang_name.value();

                        if (!language.code.empty() && !language.name.empty())
                            languages.push_back(language);
                    }
                }
            }
        }
    }
    catch (const dpp::json::exception &exception) {
        std::cerr << "Exception thrown while parsing supported languages JSON: " << exception.what() << std::endl;
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while getting supported languages: " << exception.what() << std::endl;
    }

    return languages;
}

const std::string bot::translate::libretranslate::translate(const std::string &text, const std::string &source, const std::string &target)
{
    const dpp::http_headers http_headers = {
        {"Content-Type", "application/json"}
    };

    dpp::json json_body = {
        {"q", text},
        {"source", source},
        {"target", target},
        {"format", "text"}
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
    catch (const dpp::json::exception &exception) {
        std::cerr << "Exception thrown while parsing translated JSON: " << exception.what() << std::endl;
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while translating: " << exception.what() << std::endl;
    }

    return text;
}
