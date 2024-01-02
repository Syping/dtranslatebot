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

#ifndef QUEUE_H
#define QUEUE_H
#include <dpp/dpp.h>
#include <mutex>
#include <string>
#include <vector>
#include "settings.h"

namespace bot {
    struct message {
        std::string author;
        std::string avatar;
        std::string message;
        /* Webhook URL */
        std::string webhook;
        /* Translation Parameters */
        std::string source;
        std::string target;
    };

    class queue {
    public:
        void add(const bot::message &message);
        void run(dpp::cluster *bot, bot::settings::settings *settings);

    private:
        std::mutex m_mutex;
        std::vector<bot::message> m_queue;
    };
}

#endif //QUEUE_H
