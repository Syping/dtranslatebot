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

#include <dpp/cluster.h>
#include <dpp/once.h>
#include <iostream>
#include <vector>
#include <thread>
#include "message_queue.h"
#include "settings.h"
#include "slashcommands.h"
using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
    bool flag_wait_for_translator = false;
    std::vector<std::string> args;
    for (size_t i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--wait-for-translator"))
            flag_wait_for_translator = true;
        else
            args.push_back(argv[i]);
    }
    if (args.size() != 1) {
        std::cout << "Usage: " << argv[0] << " [--wait-for-translator] [json]" << std::endl;
        return 0;
    }

    std::cout << "[Launch] Processing configuration..." << std::endl;
    bot::settings::settings settings;
    if (!settings.parse_file(args.at(0)))
        return 1;

    for (;;) {
        std::cout << "[Launch] Requesting supported languages..." << std::endl;
        if (!settings.get_translator()->get_languages().empty()) {
            break;
        }
        else if (flag_wait_for_translator) {
            std::this_thread::sleep_for(5000ms);
        }
        else {
            std::cerr << "[Error] Failed to initialise translateable languages" << std::endl;
            return 1;
        }
    }

    dpp::cluster bot(settings.token(), dpp::i_default_intents | dpp::i_message_content);
    bot.on_log([&bot](const dpp::log_t &event) {
        std::cerr << "[Log] " << event.message << std::endl;
    });

    bot::submit_queue submit_queue;
    std::thread submit_queue_loop(&bot::submit_queue::run, &submit_queue, &bot);

    bot::message_queue message_queue;
    std::thread message_queue_loop(&bot::message_queue::run, &message_queue, &settings, &submit_queue);

    bot.on_message_create(std::bind(&bot::message_queue::process_message_event, &message_queue, &bot, &settings, std::placeholders::_1));
    bot.on_slashcommand(std::bind(&bot::slashcommands::process_command_event, &bot, &settings, std::placeholders::_1));
    bot.on_ready([&bot, &settings]([[maybe_unused]] const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot::slashcommands::register_commands(&bot, &settings);
        }
    });

    std::cout << "[Launch] Starting bot..." << std::endl;
    bot.start(dpp::st_wait);

    // It's unneccessary, but we choose to exit clean anyway
    message_queue.terminate();
    message_queue_loop.join();

    submit_queue.terminate();
    submit_queue_loop.join();

    return 0;
}
