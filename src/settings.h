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
#include <filesystem>
#include <mutex>
#include <string>
#include <vector>
#include "translate_core.h"

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
            void add_channel(const bot::settings::channel &channel, dpp::snowflake guild_id);
            bool add_target(const bot::settings::target &target, dpp::snowflake guild_id, dpp::snowflake channel_id);
            void add_translatebot_webhook(dpp::snowflake webhook_id);
            uint16_t get_avatar_size();
            const bot::settings::channel* get_channel(const bot::settings::guild *guild, dpp::snowflake channel_id);
            const bot::settings::channel* get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            const bot::settings::guild* get_guild(dpp::snowflake guild_id);
            const std::vector<std::string> get_preferred_languages();
            const std::filesystem::path get_storage_path();
            const bot::settings::translate* get_translate();
            std::unique_ptr<bot::translate::translator> get_translator();
            const std::string get_token();
            bool is_translatebot(dpp::snowflake webhook_id);
            void lock();
            bool parse(const std::string &filename);
            void unlock();

        private:
            uint16_t m_avatarSize;
            std::recursive_mutex m_mutex;
            std::vector<bot::settings::guild> m_guilds;
            std::vector<std::string> m_preflangs;
            std::filesystem::path m_storagepath;
            bot::settings::translate m_translate;
            std::string m_token;
            std::vector<dpp::snowflake> m_webhookIds;
        };
    }
}

#endif // SETTINGS_H
