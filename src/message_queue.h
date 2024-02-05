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

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H
#include <dpp/cluster.h>
#include <mutex>
#include <string>
#include <queue>
#include <vector>
#include "settings.h"
#include "submit_queue.h"

namespace bot {
    struct message {
        uint64_t id;
        std::string author;
        std::string avatar;
        std::string message;
        std::string source;
        std::vector<bot::settings::target> targets;
    };

    class message_queue {
    public:
        void add(const message &message);
        void add(message &&message);
        void process_message_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::message_create_t &event);
        void run(bot::settings::settings *settings, submit_queue *submit_queue);
        void terminate();

    private:
        bool m_running;
        std::mutex m_mutex;
        std::queue<message> m_queue;
    };
}

#endif // MESSAGE_QUEUE_H
