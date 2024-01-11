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
#include <mutex>
#include <string>
#include <vector>

namespace bot {
    namespace settings {
        struct channel {
            uint64_t id;
            std::string source;
            std::string target;
            std::string webhook;
        };
        struct guild {
            uint64_t id;
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
            bot::settings::channel* get_channel(bot::settings::guild *guild, uint64_t channel_id);
            bot::settings::guild* get_guild(uint64_t guild_id);
            bot::settings::translate* get_translate();
            const std::string get_token();
            void lock();
            bool parse(const std::string &filename);
            void unlock();

        private:
            std::recursive_mutex m_mutex;
            std::vector<bot::settings::guild> m_guilds;
            bot::settings::translate m_translate_settings;
            std::string m_token;
        };
    }
}

#endif //SETTINGS_H
