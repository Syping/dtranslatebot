/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2023-2026 Syping
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
using bot::message_queue;
using namespace std::chrono_literals;
using namespace std::string_literals;

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

void message_queue::process_direct_message_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::message_context_menu_t &event)
{
    try {
        // We check for conditions we want to skip translation for
        if (event.ctx_message.author.id == bot->me.id || event.ctx_message.content.empty()) {
            event.reply(dpp::message("Invalid message").set_flags(dpp::m_ephemeral));
            return;
        }

        event.thinking(false);

        bot::direct_message direct_message;
        direct_message.event = event;
        direct_message.message = event.ctx_message.content;

        add(std::move(direct_message));
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
        event.reply(dpp::message("Exception while processing command:\n"s + exception.what()).set_flags(dpp::m_ephemeral));
    }
}

void message_queue::process_guild_message_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::message_create_t &event)
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
        bot::guild_message message;
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

            if (const auto *direct_message = std::get_if<bot::direct_message>(&message)) {
                translated_direct_message translated_message;
                translated_message.event = direct_message->event;
                translated_message.message = translator->translate(direct_message->message, {}, "en");
                submit_queue->add(std::move(translated_message));
            }
            else if (const auto *guild_message = std::get_if<bot::guild_message>(&message)) {
                for (auto target = guild_message->targets.begin(); target != guild_message->targets.end(); target++) {
                    translated_guild_message translated_message;
                    translated_message.author = guild_message->author;
                    translated_message.avatar = guild_message->avatar;
                    translated_message.message = translator->translate(guild_message->message, guild_message->source, target->target);
                    translated_message.webhook = target->webhook;
                    submit_queue->add(std::move(translated_message));
                }
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
