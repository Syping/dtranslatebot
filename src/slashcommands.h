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

#ifndef SLASHCOMMANDS_H
#define SLASHCOMMANDS_H

#include <dpp/cluster.h>
#include "settings.h"

namespace bot {
    namespace slashcommands {
        extern void process_command_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        extern void process_translate_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        extern void register_commands(dpp::cluster *bot, bot::settings::settings *settings);
    }
}

#endif // SLASHCOMMANDS_H
