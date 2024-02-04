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
#include <mutex>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "settings.h"
#include "translator_libretranslate.h"
using namespace bot::settings;

void process_guild_settings(const dpp::json &json, std::vector<guild> *guilds, std::vector<dpp::snowflake> *webhookIds)
{
    for (auto json_guild = json.begin(); json_guild != json.end(); json_guild++) {
        if (json_guild->is_object()) {
            guild guild;

            auto json_guild_id = json_guild->find("id");
            if (json_guild_id != json_guild->end()) {
                if (json_guild_id->is_number())
                    guild.id = *json_guild_id;
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
                            channel.id = *json_channel_id;
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
                            webhookIds->push_back(target.webhook.id);
                            channel.targets.push_back(std::move(target));
                        }
                        else if (json_channel_target->is_object()) {
                            for (auto json_target = json_channel_target->begin(); json_target != json_channel_target->end(); json_target++) {
                                target target;
                                target.target = json_target.key();
                                target.webhook = dpp::webhook(*json_target);
                                webhookIds->push_back(target.webhook.id);
                                channel.targets.push_back(std::move(target));
                            }
                        }
                    }

                    if (!channel.source.empty() && !channel.targets.empty())
                        guild.channel.push_back(std::move(channel));
                }
            }
            guilds->push_back(std::move(guild));
        }
    }
}

void process_preflang_settings(const dpp::json &json, std::vector<std::string> *preferred_langs)
{
    for (auto json_preferred_lang = json.begin(); json_preferred_lang != json.end(); json_preferred_lang++) {
        if (std::distance(json.begin(), json_preferred_lang) >= 25) {
            std::cerr << "[dtranslatebot] [ERROR] preferred_lang is limited to 25 languages" << std::endl;
            break;
        }
        preferred_langs->push_back(*json_preferred_lang);
    }
}

void process_user_settings(const dpp::json &json, uint16_t *avatar_size)
{
    auto json_avatar_size = json.find("avatar_size");
    if (json_avatar_size != json.end()) {
        *avatar_size = *json_avatar_size;
        if (*avatar_size < 16)
            *avatar_size = 16;
        else if (*avatar_size > 4096)
            *avatar_size = 4096;
    }
    else
        *avatar_size = 256;
}

bool process_translator_settings(const dpp::json &json, translator *translator)
{
    if (!json.is_object()) {
        std::cerr << "[dtranslatebot] [ERROR] Translator settings needs to be in a object" << std::endl;
        return false;
    }

    auto json_translate_hostname = json.find("hostname");
    if (json_translate_hostname == json.end()) {
        std::cerr << "[dtranslatebot] [ERROR] hostname can not be found in Translator settings" << std::endl;
        return false;
    }
    translator->hostname = *json_translate_hostname;

    auto json_translate_port = json.find("port");
    if (json_translate_port == json.end()) {
        std::cerr << "[dtranslatebot] [ERROR] port can not be found in Translator settings" << std::endl;
        return false;
    }
    translator->port = *json_translate_port;

    auto json_translate_url = json.find("url");
    if (json_translate_url == json.end()) {
        std::cerr << "[dtranslatebot] [ERROR] url can not be found in Translate settings" << std::endl;
        return false;
    }
    translator->url = *json_translate_url;

    auto json_translate_tls = json.find("tls");
    if (json_translate_tls != json.end())
        translator->tls = *json_translate_tls;
    else
        translator->tls = false;

    auto json_translate_apiKey = json.find("apiKey");
    if (json_translate_apiKey != json.end())
        translator->apiKey = *json_translate_apiKey;
    else
        translator->apiKey.clear();

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
    guild guild;
    guild.id = guild_id;
    guild.channel.push_back(std::move(channel));
    m_guilds.push_back(std::move(guild));
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

uint16_t settings::avatar_size()
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_avatarSize;
}

const channel* settings::get_channel(const guild *guild, dpp::snowflake channel_id) const
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[DEBUG] settings::get_channel(const guild*, dpp::snowflake) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
        if (channel->id == channel_id)
            return &(*channel);
    }
    return nullptr;
}

const channel* settings::get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id) const
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[DEBUG] settings::get_channel(dpp::snowflake, dpp::snowflake) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id)
                    return &(*channel);
            }
            return nullptr;
        }
    }
    return nullptr;
}

const guild* settings::get_guild(dpp::snowflake guild_id) const
{
    if (!m_externallyLockedCount) {
#ifndef NDEBUG
        std::cerr << "[DEBUG] settings::get_guild(dpp::snowflake) have being called without settings being locked." << std::endl;
#endif
        return nullptr;
    }
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id)
            return &(*guild);
    }
    return nullptr;
}

const std::vector<std::string> settings::preferred_languages() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_prefLangs;
}

const std::filesystem::path settings::storage_path() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    return m_storagePath;
}

std::unique_ptr<bot::translator::translator> settings::get_translator() const
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    std::unique_ptr<bot::translator::libretranslate> libretranslate(
                new bot::translator::libretranslate(m_translator.hostname, m_translator.port, m_translator.url, m_translator.tls, m_translator.apiKey));
    return libretranslate;
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

bool settings::parse(const std::string &data)
{
    try {
        dpp::json json;
        try {
            json = dpp::json::parse(data);
        }
        catch (const std::exception &exception) {
            std::cerr << "[dtranslatebot] [EXCEPTION] " << exception.what() << std::endl;
            std::cerr << "[dtranslatebot] [ERROR] Exception while parsing JSON" << std::endl;
            return false;
        }

        auto json_token = json.find("token");
        if (json_token == json.end()) {
            std::cerr << "[dtranslatebot] [ERROR] Bot token can not be found" << std::endl;
            return false;
        }

        const std::lock_guard<std::recursive_mutex> guard(m_mutex);
        m_token = *json_token;

        auto json_storage = json.find("storage");
        if (json_storage != json.end())
            m_storagePath = std::string(*json_storage);
        else if (char *storagepath = getenv("DTRANSLATEBOT_STORAGE"))
            m_storagePath = storagepath;

        if (m_storagePath.empty())
            m_storagePath = std::filesystem::current_path();

        auto json_translator = json.find("translator");
        if (json_translator == json.end()) {
            std::cerr << "[dtranslatebot] [ERROR] Translator settings can not be found" << std::endl;
            return false;
        }
        if (!process_translator_settings(*json_translator, &m_translator))
            return false;

        m_avatarSize = 256;
        m_guilds.clear();
        m_prefLangs.clear();
        m_webhookIds.clear();

        auto json_guilds = json.find("guilds");
        if (json_guilds != json.end() && json_guilds->is_object())
            process_guild_settings(*json_guilds, &m_guilds, &m_webhookIds);

        auto json_preflangs = json.find("preferred_lang");
        if (json_preflangs != json.end() && json_preflangs->is_array())
            process_preflang_settings(*json_preflangs, &m_prefLangs);

        auto json_user = json.find("user");
        if (json_user != json.end() && json_user->is_object())
            process_user_settings(*json_user, &m_avatarSize);

        return true;
    }
    catch (const std::exception &exception) {
        std::cerr << "[dtranslatebot] [EXCEPTION] " << exception.what() << std::endl;
    }
    return false;
}

bool settings::parse_file(const std::string &filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "[dtranslatebot] [ERROR] Failed to open JSON configuration file located at " << filename << std::endl;
        return false;
    }

    std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    return parse(std::move(sdata));
}

void settings::unlock()
{
    m_mutex.unlock();
    m_externallyLockedCount--;
}
