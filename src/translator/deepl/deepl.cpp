/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2024-2026 Syping
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
#include "deepl.h"
using namespace bot::translator;
using namespace std::chrono_literals;

deepl::deepl(const std::string &hostname, const std::string apiKey) :
    m_hostname(hostname), m_apiKey(apiKey)
{
}

deepl::~deepl()
{
}

const std::vector<language> deepl::get_languages()
{
    if (!m_languages.languages.empty()) {
        auto current_time = std::chrono::system_clock::now();
        auto threshold_time = m_languages.query_time + 24h;
        if (current_time <= threshold_time)
            return m_languages.languages;
    }

    try {
        http_request request;
        http_response response = request.get(http_request::legacy_url(m_hostname, 443, "/v2/languages?type=target", true), { {"Authorization", "DeepL-Auth-Key " + m_apiKey} });
        if (response.status == 200) {
            const dpp::json json_response = dpp::json::parse(response.content);
            if (json_response.is_array()) {
                m_languages.languages.clear();
                for (auto json_language = json_response.begin(); json_language != json_response.end(); json_language++) {
                    if (json_language->is_object()) {
                        language language;

                        auto json_lang_code = json_language->find("language");
                        if (json_lang_code != json_language->end())
                            language.code = *json_lang_code;

                        if (language.code.size() > 2)
                            std::transform(language.code.begin(), language.code.begin() + 2, language.code.begin(), ::tolower);
                        else
                            std::transform(language.code.begin(), language.code.end(), language.code.begin(), ::tolower);

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

const std::string deepl::translate(const std::string &text, const std::string &source, const std::string &target)
{
    const dpp::http_headers http_headers = {
        {"Authorization", "DeepL-Auth-Key " + m_apiKey},
        {"Content-Type", "application/json"}
    };

    dpp::json json_body = {
        {"text", { text } },
        {"target_lang", target}
    };
    if (!source.empty())
        json_body["source_lang"] = source;

    try {
        http_request request;
        http_response response = request.post(http_request::legacy_url(m_hostname, 443, "/v2/translate", true), json_body.dump(), "application/json", { {"Authorization", "DeepL-Auth-Key " + m_apiKey} });
        if (response.status == 200) {
            const dpp::json json_response = dpp::json::parse(response.content);
            if (json_response.is_object()) {
                auto translations = json_response.find("translations");
                if (translations != json_response.end() && translations->is_array()) {
                    for (auto translation = translations->begin(); translation != translations->end(); translation++) {
                        auto tr_text = translation->find("text");
                        if (tr_text != translation->end())
                            return *tr_text;
                    }
                }
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }

    return text;
}
