/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2024 Syping
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
#include "deepl.h"
using namespace bot::translator;
using namespace std::chrono_literals;
using namespace std::string_literals;

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
        dpp::https_client http_request(&m_cluster, m_hostname, 443, "/v2/languages?type=target", "GET", {}, { {"Authorization"s, "DeepL-Auth-Key " + m_apiKey} }, false);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_array()) {
                m_languages.languages.clear();
                for (auto json_language = response.begin(); json_language != response.end(); json_language++) {
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
        {"Authorization"s, "DeepL-Auth-Key " + m_apiKey},
        {"Content-Type"s, "application/json"s}
    };

    dpp::json json_body = {
        {"text"s, { text } },
        {"source_lang"s, source},
        {"target_lang"s, target},
    };

    try {
        dpp::https_client http_request(&m_cluster, m_hostname, 443, "/v2/translate", "POST", json_body.dump(), http_headers, false);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_object()) {
                auto translations = response.find("translations");
                if (translations != response.end() && translations->is_array()) {
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
