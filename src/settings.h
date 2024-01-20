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

#ifndef SETTINGS_H
#define SETTINGS_H
#include <cstdint>
#include <dpp/snowflake.h>
#include <dpp/webhook.h>
#include <mutex>
#include <string>
#include <vector>

namespace bot {
    namespace settings {
        struct target {
            std::string target;
            dpp::webhook webhook;
        };
        struct channel {
            dpp::snowflake id;
            std::string source;
            std::vector<bot::settings::target> targets;
        };
        struct guild {
            dpp::snowflake id;
            std::vector<bot::settings::channel> channel;
        };
        struct translate {
            std::string hostname;
            uint16_t port;
            std::string url;
            bool tls;
            std::string apiKey;
        };

        class settings {
        public:
            uint16_t get_avatar_size();
            const bot::settings::channel* get_channel(const bot::settings::guild *guild, dpp::snowflake channel_id);
            const bot::settings::channel* get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            const bot::settings::guild* get_guild(dpp::snowflake guild_id);
            const bot::settings::translate* get_translate();
            const std::string get_token();
            bool is_translatebot(dpp::snowflake webhook_id);
            void lock();
            bool parse(const std::string &filename);
            void unlock();

        private:
            uint16_t m_avatarSize;
            std::recursive_mutex m_mutex;
            std::vector<bot::settings::guild> m_guilds;
            bot::settings::translate m_translate;
            std::string m_token;
            std::vector<dpp::snowflake> m_webhookIds;
        };
    }
}

#endif // SETTINGS_H
