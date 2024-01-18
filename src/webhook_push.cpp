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

bot::webhook_push::webhook_push(const std::string &webhook, const bot::translated_message &message, dpp::cluster *bot) :
    m_message(message)
{
    const dpp::json json_body = {
        {"content", message.message},
        {"username", message.author},
        {"avatar_url", message.avatar}
    };

    try {
        dpp::http_request webhook_request(webhook, nullptr, dpp::m_post, json_body.dump(), "application/json");
        const dpp::http_request_completion_t result = webhook_request.run(bot);
        if (result.status != 204)
            std::cerr << "Webhook push returned unexpected code " << result.status << " with response: " << result.body << std::endl;
        m_content = result.body;
        m_status = result.status;
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while Webhook push: " << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Exception thrown while Webhook push: unknown" << std::endl;
    }
}

const std::string bot::webhook_push::get_content() const
{
    return m_content;
}

uint16_t bot::webhook_push::get_status() const
{
    return m_status;
}
