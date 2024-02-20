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

#include <sstream>
#include "slashcommands.h"
using namespace std::string_literals;

void bot::slashcommands::process_command_event(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event)
{
    if (event.command.get_command_name() == "edit")
        bot::slashcommands::process_edit_command(bot, settings, event);
    else if (event.command.get_command_name() == "list")
        bot::slashcommands::process_list_command(bot, settings, event);
    else if (event.command.get_command_name() == "translate" || event.command.get_command_name() == "translate_pref")
        bot::slashcommands::process_translate_command(bot, settings, event);
}

void bot::slashcommands::process_edit_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event)
{
    try {
        dpp::permission user_permissions = event.command.get_resolved_permission(event.command.usr.id);
        if (!user_permissions.has(dpp::p_manage_webhooks))
            throw dpp::exception("Unauthorized to use command");

        dpp::command_interaction interaction = event.command.get_command_interaction();
        if (interaction.options[0].name == "delete") {
            const std::lock_guard<bot::settings::settings> guard(*settings);
            if (bot::settings::channel *channel = settings->get_channel(event.command.guild_id, event.command.channel_id)) {
                const std::string target = std::get<std::string>(event.get_parameter("target"));

                std::shared_ptr<bot::database::database> database = settings->get_database();
                const bot::settings::channel db_channel = database->get_channel(event.command.guild_id, event.command.channel_id);

                if (db_channel.targets.empty()) {
                    event.reply(dpp::message("The current channel has no deleteable targets!").set_flags(dpp::m_ephemeral));
                }
                else if (target == "**") {
                    std::vector<std::string> targets;
                    for (auto db_target = db_channel.targets.begin(); db_target != db_channel.targets.end(); db_target++) {
                        targets.push_back(db_target->target);
                    }
                    for (auto target = channel->targets.begin(); target != channel->targets.end();) {
                        if (std::find(targets.begin(), targets.end(), target->target) != targets.end()) {
                            bot->delete_webhook(target->webhook.id, std::bind(&bot::slashcommands::process_deleted_webhook, settings, target->webhook.id, std::placeholders::_1));
                            target = channel->targets.erase(target);
                        }
                        else {
                            target++;
                        }
                    }

                    database->delete_channel(event.command.guild_id, event.command.channel_id);
                    database->sync();

                    event.reply(dpp::message("Deleteable targets have being deleted!").set_flags(dpp::m_ephemeral));
                }
                else {
                    bool target_found = false;
                    for (auto db_target = db_channel.targets.begin(); db_target != db_channel.targets.end(); db_target++) {
                        if (db_target->target == target) {
                            target_found = true;
                            break;
                        }
                    }

                    if (target_found) {
                        for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
                            if (_target->target == target) {
                                bot->delete_webhook(_target->webhook.id, std::bind(&bot::slashcommands::process_deleted_webhook, settings, _target->webhook.id, std::placeholders::_1));
                                channel->targets.erase(_target);
                                break;
                            }
                        }

                        if (db_channel.targets.size() == 1)
                            database->delete_channel(event.command.guild_id, event.command.channel_id);
                        else
                            database->delete_channel_target(event.command.guild_id, event.command.channel_id, target);
                        database->sync();

                        event.reply(dpp::message("Target have being deleted!").set_flags(dpp::m_ephemeral));
                    }
                    else {
                        event.reply(dpp::message("Target language is not being found or deleteable!").set_flags(dpp::m_ephemeral));
                    }
                }
            }
            else {
                event.reply(dpp::message("The current channel is not being translated!").set_flags(dpp::m_ephemeral));
            }
        }
        else if (interaction.options[0].name == "source") {
            const std::lock_guard<bot::settings::settings> guard(*settings);
            if (bot::settings::channel *channel = settings->get_channel(event.command.guild_id, event.command.channel_id)) {
                const std::string source = std::get<std::string>(event.get_parameter("source"));
                const std::vector<bot::translator::language> languages = settings->get_translator()->get_languages();

                std::ostringstream language_codes;
                bool source_valid = false;
                for (const bot::translator::language &language : languages) {
                    if (language.code == source) {
                        source_valid = true;
                        break;
                    }
                    language_codes << " " << language.code;
                }

                if (source_valid) {
                    channel->source = source;

                    std::shared_ptr<bot::database::database> database = settings->get_database();
                    database->set_channel_source(event.command.guild_id, event.command.channel_id, source);
                    database->sync();

                    event.reply(dpp::message("Source language have being updated!").set_flags(dpp::m_ephemeral));
                }
                else {
                    event.reply(dpp::message("Source language is not valid!\nAvailable languages are:" + language_codes.str()).set_flags(dpp::m_ephemeral));
                }
            }
            else {
                event.reply(dpp::message("The current channel is not being translated!").set_flags(dpp::m_ephemeral));
            }
        }
        else {
            throw std::invalid_argument("Option " + interaction.options[0].name + " is not known");
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
        event.reply(dpp::message("Exception while processing command:\n"s + exception.what()).set_flags(dpp::m_ephemeral));
    }
}

void bot::slashcommands::process_deleted_webhook(bot::settings::settings *settings, dpp::snowflake webhook_id, const dpp::confirmation_callback_t &callback)
{
    if (callback.is_error()) {
        std::cerr << "[Error] Failed to delete Webhook " << webhook_id << std::endl;
        return;
    }
    settings->erase_translatebot_webhook(webhook_id);
}

void bot::slashcommands::process_list_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event)
{
    try {
        dpp::command_interaction interaction = event.command.get_command_interaction();
        if (interaction.options[0].name == "channel") {
            const std::lock_guard<bot::settings::settings> guard(*settings);
            if (const bot::settings::channel *channel = settings->get_channel(event.command.guild_id, event.command.channel_id)) {
                std::ostringstream reply_translated;
                reply_translated << "**Channel <#" << channel->id << ">**\n";
                reply_translated << "Source: " << channel->source << '\n';
                reply_translated << "Targets: " << channel->targets.size();

                // We want give more information to users who can Manage Webhooks
                dpp::permission user_permissions = event.command.get_resolved_permission(event.command.usr.id);
                if (user_permissions.has(dpp::p_manage_webhooks)) {
                    std::shared_ptr<bot::database::database> database = settings->get_database();
                    const bot::settings::channel db_channel = database->get_channel(event.command.guild_id, event.command.channel_id);

                    for (auto target = channel->targets.begin(); target != channel->targets.end(); target++) {
                        reply_translated << "\n\n";
                        reply_translated << "**Target " << target->target << "**\n";
                        bool db_found = false;
                        for (auto db_target = db_channel.targets.begin(); db_target != db_channel.targets.end(); db_target++) {
                            if (db_target->target == target->target) {
                                db_found = true;
                                break;
                            }
                        }
                        reply_translated << "Deleteable: " << (db_found ? "Yes" : "No") << '\n';
                        reply_translated << "Webhook: " << target->webhook.id;
                    }
                }

                event.reply(dpp::message(reply_translated.str()).set_flags(dpp::m_ephemeral));
            }
            else {
                event.reply(dpp::message("The current channel is not being translated!").set_flags(dpp::m_ephemeral));
            }
        }
        else if (interaction.options[0].name == "guild") {
            const std::lock_guard<bot::settings::settings> guard(*settings);
            if (const bot::settings::guild *guild = settings->get_guild(event.command.guild_id)) {
                if (!guild->channel.empty()) {
                    std::ostringstream reply_translated;
                    for (auto channel = guild->channel.begin(); channel != guild->channel.end();) {
                        reply_translated << "**Channel <#" << channel->id << ">**\n";
                        reply_translated << "Source: " << channel->source << '\n';
                        reply_translated << "Targets: " << channel->targets.size();
                        if (++channel != guild->channel.end())
                            reply_translated << "\n\n";
                    }
                    event.reply(dpp::message(reply_translated.str()).set_flags(dpp::m_ephemeral));
                }
                else {
                    event.reply(dpp::message("The current guild have no translated channel!").set_flags(dpp::m_ephemeral));
                }
            }
            else {
                event.reply(dpp::message("The current guild have no translated channel!").set_flags(dpp::m_ephemeral));
            }
        }
        else {
            throw std::invalid_argument("Option " + interaction.options[0].name + " is not known");
        }
    }
    catch (const std::exception& exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
        event.reply(dpp::message("Exception while processing command:\n"s + exception.what()).set_flags(dpp::m_ephemeral));
    }
}

void bot::slashcommands::process_translate_command(dpp::cluster *bot, bot::settings::settings *settings, const dpp::slashcommand_t &event)
{
    try {
        dpp::permission user_permissions = event.command.get_resolved_permission(event.command.usr.id);
        if (!user_permissions.has(dpp::p_manage_webhooks))
            throw dpp::exception("Unauthorized to use command");

        std::variant<dpp::channel,dpp::webhook> v_target;
        const std::string source = std::get<std::string>(event.get_parameter("source"));
        const std::string target = std::get<std::string>(event.get_parameter("target"));

        dpp::command_interaction interaction = event.command.get_command_interaction();
        if (interaction.options[0].name == "channel") {
            v_target = event.command.get_resolved_channel(
                           std::get<dpp::snowflake>(event.get_parameter("channel")));
        }
        else if (interaction.options[0].name == "webhook") {
            v_target = dpp::webhook(std::get<std::string>(event.get_parameter("webhook")));
        }

        const std::vector<bot::translator::language> languages = settings->get_translator()->get_languages();

        std::ostringstream language_codes;
        bool source_valid = false, target_valid = false;
        for (const bot::translator::language &language : languages) {
            if (language.code == source)
                source_valid = true;
            if (language.code == target)
                target_valid = true;
            if (source_valid && target_valid)
                break;
            language_codes << " " << language.code;
        }

        if (source_valid && target_valid) {
            const std::lock_guard<bot::settings::settings> guard(*settings);
            const bot::settings::channel *channel = settings->get_channel(event.command.guild_id, event.command.channel_id);
            if (!channel) {
                if (dpp::channel *channel = std::get_if<dpp::channel>(&v_target)) {
                    dpp::webhook webhook;
                    webhook.channel_id = channel->id;
                    webhook.guild_id = channel->guild_id;
                    webhook.name = "Translate Bot Webhook <" + std::to_string(event.command.channel_id) + ":" + source + ":" + target + ">";

                    bot->create_webhook(webhook, std::bind(&bot::slashcommands::process_translate_webhook_new_channel, settings, event, source, target, std::placeholders::_1));
                }
                else if (dpp::webhook *webhook = std::get_if<dpp::webhook>(&v_target)) {
                    const bot::settings::target s_target = { target, *webhook };
                    const bot::settings::channel s_channel = { event.command.channel_id, source, { s_target } };

                    settings->add_channel(s_channel, event.command.guild_id);
                    settings->add_translatebot_webhook(webhook->id);

                    std::shared_ptr<bot::database::database> database = settings->get_database();
                    database->set_channel_source(event.command.guild_id, event.command.channel_id, source);
                    database->add_channel_target(event.command.guild_id, event.command.channel_id, s_target);
                    database->sync();

                    event.reply(dpp::message("Channel will be now translated!").set_flags(dpp::m_ephemeral));
                }
            }
            else if (!settings->get_target(channel, target)) {
                if (channel->source != source) {
                    event.reply(dpp::message("The current channel is already being translated from a different source language!").set_flags(dpp::m_ephemeral));
                }
                else if (dpp::channel *channel = std::get_if<dpp::channel>(&v_target)) {
                    dpp::webhook webhook;
                    webhook.channel_id = channel->id;
                    webhook.guild_id = channel->guild_id;
                    webhook.name = "Translate Bot Webhook <" + std::to_string(event.command.channel_id) + ":" + source + ":" + target + ">";

                    bot->create_webhook(webhook, std::bind(&bot::slashcommands::process_translate_webhook_add_target, settings, event, target, std::placeholders::_1));
                }
                else if (dpp::webhook *webhook = std::get_if<dpp::webhook>(&v_target)) {
                    const bot::settings::target s_target = { target, *webhook };

                    settings->add_target(s_target, event.command.guild_id, event.command.channel_id);
                    settings->add_translatebot_webhook(webhook->id);

                    std::shared_ptr<bot::database::database> database = settings->get_database();
                    database->add_channel_target(event.command.guild_id, event.command.channel_id, s_target);
                    database->sync();

                    event.reply(dpp::message("Channel will be now translated!").set_flags(dpp::m_ephemeral));
                }
            }
            else {
                event.reply(dpp::message("The current channel is already being translated to the target language!").set_flags(dpp::m_ephemeral));
            }
        }
        else if (!source_valid && !target_valid) {
            event.reply(dpp::message("Source and target languages are not valid!\nAvailable languages are:" + language_codes.str()).set_flags(dpp::m_ephemeral));
        }
        else if (!source_valid) {
            event.reply(dpp::message("Source language is not valid!\nAvailable languages are:" + language_codes.str()).set_flags(dpp::m_ephemeral));
        }
        else if (!target_valid) {
            event.reply(dpp::message("Target language is not valid!\nAvailable languages are:" + language_codes.str()).set_flags(dpp::m_ephemeral));
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
        event.reply(dpp::message("Exception while processing command:\n"s + exception.what()).set_flags(dpp::m_ephemeral));
    }
}

void bot::slashcommands::process_translate_webhook_add_target(bot::settings::settings *settings, const dpp::slashcommand_t &event, const std::string &target, const dpp::confirmation_callback_t &callback)
{
    if (callback.is_error()) {
        event.reply(dpp::message("Failed to generate webhook!").set_flags(dpp::m_ephemeral));
        return;
    }
    const dpp::webhook webhook = callback.get<dpp::webhook>();

    const bot::settings::target s_target = { target, webhook };

    const std::lock_guard<bot::settings::settings> guard(*settings);
    settings->add_target(s_target, event.command.guild_id, event.command.channel_id);
    settings->add_translatebot_webhook(webhook.id);

    std::shared_ptr<bot::database::database> database = settings->get_database();
    database->add_channel_target(event.command.guild_id, event.command.channel_id, s_target);
    database->sync();

    event.reply(dpp::message("Channel will be now translated!").set_flags(dpp::m_ephemeral));
}

void bot::slashcommands::process_translate_webhook_new_channel(bot::settings::settings *settings, const dpp::slashcommand_t &event, const std::string &source, const std::string &target, const dpp::confirmation_callback_t &callback)
{
    if (callback.is_error()) {
        event.reply(dpp::message("Failed to generate webhook!").set_flags(dpp::m_ephemeral));
        return;
    }
    const dpp::webhook webhook = callback.get<dpp::webhook>();

    const bot::settings::target s_target = { target, webhook };
    const bot::settings::channel s_channel = { event.command.channel_id, source, { s_target } };

    const std::lock_guard<bot::settings::settings> guard(*settings);
    settings->add_channel(s_channel, event.command.guild_id);
    settings->add_translatebot_webhook(webhook.id);

    std::shared_ptr<bot::database::database> database = settings->get_database();
    database->set_channel_source(event.command.guild_id, event.command.channel_id, source);
    database->add_channel_target(event.command.guild_id, event.command.channel_id, s_target);
    database->sync();

    event.reply(dpp::message("Channel will be now translated!").set_flags(dpp::m_ephemeral));
}

void bot::slashcommands::register_commands(dpp::cluster *bot, bot::settings::settings *settings)
{
    settings->lock();
    const std::vector<bot::translator::language> languages = settings->get_translator()->get_languages();
    const std::vector<std::string> preferred_languages = settings->preferred_languages();
    settings->unlock();

    std::vector<dpp::slashcommand> commands;

    dpp::command_option source_option(dpp::co_string, "source", "Source language (ISO 639-1)", true);
    source_option.set_max_length(static_cast<int64_t>(2)).set_min_length(static_cast<int64_t>(2));
    dpp::command_option target_option(dpp::co_string, "target", "Target language (ISO 639-1)", true);
    target_option.set_max_length(static_cast<int64_t>(2)).set_min_length(static_cast<int64_t>(2));

    dpp::slashcommand command_edit("edit", "Edit current channel settings", bot->me.id);
    command_edit.set_default_permissions(dpp::p_manage_webhooks);
    dpp::command_option delete_edit_subcommand(dpp::co_sub_command, "delete", "Delete current channel target language");
    dpp::command_option source_edit_subcommand(dpp::co_sub_command, "source", "Edit current channel source language");
    delete_edit_subcommand.add_option(target_option);
    source_edit_subcommand.add_option(source_option);
    command_edit.add_option(delete_edit_subcommand);
    command_edit.add_option(source_edit_subcommand);
    commands.push_back(command_edit);

    dpp::slashcommand command_list("list", "List translation settings", bot->me.id);
    dpp::command_option channel_list_subcommand(dpp::co_sub_command, "channel", "List current channel translation settings");
    dpp::command_option guild_list_subcommand(dpp::co_sub_command, "guild", "List current guild translation settings");
    command_list.add_option(channel_list_subcommand);
    command_list.add_option(guild_list_subcommand);
    commands.push_back(command_list);

    dpp::slashcommand command_translate("translate", "Translate current channel", bot->me.id);
    command_translate.set_default_permissions(dpp::p_manage_webhooks);
    dpp::command_option channel_translate_subcommand(dpp::co_sub_command, "channel", "Translate current channel to a channel");
    dpp::command_option webhook_translate_subcommand(dpp::co_sub_command, "webhook", "Translate current channel to a webhook");
    dpp::command_option channel_option(dpp::co_channel, "channel", "Target channel", true);
    channel_option.add_channel_type(dpp::CHANNEL_TEXT);
    dpp::command_option webhook_option(dpp::co_string, "webhook", "Target webhook", true);
    channel_translate_subcommand.add_option(source_option);
    channel_translate_subcommand.add_option(target_option);
    channel_translate_subcommand.add_option(channel_option);
    webhook_translate_subcommand.add_option(source_option);
    webhook_translate_subcommand.add_option(target_option);
    webhook_translate_subcommand.add_option(webhook_option);
    command_translate.add_option(channel_translate_subcommand);
    command_translate.add_option(webhook_translate_subcommand);
    commands.push_back(command_translate);

    if (preferred_languages.size() > 1) {
        dpp::slashcommand command_translate_pref("translate_pref", "Translate current channel (Preferred languages)", bot->me.id);
        command_translate_pref.set_default_permissions(dpp::p_manage_webhooks);
        dpp::command_option channel_pref_subcommand(dpp::co_sub_command, "channel", "Translate current channel to a channel (Preferred languages)");
        dpp::command_option webhook_pref_subcommand(dpp::co_sub_command, "webhook", "Translate current channel to a webhook (Preferred languages)");
        dpp::command_option source_pref_option(dpp::co_string, "source", "Source language", true);
        dpp::command_option target_pref_option(dpp::co_string, "target", "Target language", true);
        for (const bot::translator::language &language : languages) {
            if (std::find(preferred_languages.begin(), preferred_languages.end(), language.code) != preferred_languages.end()) {
                source_pref_option.add_choice(dpp::command_option_choice(language.name, language.code));
                target_pref_option.add_choice(dpp::command_option_choice(language.name, language.code));
            }
        }
        channel_pref_subcommand.add_option(source_pref_option);
        channel_pref_subcommand.add_option(target_pref_option);
        channel_pref_subcommand.add_option(channel_option);
        webhook_pref_subcommand.add_option(source_pref_option);
        webhook_pref_subcommand.add_option(target_pref_option);
        webhook_pref_subcommand.add_option(webhook_option);
        command_translate_pref.add_option(channel_pref_subcommand);
        command_translate_pref.add_option(webhook_pref_subcommand);
        commands.push_back(command_translate_pref);
    }

    bot->global_bulk_command_create(commands);
}
