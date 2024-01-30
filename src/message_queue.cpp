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
using namespace std::chrono_literals;

inline bot::translated_message make_translated_message(const bot::message &message, const std::string &translated_message, const dpp::webhook &webhook)
{
    bot::translated_message tr_message;
    tr_message.author = message.author;
    tr_message.avatar = message.avatar;
    tr_message.message = translated_message;
    tr_message.webhook = webhook;
    return tr_message;
}

void bot::message_queue::add(const bot::message &message)
{
    m_mutex.lock();
    m_queue.push(message);
    m_mutex.unlock();
}

void bot::message_queue::run(bot::settings::settings *settings, bot::submit_queue *submit_queue)
{
    m_running = true;
    while (m_running) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            const bot::message message = m_queue.front();
            m_queue.pop();
            m_mutex.unlock();

            std::unique_ptr<bot::translate::translator> translator = settings->get_translator();

            for (auto target = message.targets.begin(); target != message.targets.end(); target++) {
                const std::string tr_message = translator->translate(message.message, message.source, target->target);
                submit_queue->add(make_translated_message(message, tr_message, target->webhook));
            }

            std::this_thread::yield();
        }
        else {
            m_mutex.unlock();
            std::this_thread::sleep_for(100ms);
        }
    }
}

void bot::message_queue::terminate()
{
    m_running = false;
}
