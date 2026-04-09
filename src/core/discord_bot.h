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

#ifndef DISCORD_BOT_H
#define DISCORD_BOT_H

#include <dpp/cluster.h>
#include <memory>
#include "../core/log.h"
#include "../core/message_queue.h"
#include "../core/settings.h"
#include "../core/submit_queue.h"

namespace bot {
    class discord_bot {
    public:
        void run(std::shared_ptr<bot::settings::settings> settings, bool background, bool wait_for_translator = false);
        void terminate();
        void log_callback_add(const bot::log::log_message_callback &log_callback);
        bool is_running();
        message_queue* get_message_queue();
        submit_queue* get_submit_queue();

        /* prevent copies */
        explicit discord_bot() = default;
        discord_bot(const discord_bot&) = delete;
        discord_bot& operator=(const discord_bot&) = delete;

    private:
        bool m_running;
        bot::message_queue m_message_queue;
        bot::submit_queue m_submit_queue;
        std::shared_ptr<bot::settings::settings> m_settings;
        std::unique_ptr<dpp::cluster> m_bot;
        std::unique_ptr<std::thread> m_message_queue_loop;
        std::unique_ptr<std::thread> m_submit_queue_loop;
        std::vector<bot::log::log_message_callback> m_log_callbacks;
    };
}

#endif // DISCORD_BOT_H
