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

#include <thread>
#include "submit_queue.h"
#include "webhook_push.h"
using namespace bot;
using namespace std::chrono_literals;

void submit_queue::add(const translated_message &message)
{
    m_mutex.lock();
    m_queue.push(message);
    m_mutex.unlock();
}

void submit_queue::run(dpp::cluster *bot)
{
    m_running = true;
    while (m_running) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            const translated_message message = m_queue.front();
            m_queue.pop();
            m_mutex.unlock();

            webhook_push::run(message, bot);

            std::this_thread::yield();
        }
        else {
            m_mutex.unlock();
            std::this_thread::sleep_for(100ms);
        }
    }
}

void submit_queue::terminate()
{
    m_running = false;
}
