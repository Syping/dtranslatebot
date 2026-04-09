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

#ifndef SUBMIT_QUEUE_H
#define SUBMIT_QUEUE_H
#include <dpp/cluster.h>
#include <dpp/webhook.h>
#ifdef DTRANSLATEBOT_GUI
#include <functional>
#endif
#include <mutex>
#include <queue>
#include <string>
#ifdef DTRANSLATEBOT_GUI
#include <vector>
#endif

namespace bot {
    struct translated_direct_message {
        dpp::message_context_menu_t event;
        std::string message;
    };

    struct translated_guild_message {
        std::string author;
        std::string avatar;
        std::string message;
        dpp::webhook webhook;
    };

#ifdef DTRANSLATEBOT_GUI
    typedef std::function<void(size_t)> submit_queue_size_callback;
#endif
    typedef std::variant<translated_direct_message, translated_guild_message> translated_message;

    class submit_queue {
    public:
        submit_queue() = default;
        submit_queue(const submit_queue&) = delete;
        submit_queue& operator=(const submit_queue&) = delete;
        void add(const translated_message &message);
        void add(translated_message &&message);
        void run(dpp::cluster *bot);
#ifdef DTRANSLATEBOT_GUI
        size_t size();
        void size_callback_add(const submit_queue_size_callback &callback);
#endif
        void terminate();

    private:
        bool m_running;
        std::mutex m_mutex;
        std::queue<translated_message> m_queue;
#ifdef DTRANSLATEBOT_GUI
        std::vector<submit_queue_size_callback> m_callbacks;
#endif
    };
}

#endif // SUBMIT_QUEUE_H
