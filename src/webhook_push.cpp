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

#include "webhook_push.h"

void bot::webhook_push::run(const dpp::webhook &webhook, const bot::translated_message &message, dpp::cluster *bot)
{
    const dpp::json json_body = {
        {"content", message.message},
        {"username", message.author},
        {"avatar_url", message.avatar}
    };

    try {
        bot->post_rest(API_PATH "/webhooks", std::to_string(webhook.id), dpp::utility::url_encode(webhook.token), dpp::m_post, json_body.dump(), [bot](dpp::json &json, const dpp::http_request_completion_t &event) {
            if (event.status != 204)
                std::cerr << "Webhook push returned unexpected code " << event.status << " with response: " << event.body << std::endl;
        });
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while Webhook push: " << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Exception thrown while Webhook push: unknown" << std::endl;
    }
}
