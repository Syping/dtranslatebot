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
#include <dpp/utility.h>
#include "mozhi.h"
using namespace bot::translator;
using namespace std::chrono_literals;
using namespace std::string_literals;

mozhi::mozhi(const std::string &hostname, uint16_t port, const std::string &url, bool tls, const std::string &engine) :
    m_hostname(hostname), m_port(port), m_url(url), m_tls(tls), m_engine(engine)
{
}

mozhi::~mozhi()
{
}

const std::vector<language> mozhi::get_languages()
{
    if (!m_languages.languages.empty()) {
        auto current_time = std::chrono::system_clock::now();
        auto threshold_time = m_languages.query_time + 24h;
        if (current_time <= threshold_time)
            return m_languages.languages;
    }

    try {
        const std::string parameters = dpp::utility::make_url_parameters({
            {"engine"s, m_engine}
        });
        dpp::https_client http_request(m_hostname, m_port, m_url + "api/target_languages" + parameters, "GET", {}, {}, !m_tls);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_array()) {
                m_languages.languages.clear();
                for (auto json_language = response.begin(); json_language != response.end(); json_language++) {
                    if (json_language->is_object()) {
                        language language;

                        auto json_lang_code = json_language->find("Id");
                        if (json_lang_code != json_language->end())
                            language.code = *json_lang_code;

                        auto json_lang_name = json_language->find("Name");
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

const std::string mozhi::translate(const std::string &text, const std::string &source, const std::string &target)
{
    try {
        const std::string parameters = dpp::utility::make_url_parameters({
            {"engine"s, m_engine},
            {"from"s, source},
            {"to"s, target},
            {"text"s, text},
        });
        dpp::https_client http_request(m_hostname, m_port, m_url + "api/translate" + parameters, "GET", {}, {}, !m_tls);
        if (http_request.get_status() == 200) {
            const dpp::json response = dpp::json::parse(http_request.get_content());
            if (response.is_object()) {
                auto tr_text = response.find("translated-text");
                if (tr_text != response.end())
                    return *tr_text;
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }

    return text;
}