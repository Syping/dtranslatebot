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

#include <dpp/json.h>
#include <fstream>
#include <iostream>
#include "settings.h"
#include "../database/file/file.h"
#include "../translator/deepl/deepl.h"
#include "../translator/mozhi/mozhi.h"
#include "../translator/libretranslate/libretranslate.h"
#include "../translator/lingvatranslate/lingvatranslate.h"
#include "../translator/stub/stub.h"
using namespace bot::settings;

void process_database_channels(std::shared_ptr<bot::database::database> database, bot::settings::guild &guild, std::vector<dpp::snowflake> &webhookIds)
{
    const std::vector<dpp::snowflake> db_channels = database->get_channels(guild.id);
    for (auto db_channel_id = db_channels.begin(); db_channel_id != db_channels.end(); db_channel_id++) {
        bool channel_found = false;
        for (auto channel = guild.channel.begin(); channel != guild.channel.end(); channel++) {
            if (channel->id == *db_channel_id) {
                const bot::settings::channel db_channel = database->get_channel(guild.id, channel->id);
                if (!db_channel.source.empty())
                    channel->source = db_channel.source;
                for (auto db_target = db_channel.targets.begin(); db_target != db_channel.targets.end(); db_target++) {
                    bool target_found = false;
                    for (auto target = channel->targets.begin(); target != channel->targets.end(); target++) {
                        if (target->target == db_target->target) {
                            target->webhook = db_target->webhook;
                            webhookIds.push_back(db_target->webhook.id);
                            target_found = true;
                            break;
                        }
                    }
                    if (!target_found) {
                        channel->targets.push_back(*db_target);
                        webhookIds.push_back(db_target->webhook.id);
                    }
                }
                channel_found = true;
                break;
            }
        }
        if (!channel_found) {
            const bot::settings::channel db_channel = database->get_channel(guild.id, *db_channel_id);
            guild.channel.push_back(db_channel);
            for (auto db_target = db_channel.targets.begin(); db_target != db_channel.targets.end(); db_target++)
                webhookIds.push_back(db_target->webhook.id);
        }
    }
}

void process_database(std::shared_ptr<bot::database::database> database, std::vector<guild> &guilds, std::vector<dpp::snowflake> &webhookIds)
{
    std::cout << "[Launch] Loading database..." << std::endl;
    const std::vector<dpp::snowflake> db_guilds = database->get_guilds();
    for (auto db_guild_id = db_guilds.begin(); db_guild_id != db_guilds.end(); db_guild_id++) {
        bool guild_found = false;
        for (auto guild = guilds.begin(); guild != guilds.end(); guild++) {
            if (guild->id == *db_guild_id) {
                process_database_channels(database, *guild, webhookIds);
                guild_found = true;
                break;
            }
        }
        if (!guild_found) {
            bot::settings::guild guild;
            guild.id = *db_guild_id;
            process_database_channels(database, guild, webhookIds);
            guilds.push_back(std::move(guild));
        }
    }
}

void process_guild_settings(const dpp::json &json, std::vector<guild> &guilds, std::vector<dpp::snowflake> &webhookIds)
{
    for (auto json_guild = json.begin(); json_guild != json.end(); json_guild++) {
        if (json_guild->is_object()) {
            guild guild;

            auto json_guild_id = json_guild->find("id");
            if (json_guild_id != json_guild->end()) {
                if (json_guild_id->is_number())
                    guild.id = static_cast<uint64_t>(*json_guild_id);
                else if (json_guild_id->is_string())
                    guild.id = std::stoull(std::string(*json_guild_id));
                else
                    throw std::invalid_argument("Guild id is not a number or a string");
            }
            else
                guild.id = std::stoull(json_guild.key());

            for (auto json_channel = json_guild->begin(); json_channel != json_guild->end(); json_channel++) {
                if (json_channel->is_object()) {
                    channel channel;

                    auto json_channel_id = json_channel->find("id");
                    if (json_channel_id != json_channel->end()) {
                        if (json_channel_id->is_number())
                            channel.id = static_cast<uint64_t>(*json_channel_id);
                        else if (json_channel_id->is_string())
                            channel.id = std::stoull(std::string(*json_channel_id));
                        else
                            throw std::invalid_argument("Channel id is not a number or a string");
                    }
                    else
                        channel.id = std::stoull(json_channel.key());

                    auto json_channel_source = json_channel->find("source");
                    if (json_channel_source != json_channel->end())
                        channel.source = *json_channel_source;

                    auto json_channel_target = json_channel->find("target");
                    if (json_channel_target != json_channel->end()) {
                        if (json_channel_target->is_string()) {
                            target target;
                            target.target = *json_channel_target;
                            target.webhook = dpp::webhook(json_channel->at("webhook"));
                            webhookIds.push_back(target.webhook.id);
                            channel.targets.push_back(std::move(target));
                        }
                        else if (json_channel_target->is_object()) {
                            for (auto json_target = json_channel_target->begin(); json_target != json_channel_target->end(); json_target++) {
                                target target;
                                target.target = json_target.key();
                                target.webhook = dpp::webhook(*json_target);
                                webhookIds.push_back(target.webhook.id);
                                channel.targets.push_back(std::move(target));
                            }
                        }
                    }

                    if (!channel.source.empty() && !channel.targets.empty())
                        guild.channel.push_back(std::move(channel));
                }
            }
            guilds.push_back(std::move(guild));
        }
    }
}

void process_preflang_settings(const dpp::json &json, std::vector<std::string> *preferred_langs)
{
    for (auto json_preferred_lang = json.begin(); json_preferred_lang != json.end(); json_preferred_lang++) {
        if (std::distance(json.begin(), json_preferred_lang) >= 25) {
            std::cerr << "[Error] Value preferred_lang is limited to 25 languages" << std::endl;
            break;
        }
        preferred_langs->push_back(*json_preferred_lang);
    }
}

void process_server_url(const std::string &url, translator &translator)
{
    std::string_view url_v = url;
    if (url_v.substr(0, 7) == "http://") {
        translator.tls = false;
        if (!translator.port)
            translator.port = 80;
        url_v = url_v.substr(7);
    }
    else if (url_v.substr(0, 8) == "https://") {
        translator.tls = true;
        if (!translator.port)
            translator.port = 443;
        url_v = url_v.substr(8);
    }
    else {
        translator.tls = false;
        if (!translator.port)
            translator.port = 80;
    }
    auto slash_pos = url_v.find_first_of('/');
    if (slash_pos != std::string_view::npos) {
        translator.url = url_v.substr(slash_pos);
        url_v = url_v.substr(0, slash_pos);
    }
    else {
        translator.url = "/";
        url_v = url_v.substr(0, slash_pos);
    }
    // We don't have IPv6 support here yet
    auto colon_pos = url_v.find_last_of(':');
    if (colon_pos != std::string_view::npos) {
        translator.hostname = url_v.substr(0, colon_pos);
        const int port = std::stoi(std::string(url_v.substr(colon_pos + 1)));
        if (port > 0 && port < 65536)
            translator.port = static_cast<uint16_t>(port);
        else
            throw std::invalid_argument("Port is out of range");
    }
    else {
        translator.hostname = url_v;
    }
}

bool process_server(const dpp::json &json, translator &translator)
{
    auto json_hostname = json.find("hostname");
    if (json_hostname != json.end())
        translator.hostname = *json_hostname;

    auto json_tls = json.find("tls");
    if (json_tls != json.end())
        translator.tls = *json_tls;
    else
        translator.tls = false;

    auto json_port = json.find("port");
    if (json_port != json.end())
        translator.port = *json_port;
    else
        translator.port = 0;

    auto json_url = json.find("url");
    if (json_url == json.end()) {
        std::cerr << "[Error] Value url not found in translator object" << std::endl;
        return false;
    }
    if (translator.hostname.empty())
        process_server_url(*json_url, translator);
    else
        translator.url = *json_url;

    auto json_apiKey = json.find("apiKey");
    if (json_apiKey != json.end())
        translator.apiKey = *json_apiKey;

    return true;
}

void process_user_settings(const dpp::json &json, uint16_t &avatar_size)
{
    auto json_avatar_size = json.find("avatar_size");
    if (json_avatar_size != json.end()) {
        avatar_size = *json_avatar_size;
        if (avatar_size < 16)
            avatar_size = 16;
        else if (avatar_size > 4096)
            avatar_size = 4096;
    }
}

bool process_translator_settings(const dpp::json &json, std::shared_ptr<bot::translator::translator> &translator_instance)
{
    if (!json.is_object()) {
        std::cerr << "[Error] Value translator needs to be a object" << std::endl;
        return false;
    }

    bot::settings::translator translator;
    auto json_translator_type = json.find("type");
    if (json_translator_type != json.end()) {
        translator.type = *json_translator_type;
        std::transform(translator.type.begin(), translator.type.end(), translator.type.begin(), ::tolower);
    }
    else {
        translator.type = "libretranslate";
    }

    if (translator.type == "deepl") {
        auto json_deepl_hostname = json.find("hostname");
        if (json_deepl_hostname != json.end())
            translator.hostname = *json_deepl_hostname;
        else
            translator.hostname = "api-free.deepl.com";

        auto json_deepl_apiKey = json.find("apiKey");
        if (json_deepl_apiKey == json.end()) {
            std::cerr << "[Error] DeepL requires API key for authorization" << std::endl;
            return false;
        }
        translator.apiKey = *json_deepl_apiKey;

        translator_instance = std::make_shared<bot::translator::deepl>(translator.hostname, translator.apiKey);
    }
    else if (translator.type == "mozhi") {
        if (!process_server(json, translator))
            return false;

        std::string mozhi_engine;
        auto json_mozhi_engine = json.find("engine");
        if (json_mozhi_engine != json.end())
            mozhi_engine = *json_mozhi_engine;
        else
            mozhi_engine = "google";

        translator_instance = std::make_shared<bot::translator::mozhi>(translator.hostname, translator.port, translator.url, translator.tls, mozhi_engine);
    }
    else if (translator.type == "libretranslate") {
        if (!process_server(json, translator))
            return false;

        translator_instance = std::make_shared<bot::translator::libretranslate>(translator.hostname, translator.port, translator.url, translator.tls, translator.apiKey);
    }
    else if (translator.type == "lingvatranslate") {
        if (!process_server(json, translator))
            return false;

        translator_instance = std::make_shared<bot::translator::lingvatranslate>(translator.hostname, translator.port, translator.url, translator.tls);
    }
    else if (translator.type == "stub") {
        translator_instance = std::make_shared<bot::translator::stub>();
    }
    else {
        std::cerr << "[Error] Translator " << translator.type << " is unknown" << std::endl;
        return false;
    }

    return true;
}

void settings::add_channel(const channel &channel, dpp::snowflake guild_id)
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            guild->channel.push_back(channel);
            return;
        }
    }

    // We will create the guild structure when it is not in memory
    m_guilds.push_back({ guild_id, { channel } });
}

bool settings::add_target(const target &target, dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    channel->targets.push_back(target);
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

void settings::add_translatebot_webhook(dpp::snowflake webhook_id)
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    m_webhookIds.push_back(webhook_id);
}

void settings::erase_channel(guild &guild, dpp::snowflake channel_id)
{
    for (auto channel = guild.channel.begin(); channel != guild.channel.end(); channel++) {
        if (channel->id == channel_id) {
            guild.channel.erase(channel);
            return;
        }
    }
}

void settings::erase_guild(dpp::snowflake guild_id)
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            m_guilds.erase(guild);
            return;
        }
    }
}

void settings::erase_translatebot_webhook(dpp::snowflake webhook_id)
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    auto webhook_it = std::find(m_webhookIds.begin(), m_webhookIds.end(), webhook_id);
    if (webhook_it != m_webhookIds.end())
        m_webhookIds.erase(webhook_it);
}

uint16_t settings::avatar_size()
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_avatarSize;
}

channel* settings::get_channel(guild &guild, dpp::snowflake channel_id)
{
    for (auto channel = guild.channel.begin(); channel != guild.channel.end(); channel++) {
        if (channel->id == channel_id)
            return &*channel;
    }
    return nullptr;
}

channel* settings::get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[Debug] settings::get_channel(dpp::snowflake, dpp::snowflake) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id)
                    return &*channel;
            }
            return nullptr;
        }
    }
    return nullptr;
}

guild* settings::get_guild(dpp::snowflake guild_id)
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[Debug] settings::get_guild(dpp::snowflake) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id)
            return &*guild;
    }
    return nullptr;
}

target* settings::get_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target)
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[Debug] settings::get_target(dpp::snowflake, dpp::snowflake, const std::string&) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
                        if (_target->target == target)
                            return &*_target;
                    }
                    return nullptr;
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

target* settings::get_target(channel *channel, const std::string &target)
{
    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
        if (_target->target == target)
            return &*_target;
    }
    return nullptr;
}

const target* settings::get_target(const channel *channel, const std::string &target)
{
    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
        if (_target->target == target)
            return &*_target;
    }
    return nullptr;
}

const std::vector<std::string> settings::preferred_languages() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_prefLangs;
}

std::shared_ptr<bot::database::database> settings::get_database() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_database;
}

std::shared_ptr<bot::translator::translator> settings::get_translator() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_translator;
}

const std::string settings::token() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_token;
}

bool settings::is_translatebot(dpp::snowflake webhook_id) const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    for (auto id = m_webhookIds.begin(); id != m_webhookIds.end(); id++) {
        if (*id == webhook_id)
            return true;
    }
    return false;
}

void settings::lock()
{
    m_mutex.lock();
    m_externallyLockedCount++;
}

bool settings::parse(const std::string &data, bool initialize)
{
    try {
        dpp::json json;
        try {
            json = dpp::json::parse(data, nullptr, true, true);
        }
        catch (const std::exception &exception) {
            std::cerr << "[Exception] " << exception.what() << std::endl;
            std::cerr << "[Error] Exception while parsing JSON" << std::endl;
            return false;
        }

        const std::lock_guard<std::recursive_mutex> guard(m_mutex);
        auto json_token = json.find("token");
        if (json_token != json.end())
            m_token = *json_token;
        else if (char *token = getenv("DTRANSLATEBOT_TOKEN"))
            m_token = token;

        if (m_token.empty()) {
            std::cerr << "[Error] Discord Bot Token is not configured" << std::endl;
            return false;
        }

        std::filesystem::path storage_path;
        auto json_storage = json.find("storage");
        if (json_storage != json.end())
            storage_path = std::string(*json_storage);
        else if (char *storagepath = getenv("DTRANSLATEBOT_STORAGE"))
            storage_path = storagepath;

        if (storage_path.empty())
            storage_path = std::filesystem::current_path();

        m_database = std::make_shared<bot::database::file>(storage_path);

        auto json_translator = json.find("translator");
        if (json_translator == json.end()) {
            std::cerr << "[Error] Value translator not found" << std::endl;
            return false;
        }
        if (!process_translator_settings(*json_translator, m_translator))
            return false;

        auto json_guilds = json.find("guilds");
        if (json_guilds != json.end() && json_guilds->is_object())
            process_guild_settings(*json_guilds, m_guilds, m_webhookIds);

        auto json_preflangs = json.find("preferred_lang");
        if (json_preflangs != json.end() && json_preflangs->is_array())
            process_preflang_settings(*json_preflangs, &m_prefLangs);

        auto json_user = json.find("user");
        if (json_user != json.end() && json_user->is_object())
            process_user_settings(*json_user, m_avatarSize);

        process_database(m_database, m_guilds, m_webhookIds);

        return true;
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }
    return false;
}

bool settings::parse_file(const std::string &filename, bool initialize)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "[Error] Failed to open JSON configuration file located at " << filename << std::endl;
        return false;
    }

    std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    return parse(sdata, initialize);
}

void settings::unlock()
{
    m_mutex.unlock();
    m_externallyLockedCount--;
}
