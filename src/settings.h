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
#include "translator_core.h"

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
        struct translator {
            std::string hostname;
            uint16_t port;
            std::string url;
            bool tls;
            std::string apiKey;
        };

        class settings {
        public:
            /* add functions */
            void add_channel(const channel &channel, dpp::snowflake guild_id);
            bool add_target(const target &target, dpp::snowflake guild_id, dpp::snowflake channel_id);
            void add_translatebot_webhook(dpp::snowflake webhook_id);

            /* get functions */
            uint16_t avatar_size();
            const channel* get_channel(const guild *guild, dpp::snowflake channel_id) const;
            const channel* get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id) const;
            const guild* get_guild(dpp::snowflake guild_id) const;
            const std::vector<std::string> preferred_languages() const;
            const std::filesystem::path storage_path() const;
            std::unique_ptr<bot::translator::translator> get_translator() const;
            const std::string token() const;

            /* is functions */
            bool is_translatebot(dpp::snowflake webhook_id) const;

            /* lock functions */
            void lock();
            void unlock();

            /* parse functions */
            bool parse(const std::string &data);
            bool parse_file(const std::string &filename);

        private:
            mutable std::recursive_mutex m_mutex;
            size_t m_externallyLockedCount;
            uint16_t m_avatarSize;
            std::vector<guild> m_guilds;
            std::vector<std::string> m_prefLangs;
            std::filesystem::path m_storagePath;
            bot::settings::translator m_translator;
            std::string m_token;
            std::vector<dpp::snowflake> m_webhookIds;
        };
    }
}

#endif // SETTINGS_H
