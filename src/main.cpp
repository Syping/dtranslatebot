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

#include <dpp/dpp.h>
#include <iostream>
#include <thread>
#include "message_queue.h"
#include "settings.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [json]" << std::endl;
        return 0;
    }

    bot::settings::settings settings;
    if (!settings.parse(argv[1]))
        return 0;

    dpp::cluster bot(settings.get_token(), dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    bot::submit_queue submit_queue;
    std::thread submit_queue_loop(&bot::submit_queue::run, &submit_queue, &bot);

    bot::message_queue message_queue;
    std::thread message_queue_loop(&bot::message_queue::run, &message_queue, &settings, &submit_queue);

    bot.on_message_create([&bot, &message_queue, &settings](const dpp::message_create_t &event) {
        if (event.msg.webhook_id) {
            const std::lock_guard<bot::settings::settings> guard(settings);

            // We will not translate messages from our own bot
            if (settings.is_translatebot(event.msg.webhook_id))
                return;
        }

        const std::lock_guard<bot::settings::settings> guard(settings);
        const bot::settings::channel *channel = settings.get_channel(event.msg.guild_id, event.msg.channel_id);
        if (channel) {
            bot::message message;
            message.id = event.msg.id;

            message.author = event.msg.member.get_nickname();
            if (message.author.empty())
                message.author = event.msg.author.global_name;

            message.avatar = event.msg.member.get_avatar_url(settings.get_avatar_size());
            if (message.avatar.empty())
                message.avatar = event.msg.author.get_avatar_url(settings.get_avatar_size());

            message.message = event.msg.content;
            message.source = channel->source;
            message.targets = channel->targets;
            message_queue.add(message);
        }
    });

    bot.start(dpp::st_wait);

    // It's unneccessary, but we choose to exit clean anyway
    message_queue.terminate();
    message_queue_loop.join();

    submit_queue.terminate();
    submit_queue_loop.join();

    return 0;
}
