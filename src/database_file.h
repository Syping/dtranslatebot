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

#ifndef DATABASE_FILE_H
#define DATABASE_FILE_H

#include <mutex>
#include <filesystem>
#ifdef _WIN32
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "database_core.h"

namespace bot {
    namespace database {
        class file : public database {
        public:
            explicit file(const std::filesystem::path &storage_path);
            ~file();
            void add_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const bot::settings::target &target) override;
            void delete_channel(dpp::snowflake guild_id, dpp::snowflake channel_id) override;
            void delete_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target) override;
            void delete_guild(dpp::snowflake guild_id) override;
            std::variant<std::monostate,bot::settings::target> find_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target) override;
            bot::settings::channel get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id) override;
            std::vector<dpp::snowflake> get_channels(dpp::snowflake guild_id) override;
            /* unused atm.
            std::string get_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id) override;
            std::vector<bot::settings::target> get_channel_targets(dpp::snowflake guild_id, dpp::snowflake channel_id) override;
            */
            std::vector<dpp::snowflake> get_guilds() override;
            void set_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &source) override;
            bool sync() override;

        private:
            void cache_add_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            void cache_get_channel(dpp::snowflake channel_id, bot::settings::channel *channel);
            void cache_guild(dpp::snowflake guild_id, std::vector<dpp::snowflake> *channels);
            void list_guilds(std::vector<dpp::snowflake> *guilds);
            void sync_cache();
#if defined(__unix__)
            int fd;
#elif defined(_WIN32)
            HANDLE fh;
#endif
            std::vector<bot::database::guild> m_channelCache;
            std::vector<bot::settings::guild> m_dataCache;
            std::mutex m_mutex;
            std::filesystem::path m_storagePath;
        };
    }
}

#endif // DATABASE_FILE_H
