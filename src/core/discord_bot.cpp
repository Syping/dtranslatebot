/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2026 Syping
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

#include <dpp/once.h>
#include "discord_bot.h"
#include "slashcommands.h"
using namespace bot;
using namespace std::chrono_literals;

void discord_bot::run(std::shared_ptr<bot::settings::settings> settings, bool background, bool wait_for_translator) {
    if (m_running)
        terminate();
    for (;;) {
        for (const auto &log_callback : m_log_callbacks)
            log_callback("Requesting supported languages...", "Launch", false);
        if (!settings->get_translator()->get_languages().empty())
            break;
        else if (wait_for_translator)
            std::this_thread::sleep_for(5000ms);
        else
            throw std::runtime_error("Failed to initialise translateable languages");
    }
    m_settings = settings;
    m_bot = std::make_unique<dpp::cluster>(m_settings->token(), dpp::i_default_intents | dpp::i_direct_messages | dpp::i_message_content);
    m_bot->on_log([=](const dpp::log_t &event) {
        for (const auto &log_callback : m_log_callbacks)
            log_callback(event.message, "Log", false);
    });
    m_submit_queue_loop = std::make_unique<std::thread>(&bot::submit_queue::run, &m_submit_queue, m_bot.get());
    m_message_queue_loop = std::make_unique<std::thread>(&bot::message_queue::run, &m_message_queue, m_settings.get(), &m_submit_queue);
    m_bot->on_message_context_menu(std::bind(&bot::slashcommands::process_message_menu_event, &m_message_queue, m_bot.get(), m_settings.get(), std::placeholders::_1));
    m_bot->on_message_create(std::bind(&bot::message_queue::process_guild_message_event, &m_message_queue, m_bot.get(), m_settings.get(), std::placeholders::_1));
    m_bot->on_slashcommand(std::bind(&bot::slashcommands::process_command_event, m_bot.get(), m_settings.get(), std::placeholders::_1));
    m_bot->on_ready([=]([[maybe_unused]] const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>())
            bot::slashcommands::register_commands(m_bot.get(), m_settings.get());
    });
    for (const auto &log_callback : m_log_callbacks)
        log_callback("Starting bot...", "Launch", false);
    m_bot->start(background ? dpp::st_return : dpp::st_wait);
    m_running = true;
}

void discord_bot::terminate() {
    if (!m_running)
        return;
    if (std::thread *message_queue_loop = m_message_queue_loop.get()) {
        m_message_queue.terminate();
        message_queue_loop->join();
        m_message_queue_loop.reset();
    }
    if (std::thread *submit_queue_loop = m_submit_queue_loop.get()) {
        m_submit_queue.terminate();
        submit_queue_loop->join();
        m_submit_queue_loop.reset();
    }
    m_bot.reset();
    m_settings.reset();
    m_running = false;
}

bool discord_bot::is_running() {
    return m_running;
}

void discord_bot::log_callback_add(const bot::log::log_message_callback &log_callback) {
    m_log_callbacks.push_back(log_callback);
}

message_queue* discord_bot::get_message_queue() {
    return &m_message_queue;
}

submit_queue* discord_bot::get_submit_queue() {
    return &m_submit_queue;
}
