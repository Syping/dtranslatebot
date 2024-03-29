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
    class slashcommands {
    public:
        slashcommands() = delete;
        static void process_command_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        static void register_commands(dpp::cluster *bot, bot::settings::settings *settings);

    private:
        static void process_edit_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        static void process_deleted_webhook(bot::settings::settings *settings, dpp::snowflake webhook_id, const dpp::confirmation_callback_t &callback);
        static void process_list_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        static void process_translate_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event);
        static void process_translate_webhook_add_target(bot::settings::settings *settings, const dpp::slashcommand_t &event, const std::string &target, const dpp::confirmation_callback_t &callback);
        static void process_translate_webhook_new_channel(bot::settings::settings *settings, const dpp::slashcommand_t &event, const std::string &source, const std::string &target, const dpp::confirmation_callback_t &callback);
    };
}

#endif // SLASHCOMMANDS_H
