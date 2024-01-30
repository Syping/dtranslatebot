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

#ifndef SUBMIT_QUEUE_H
#define SUBMIT_QUEUE_H
#include <dpp/dpp.h>
#include <mutex>
#include <string>
#include <queue>
#include <vector>

namespace bot {
    struct translated_message {
        std::string author;
        std::string avatar;
        std::string message;
        dpp::webhook webhook;
    };

    class submit_queue {
    public:
        void add(const bot::translated_message &message);
        void run(dpp::cluster *bot);
        void terminate();

    private:
        bool m_running;
        std::mutex m_mutex;
        std::queue<bot::translated_message> m_queue;
    };
}

#endif // SUBMIT_QUEUE_H
