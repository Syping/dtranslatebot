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

#ifndef SETTINGS_TYPES_H
#define SETTINGS_TYPES_H

#include <cstdint>
#include <dpp/snowflake.h>
#include <dpp/webhook.h>
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
        struct translator {
            std::string type;
            std::string hostname;
            uint16_t port;
            std::string url;
            bool tls;
            std::string apiKey;
        };
    }
}

#endif // SETTINGS_TYPES_H
