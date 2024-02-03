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
#include "translate_libretranslate.h"

void bot::settings::settings::add_channel(const bot::settings::channel &channel, dpp::snowflake guild_id)
{
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id) {
            guild->channel.push_back(channel);
            return;
        }
    }

    // We will create the guild structure when it is not in memory
    bot::settings::guild guild;
    guild.id = guild_id;
    guild.channel.push_back(std::move(channel));
    m_guilds.push_back(std::move(guild));
}

void bot::settings::settings::add_translatebot_webhook(dpp::snowflake webhook_id)
{
    m_webhookIds.push_back(webhook_id);
}

uint16_t bot::settings::settings::get_avatar_size()
{
    return m_avatarSize;
}

const bot::settings::channel* bot::settings::settings::get_channel(const bot::settings::guild *guild, dpp::snowflake channel_id)
{
    for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
        if (channel->id == channel_id)
            return &(*channel);
    }
    return nullptr;
}

const bot::settings::channel* bot::settings::settings::get_channel(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
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

const bot::settings::guild* bot::settings::settings::get_guild(dpp::snowflake guild_id)
{
    for (auto guild = m_guilds.begin(); guild != m_guilds.end(); guild++) {
        if (guild->id == guild_id)
            return &(*guild);
    }
    return nullptr;
}

const std::vector<std::string> bot::settings::settings::get_preferred_languages()
{
    return m_preflangs;
}

const std::filesystem::path bot::settings::settings::get_storage_path()
{
    return m_storagepath;
}

const bot::settings::translate* bot::settings::settings::get_translate()
{
    return &m_translate;
}

std::unique_ptr<bot::translate::translator> bot::settings::settings::get_translator()
{
    const std::lock_guard<std::recursive_mutex> guard(m_mutex);
    std::unique_ptr<bot::translate::libretranslate> libretranslate(
                new bot::translate::libretranslate(m_translate.hostname, m_translate.port, m_translate.url, m_translate.tls, m_translate.apiKey));
    return libretranslate;
}

const std::string bot::settings::settings::get_token()
{
    return m_token;
}

bool bot::settings::settings::is_translatebot(dpp::snowflake webhook_id)
{
    for (auto id = m_webhookIds.begin(); id != m_webhookIds.end(); id++) {
        if (*id == webhook_id)
            return true;
    }
    return false;
}

void bot::settings::settings::lock()
{
    m_mutex.lock();
}

bool bot::settings::settings::parse(const std::string &filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open JSON configuration file located at " << filename << std::endl;
        return false;
    }

    std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    try {
        const dpp::json json = dpp::json::parse(sdata);
        if (!json.is_object()) {
            std::cerr << "JSON configuration file is corrupt" << std::endl;
            return false;
        }

        auto json_token = json.find("token");
        if (json_token == json.end()) {
            std::cerr << "Bot token can not be found" << std::endl;
            return false;
        }

        const std::lock_guard<std::recursive_mutex> guard(m_mutex);
        m_token = json_token.value();

        auto json_storage = json.find("storage");
        if (json_storage != json.end())
            m_storagepath = std::string(json_storage.value());
        else if (char *storagepath = getenv("DTRANSLATEBOT_STORAGE"))
            m_storagepath = storagepath;

        if (m_storagepath.empty())
            m_storagepath = std::filesystem::current_path();

        // In future we should allow more services here, for now it's only LibreTranslate
        auto json_translate = json.find("translate");
        if (json_translate == json.end()) {
            std::cerr << "Translate settings can not be found" << std::endl;
            return false;
        }

        if (!json_translate->is_object()) {
            std::cerr << "Translate settings needs to be in a object" << std::endl;
            return false;
        }

        auto json_translate_hostname = json_translate.value().find("hostname");
        if (json_translate_hostname == json_translate.value().end()) {
            std::cerr << "\"hostname\" can not be found in Translate settings" << std::endl;
            return false;
        }
        m_translate.hostname = json_translate_hostname.value();

        auto json_translate_port = json_translate.value().find("port");
        if (json_translate_port == json_translate.value().end()) {
            std::cerr << "\"port\" can not be found in Translate settings" << std::endl;
            return false;
        }
        m_translate.port = json_translate_port.value();

        auto json_translate_url = json_translate.value().find("url");
        if (json_translate_url == json_translate.value().end()) {
            std::cerr << "\"url\" can not be found in Translate settings" << std::endl;
            return false;
        }
        m_translate.url = json_translate_url.value();

        auto json_translate_tls = json_translate.value().find("tls");
        if (json_translate_tls != json_translate.value().end())
            m_translate.tls = json_translate_tls.value();
        else
            m_translate.tls = false;

        auto json_translate_apiKey = json_translate.value().find("apiKey");
        if (json_translate_apiKey != json_translate.value().end())
            m_translate.apiKey = json_translate_apiKey.value();
        else
            m_translate.apiKey.clear();

        auto json_avatarSize = json.find("avatar_size");
        if (json_avatarSize != json.end()) {
            m_avatarSize = json_avatarSize.value();
            if (m_avatarSize < 16)
                m_avatarSize = 16;
            else if (m_avatarSize > 4096)
                m_avatarSize = 4096;
        }
        else
            m_avatarSize = 256;

        m_guilds.clear();
        m_preflangs.clear();
        m_webhookIds.clear();

        auto json_guilds = json.find("guilds");
        if (json_guilds != json.end()) {
            for (auto json_guild = json_guilds.value().begin(); json_guild != json_guilds.value().end(); json_guild++) {
                if (json_guild.value().is_object()) {
                    bot::settings::guild guild;

                    auto json_guild_id = json_guild.value().find("id");
                    if (json_guild_id != json_guild.value().end()) {
                        if (json_guild_id->is_number())
                            guild.id = json_guild_id.value();
                        else if (json_guild_id->is_string())
                            guild.id = std::stoull(std::string(json_guild_id.value()));
                        else
                            throw std::invalid_argument("Guild id is not a number or a string");
                    }
                    else
                        guild.id = std::stoull(json_guild.key());

                    for (auto json_channel = json_guild.value().begin(); json_channel != json_guild.value().end(); json_channel++) {
                        if (json_channel.value().is_object()) {
                            bot::settings::channel channel;

                            auto json_channel_id = json_channel.value().find("id");
                            if (json_channel_id != json_channel.value().end()) {
                                if (json_channel_id->is_number())
                                    channel.id = json_channel_id.value();
                                else if (json_channel_id->is_string())
                                    channel.id = std::stoull(std::string(json_channel_id.value()));
                                else
                                    throw std::invalid_argument("Channel id is not a number or a string");
                            }
                            else
                                channel.id = std::stoull(json_channel.key());

                            auto json_channel_source = json_channel.value().find("source");
                            if (json_channel_source != json_channel.value().end())
                                channel.source = json_channel_source.value();

                            auto json_channel_target = json_channel.value().find("target");
                            if (json_channel_target != json_channel.value().end()) {
                                if (json_channel_target.value().is_string()) {
                                    bot::settings::target target;
                                    target.target = json_channel_target.value();
                                    target.webhook = dpp::webhook(json_channel->at("webhook"));
                                    m_webhookIds.push_back(target.webhook.id);
                                    channel.targets.push_back(std::move(target));
                                }
                                else if (json_channel_target.value().is_object()) {
                                    for (auto json_target = json_channel_target.value().begin(); json_target != json_channel_target.value().end(); json_target++) {
                                        bot::settings::target target;
                                        target.target = json_target.key();
                                        target.webhook = dpp::webhook(json_target.value());
                                        m_webhookIds.push_back(target.webhook.id);
                                        channel.targets.push_back(std::move(target));
                                    }
                                }
                            }

                            if (!channel.source.empty() && !channel.targets.empty())
                                guild.channel.push_back(std::move(channel));
                        }
                    }
                    m_guilds.push_back(std::move(guild));
                }
            }
        }

        auto json_preflangs = json.find("preferred_lang");
        if (json_preflangs != json.end() && json_preflangs->is_array()) {
            size_t i = 0;
            for (const auto &json_preflang : json_preflangs.value()) {
                if (i >= 25) {
                    std::cerr << "\"preferred_lang\" is limited to 25 languages" << std::endl;
                    break;
                }
                m_preflangs.push_back(std::move(json_preflang));
                i++;
            }
        }

        return true;
    }
    catch (const dpp::json::exception &exception) {
        std::cerr << "Exception thrown while parsing configuration: " << exception.what() << std::endl;
    }
    catch (const std::exception &exception) {
        std::cerr << "Exception thrown while parsing configuration: " << exception.what() << std::endl;
    }
    return false;
}

void bot::settings::settings::unlock()
{
    m_mutex.unlock();
}
