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
#include <mutex>
#include "database.h"
#include "settings_types.h"
#include "translator.h"

namespace bot {
    namespace settings {
        class settings {
        public:
            /* add functions */
            void add_channel(const channel &channel, dpp::snowflake guild_id);
            bool add_target(const target &target, dpp::snowflake guild_id, dpp::snowflake channel_id);
            void add_translatebot_webhook(dpp::snowflake webhook_id);

            /* erase functions */
            static void erase_channel(guild &guild, dpp::snowflake channel_id);
            void erase_guild(dpp::snowflake guild_id);
            void erase_translatebot_webhook(dpp::snowflake webhook_id);

            /* get functions */
            uint16_t avatar_size();
            static channel* get_channel(guild &guild, dpp::snowflake channel_id);
            channel* get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            guild* get_guild(dpp::snowflake guild_id);
            target* get_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target);
            static target* get_target(channel *channel, const std::string &target);
            static const target* get_target(const channel *channel, const std::string &target);
            const std::vector<std::string> preferred_languages() const;
            std::shared_ptr<bot::database::database> get_database() const;
            std::unique_ptr<bot::translator::translator> get_translator() const;
            const std::string token() const;

            /* is functions */
            bool is_translatebot(dpp::snowflake webhook_id) const;

            /* lock functions */
            void lock();
            void unlock();

            /* parse functions */
            bool parse(const std::string &data, bool initialize = true);
            bool parse_file(const std::string &filename, bool initialize = true);

            /* prevent copies */
            settings() = default;
            settings(const settings&) = delete;
            settings& operator=(const settings&) = delete;

        private:
            mutable std::recursive_mutex m_mutex;
            size_t m_externallyLockedCount = 0;
            uint16_t m_avatarSize = 256;
            std::shared_ptr<bot::database::database> m_database;
            std::vector<guild> m_guilds;
            std::vector<std::string> m_prefLangs;
            bot::settings::translator m_translator;
            std::string m_token;
            std::vector<dpp::snowflake> m_webhookIds;
        };
    }
}

#endif // SETTINGS_H
