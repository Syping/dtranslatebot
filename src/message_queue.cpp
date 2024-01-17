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

#include <thread>
#include "message_queue.h"
#include "settings.h"
using namespace std::chrono_literals;

inline bot::translated_message make_translated_message(const bot::message &message, const std::string &translated_message, const std::string &webhook)
{
    bot::translated_message tr_message;
    tr_message.author = message.author;
    tr_message.avatar = message.avatar;
    tr_message.message = translated_message;
    tr_message.webhook = webhook;
    return tr_message;
}

void bot::message_queue::add(const bot::message &message)
{
    m_mutex.lock();
    m_queue.push_back(message);
    m_mutex.unlock();
}

void bot::message_queue::run(bot::settings::settings *settings, bot::submit_queue *submit_queue)
{
    m_running = true;
    while (m_running) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            const bot::message message = m_queue.front();
            m_queue.erase(m_queue.begin());
            m_mutex.unlock();

            settings->lock();
            bot::settings::translate *translate = settings->get_translate();
            const std::string tr_apiKey = translate->apiKey;
            const std::string tr_hostname = translate->hostname;
            const uint16_t tr_port = translate->port;
            const std::string tr_url = translate->url;
            const bool tr_tls = translate->tls;
            settings->unlock();

            for (auto target = message.targets.begin(); target != message.targets.end(); target++) {
                dpp::json json_body = {
                    {"q", message.message},
                    {"source", message.source},
                    {"target", target->target},
                    {"format", "text"}
                };

                if (!tr_apiKey.empty())
                    json_body.emplace("apiKey", tr_apiKey);

                dpp::http_headers http_headers;
                http_headers.emplace("Content-Type", "application/json");

                std::string tr_message = message.message;

                try {
                    dpp::https_client http_request(tr_hostname, tr_port, tr_url, "POST", json_body.dump(), http_headers, !tr_tls);
                    if (http_request.get_status() == 200) {
                        const dpp::json response = dpp::json::parse(http_request.get_content());
                        if (response.is_object()) {
                            auto tr_text = response.find("translatedText");
                            if (tr_text != response.end())
                                tr_message = tr_text.value();
                        }
                    }
                }
                catch (const dpp::json::exception &exception) {
                    std::cerr << "Exception thrown while parsing translated JSON: " << exception.what() << std::endl;
                }
                catch (const std::exception &exception) {
                    std::cerr << "Exception thrown while translating: " << exception.what() << std::endl;
                }
                catch (...) {
                    std::cerr << "Exception thrown while translating: unknown" << std::endl;
                }

                submit_queue->add(make_translated_message(message, tr_message, target->webhook));
            }

            std::this_thread::yield();
        }
        else {
            m_mutex.unlock();
            std::this_thread::sleep_for(100ms);
        }
    }
}

void bot::message_queue::terminate()
{
    m_running = false;
}
