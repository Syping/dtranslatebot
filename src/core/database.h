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

#ifndef DATABASE_H
#define DATABASE_H

#include "settings_types.h"

namespace bot {
    namespace database {
        struct guild {
            dpp::snowflake id;
            std::vector<dpp::snowflake> channel;
        };

        class database {
        public:
            explicit database() = default;
            database(const database&) = delete;
            database& operator=(const database&) = delete;
            virtual ~database() = default;
            virtual void add_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const bot::settings::target &target);
            virtual void delete_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            virtual void delete_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target);
            virtual void delete_guild(dpp::snowflake guild_id);
            /* unused atm.
            virtual std::variant<std::monostate,bot::settings::target> find_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target);
            */
            virtual bot::settings::channel get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id);
            virtual std::vector<dpp::snowflake> get_channels(dpp::snowflake guild_id);
            /* unused atm.
            virtual std::string get_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id);
            virtual std::vector<bot::settings::target> get_channel_targets(dpp::snowflake guild_id, dpp::snowflake channel_id);
            */
            virtual std::vector<dpp::snowflake> get_guilds();
            virtual void set_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &source);
            virtual bool sync();
        };
    }
}

#endif // DATABASE_H
