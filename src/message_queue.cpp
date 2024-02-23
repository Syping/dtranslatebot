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
using namespace bot;
using namespace std::chrono_literals;

void message_queue::add(const message &message)
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(message);
}

void message_queue::add(message &&message)
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(message);
}

void message_queue::process_message_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::message_create_t &event)
{
    // We check for conditions we want to skip translation for
    if (event.msg.author.id == bot->me.id || event.msg.content.empty() || event.msg.has_thread())
        return;

    if (event.msg.webhook_id) {
        const std::lock_guard<bot::settings::settings> guard(*settings);

        // We will not translate messages from our own bot
        if (settings->is_translatebot(event.msg.webhook_id))
            return;
    }

    const std::lock_guard<bot::settings::settings> guard(*settings);
    if (const bot::settings::channel *channel = settings->get_channel(event.msg.guild_id, event.msg.channel_id)) {
        bot::message message;
        message.id = event.msg.id;

        message.author = event.msg.member.get_nickname();
        if (message.author.empty())
            message.author = event.msg.author.global_name;

        message.avatar = event.msg.member.get_avatar_url(settings->avatar_size());
        if (message.avatar.empty())
            message.avatar = event.msg.author.get_avatar_url(settings->avatar_size());

        message.message = event.msg.content;
        message.source = channel->source;
        message.targets = channel->targets;

        add(std::move(message));
    }
}

void message_queue::run(bot::settings::settings *settings, submit_queue *submit_queue)
{
    m_running = true;
    while (m_running) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            const message message = m_queue.front();
            m_queue.pop();
            m_mutex.unlock();

            auto translator = settings->get_translator();

            for (auto target = message.targets.begin(); target != message.targets.end(); target++) {
                translated_message tr_message;
                tr_message.author = message.author;
                tr_message.avatar = message.avatar;
                tr_message.message = translator->translate(message.message, message.source, target->target);
                tr_message.webhook = target->webhook;
                submit_queue->add(std::move(tr_message));
            }

            std::this_thread::yield();
        }
        else {
            m_mutex.unlock();
            std::this_thread::sleep_for(100ms);
        }
    }
}

void message_queue::terminate()
{
    m_running = false;
}
