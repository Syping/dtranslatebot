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

#ifndef WEBHOOK_PUSH_H
#define WEBHOOK_PUSH_H

#include <dpp/dpp.h>
#include "submit_queue.h"

namespace bot {
    class webhook_push {
    public:
        explicit webhook_push(const std::string &webhook, const bot::translated_message &message, dpp::cluster *bot);
        const std::string get_content() const;
        uint16_t get_status() const;

    private:
        std::string m_content;
        uint16_t m_status;
        bot::translated_message m_message;
    };
}

#endif // WEBHOOK_PUSH_H
