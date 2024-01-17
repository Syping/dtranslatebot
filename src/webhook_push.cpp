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

#include <sstream>
#include "webhook_push.h"

bot::webhook_push::webhook_push(const dpp::webhook &webhook, const bot::translated_message &message) :
    m_webhook(webhook), m_message(message)
{
    const dpp::json json_body = {
        {"content", message.message},
        {"username", message.author},
        {"avatar_url", message.avatar}
    };

    dpp::http_headers http_headers;
    http_headers.emplace("Content-Type", "application/json");

    std::ostringstream webhook_url;
    webhook_url << "/api/webhooks/" << m_webhook.id << "/" << m_webhook.token;

    try {
        dpp::https_client https_request("discord.com", 443, webhook_url.str(), "POST", json_body.dump(), http_headers);
        m_content = https_request.get_content();
        m_status = https_request.get_status();
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while submitting: " << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Exception thrown while submitting: unknown" << std::endl;
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
