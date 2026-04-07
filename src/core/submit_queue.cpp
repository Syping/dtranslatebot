/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2024-2026 Syping
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
#include "submit_queue.h"
#include "webhook_push.h"
using bot::submit_queue;
using namespace std::chrono_literals;

void submit_queue::add(const translated_message &message)
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(message);
    for (const submit_queue_size_callback &callback : m_callbacks)
        callback(m_queue.size());
}

void submit_queue::add(translated_message &&message)
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(message);
    for (const submit_queue_size_callback &callback : m_callbacks)
        callback(m_queue.size());
}

void submit_queue::run(dpp::cluster *bot)
{
    m_running = true;
    while (m_running) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            const translated_message message = m_queue.front();
            m_queue.pop();
            for (const submit_queue_size_callback &callback : m_callbacks)
                callback(m_queue.size());
            m_mutex.unlock();

            if (const auto *direct_message = std::get_if<bot::translated_direct_message>(&message)) {
                direct_message->event.edit_original_response(dpp::message(direct_message->message).set_flags(dpp::m_ephemeral));
            }
            else if (const auto *guild_message = std::get_if<bot::translated_guild_message>(&message)) {
                webhook_push::run(*guild_message, bot);
            }

            std::this_thread::yield();
        }
        else {
            m_mutex.unlock();
            std::this_thread::sleep_for(100ms);
        }
    }
}

size_t submit_queue::size() {
    const std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.size();
}

void submit_queue::size_callback_add(const submit_queue_size_callback &callback) {
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_callbacks.push_back(callback);
}

/*
void submit_queue::size_callback_remove(const submit_queue_size_callback &callback) {
    const std::lock_guard<std::mutex> guard(m_mutex);
    m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), callback));
}
*/

void submit_queue::terminate()
{
    m_running = false;
}
