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

    bot::message_queue message_queue;
    std::thread message_queue_loop(&bot::message_queue::run, &message_queue, &bot, &settings);

    bot.on_message_create([&bot, &message_queue, &settings](const dpp::message_create_t &event) {
        if (event.msg.author.is_bot())
            return;

        settings.lock();
        bot::settings::guild *guild = settings.get_guild(event.msg.guild_id);
        if (guild) {
            bot::settings::channel *channel = settings.get_channel(guild, event.msg.channel_id);
            if (channel) {
                bot::message message;
                message.author = event.msg.author.format_username();
                message.avatar = event.msg.author.avatar.to_string();
                message.message = event.msg.content;
                message.source = channel->source;
                message.targets = channel->targets;
                message_queue.add(message);
            }
        }
        settings.unlock();
    });

    bot.start(dpp::st_wait);

    return 0;
}
