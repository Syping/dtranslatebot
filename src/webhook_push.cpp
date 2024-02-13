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

#include <future>
#include "regex.h"
#include "webhook_push.h"
using namespace std::string_literals;
using namespace std::string_view_literals;

void bot::webhook_push::run(const bot::translated_message &message, dpp::cluster *bot)
{
    dpp::json json_body = {
        {"username"s, message.author},
        {"avatar_url"s, message.avatar}
    };

    // We will split too long messages into multiple messages
    if (message.message.length() > 2000) {
        std::string_view message_v = message.message;
        while (!message_v.empty()) {
            const std::string_view message_eov = message_v.substr(1333, 666);
            const std::string_view::size_type pos = message_eov.rfind('\n');
            if (pos != std::string_view::npos) {
                json_body["content"] = message_v.substr(0, 1333 + pos);
                message_v = message_v.substr(1333 + pos);
            }
            else {
                bot::svmatch match;
                if (bot::regex_match(message_eov.begin(), message_eov.end(), match, bot::regex("^.*(\\.|\\?|\\!|\\。)\\s.*$"s))) {
                    json_body["content"] = message_v.substr(0, 1334 + match.position(1));
                    message_v = message_v.substr(1334 + match.position(1));
                }
                else if (bot::regex_match(message_eov.begin(), message_eov.end(), match, bot::regex("^.*(\\,)\\s.*$"s))) {
                    json_body["content"] = message_v.substr(0, 1334 + match.position(1));
                    message_v = message_v.substr(1334 + match.position(1));
                }
                else if (bot::regex_match(message_eov.begin(), message_eov.end(), match, bot::regex("^.*()\\s.*$"s))) {
                    json_body["content"] = message_v.substr(0, 1334 + match.position(1));
                    message_v = message_v.substr(1334 + match.position(1));
                }
                else {
                    json_body["content"] = message_v.substr(0, 1333);
                    message_v = message_v.substr(1333);
                }
            }
            push_request(message.webhook.id, message.webhook.token, json_body.dump(), bot);

            if (message_v.length() <= 2000) {
                json_body["content"] = message_v;
                message_v = {};
                push_request(message.webhook.id, message.webhook.token, json_body.dump(), bot);
            }
        }
    }
    else {
        json_body["content"] = message.message;
        push_request(message.webhook.id, message.webhook.token, json_body.dump(), bot);
    }
}

void bot::webhook_push::push_request(dpp::snowflake webhook_id, const std::string &webhook_token, const std::string &json, dpp::cluster *bot)
{
    std::promise<dpp::http_request_completion_t> _p;
    std::future<dpp::http_request_completion_t> _f = _p.get_future();
    bot->post_rest(API_PATH "/webhooks", std::to_string(webhook_id), dpp::utility::url_encode(webhook_token), dpp::m_post, json, [&bot, &_p](dpp::json &json, const dpp::http_request_completion_t &event) {
        if (event.status != 204)
            std::cerr << "[Warning] Webhook push returned unexpected code " << event.status << std::endl;
        _p.set_value(event);
    });
    _f.wait();
}
