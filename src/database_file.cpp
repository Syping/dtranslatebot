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

#ifdef __unix__
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <cctype>
#include <dpp/discordclient.h>
#include <dpp/json.h>
#include <dpp/utility.h>
#include <fstream>
#include <iostream>
#include <thread>
#include "database_file.h"
using namespace bot::database;
using namespace std::string_literals;

file::file(const std::filesystem::path &storage_path) : m_storagePath(storage_path)
{
    std::cout << "[Launch] Checking storage directory..." << std::endl;
    if (!std::filesystem::is_directory(storage_path)) {
        std::cerr << "[Error] Storage directory " << storage_path << " can not be found" << std::endl;
        throw std::runtime_error("Storage directory can not be found");
    }

#ifdef __unix__
    struct flock lock;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    const std::string lock_file = storage_path / ".lock";
    fd = open(lock_file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        std::cerr << "[Error] Storage directory " << storage_path << " can not be locked" << std::endl;
        throw std::system_error(errno, std::system_category());
    }
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        close(fd);
        std::cerr << "[Error] Storage directory " << storage_path << " can not be locked" << std::endl;
        throw std::system_error(errno, std::system_category());
    }
#endif
}

file::~file()
{
    const std::lock_guard<std::mutex> guard(m_mutex);
#ifdef __unix__
    close(fd);
#endif
}

void file::add_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const bot::settings::target &target)
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
                        if (_target->target == target.target) {
                            _target->webhook = target.webhook;
                            return;
                        }
                    }

                    channel->targets.push_back(target);
                    return;
                }
            }

            bot::settings::channel channel;
            cache_get_channel(channel_id, &channel);
            channel.targets.push_back(target);
            guild->channel.push_back(std::move(channel));
            cache_add_channel(guild_id, channel_id);
            return;
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    channel.targets.push_back(target);

    bot::settings::guild guild;
    guild.id = guild_id;
    guild.channel.push_back(std::move(channel));
    cache_add_channel(guild_id, channel_id);

    m_dataCache.push_back(std::move(guild));
}

void file::delete_channel(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    guild->channel.erase(channel);
                    break;
                }
            }
            break;
        }
    }

    for (auto guild = m_channelCache.begin(); guild != m_channelCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (*channel == channel_id) {
                    guild->channel.erase(channel);
                    break;
                }
            }
            break;
        }

        std::vector<dpp::snowflake> channels;
        cache_guild(guild_id, &channels);
        for (auto channel = channels.begin(); channel != channels.end(); channel++) {
            if (*channel == channel_id) {
                channels.erase(channel);
                break;
            }
        }

        bot::database::guild _guild;
        _guild.id = guild_id;
        _guild.channel = std::move(channels);
        m_channelCache.push_back(std::move(_guild));
    }

    const std::filesystem::path channel_file = m_storagePath / "channel" / (std::to_string(channel_id) + ".json");
    std::filesystem::remove(channel_file);
}

void file::delete_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
                        if (_target->target == target) {
                            channel->targets.erase(_target);
                            return;
                        }
                    }
                    return;
                }
            }

            bot::settings::channel channel;
            cache_get_channel(channel_id, &channel);
            for (auto _target = channel.targets.begin(); _target != channel.targets.end(); _target++) {
                if (_target->target == target) {
                    channel.targets.erase(_target);
                    guild->channel.push_back(std::move(channel));
                    return;
                }
            }
            return;
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    for (auto _target = channel.targets.begin(); _target != channel.targets.end(); _target++) {
        if (_target->target == target) {
            channel.targets.erase(_target);
            break;
        }
    }

    bot::settings::guild guild;
    guild.id = guild_id;
    guild.channel.push_back(std::move(channel));

    m_dataCache.push_back(std::move(guild));
}

void file::delete_guild(dpp::snowflake guild_id)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            m_dataCache.erase(guild);
            break;
        }
    }

    for (auto guild = m_channelCache.begin(); guild != m_channelCache.end(); guild++) {
        if (guild->id == guild_id) {
            m_channelCache.erase(guild);
            break;
        }
    }

    const std::filesystem::path guild_file = m_storagePath / "guild" / (std::to_string(guild_id) + ".json");
    std::filesystem::remove(guild_file);
}

std::variant<std::monostate,bot::settings::target> file::find_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    for (auto _target = channel->targets.begin(); _target != channel->targets.end(); _target++) {
                        if (_target->target == target)
                            return *_target;
                    }
                    return {};
                }
            }
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    for (auto _target = channel.targets.begin(); _target != channel.targets.end(); _target++) {
        if (_target->target == target) {
            return *_target;
        }
    }
    return {};
}

std::vector<dpp::snowflake> file::get_channels(dpp::snowflake guild_id)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_channelCache.begin(); guild != m_channelCache.end(); guild++) {
        if (guild->id == guild_id) {
            return guild->channel;
        }
    }

    std::vector<dpp::snowflake> channels;
    cache_guild(guild_id, &channels);
    return channels;
}

std::string file::get_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id)
                    return channel->source;
            }
            break;
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    return channel.source;
}

std::vector<bot::settings::target> file::get_channel_targets(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    return channel->targets;
                }
            }
            break;
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    return channel.targets;
}

std::vector<dpp::snowflake> file::get_guilds()
{
    const std::lock_guard<std::mutex> guard(m_mutex);
    std::vector<dpp::snowflake> guilds;
    list_guilds(&guilds);
    return guilds;
}

void file::set_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &source)
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
        if (guild->id == guild_id) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++) {
                if (channel->id == channel_id) {
                    channel->source = source;
                    return;
                }
            }

            bot::settings::channel channel;
            cache_get_channel(channel_id, &channel);
            channel.source = source;
            guild->channel.push_back(std::move(channel));
            cache_add_channel(guild_id, channel_id);
            return;
        }
    }

    bot::settings::channel channel;
    cache_get_channel(channel_id, &channel);
    channel.source = source;

    bot::settings::guild guild;
    guild.id = guild_id;
    guild.channel.push_back(std::move(channel));
    cache_add_channel(guild_id, channel_id);

    m_dataCache.push_back(std::move(guild));
}

bool file::sync()
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    try {
        std::thread thread(&file::sync_cache, this);
        thread.detach();
        return true;
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }

    return false;
}

void file::cache_add_channel(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
    for (auto guild = m_channelCache.begin(); guild != m_channelCache.end(); guild++) {
        if (guild->id == guild_id) {
            if (std::find(guild->channel.begin(), guild->channel.end(), channel_id) == guild->channel.end())
                guild->channel.push_back(channel_id);
            return;
        }
    }

    std::vector<dpp::snowflake> channels;
    cache_guild(guild_id, &channels);
    if (std::find(channels.begin(), channels.end(), channel_id) == channels.end())
        channels.push_back(channel_id);

    bot::database::guild guild;
    guild.id = guild_id;
    guild.channel = std::move(channels);
    m_channelCache.push_back(std::move(guild));
}

void file::cache_get_channel(dpp::snowflake channel_id, bot::settings::channel *channel)
{
    channel->id = channel_id;

    const std::filesystem::path channel_file = m_storagePath / "channel" / (std::to_string(channel_id) + ".json");

    if (!std::filesystem::is_regular_file(channel_file))
        return;

    std::ifstream ifs(channel_file, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
        return;

    std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    try {
        const dpp::json json = dpp::json::parse(sdata);
        if (json.is_object()) {
            auto json_channel_source = json.find("source");
            if (json_channel_source != json.end())
                channel->source = *json_channel_source;

            auto json_channel_target = json.find("target");
            if (json_channel_target != json.end()) {
                if (json_channel_target->is_object()) {
                    for (auto json_target = json_channel_target->begin(); json_target != json_channel_target->end(); json_target++) {
                        bot::settings::target target;
                        target.target = json_target.key();
                        if (json_target->is_array()) {
                            if (json_target->size() == 2) {
                                target.webhook.id = std::stoull(std::string(json_target->front()));
                                target.webhook.token = json_target->back();
                            }
                        }
                        else if (json_target->is_string()) {
                            target.webhook = dpp::webhook(*json_target);
                        }
                        channel->targets.push_back(std::move(target));
                    }
                }
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }
}

void file::cache_guild(dpp::snowflake guild_id, std::vector<dpp::snowflake> *channels)
{
    const std::filesystem::path guild_file = m_storagePath / "guild" / (std::to_string(guild_id) + ".json");

    if (!std::filesystem::is_regular_file(guild_file))
        return;

    std::ifstream ifs(guild_file, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
        return;

    std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    try {
        const dpp::json json = dpp::json::parse(sdata);
        if (json.is_array()) {
            for (auto channel = json.begin(); channel != json.end(); channel++) {
                if (channel->is_number())
                    channels->push_back(*channel);
                else if (channel->is_string())
                    channels->push_back(std::stoull(std::string(*channel)));
            }
        }
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
    }
}

void file::list_guilds(std::vector<dpp::snowflake> *guilds)
{
    const std::filesystem::path guild_dir = m_storagePath / "guild";

    if (!std::filesystem::is_directory(guild_dir))
        return;

    for (const auto &guild_file : std::filesystem::directory_iterator(guild_dir)) {
        const std::filesystem::path &guild_file_path = guild_file.path();
        if (guild_file_path.extension() == "json") {
            const std::string &guild_filename = guild_file_path.filename();
            if (std::all_of(guild_filename.begin(), guild_filename.end(), ::isdigit)) {
                try {
                    dpp::snowflake guild_id = std::stoull(guild_filename);
                    guilds->push_back(guild_id);
                }
                catch (const std::exception &exception) {
                    std::cerr << "[Exception] " << exception.what() << std::endl;
                }
            }
        }
    }
}

void file::sync_cache()
{
    const std::lock_guard<std::mutex> guard(m_mutex);

    const std::filesystem::path channel_dir = m_storagePath / "channel";
    bool channel_dir_exists = std::filesystem::is_directory(channel_dir);
    if (!channel_dir_exists)
        channel_dir_exists = std::filesystem::create_directory(channel_dir);

    if (channel_dir_exists) {
        for (auto guild = m_dataCache.begin(); guild != m_dataCache.end(); guild++) {
            for (auto channel = guild->channel.begin(); channel != guild->channel.end();) {
                dpp::json target_json;
                for (auto target = channel->targets.begin(); target != channel->targets.end(); target++) {
                    target_json[target->target] = {
                        std::to_string(target->webhook.id), target->webhook.token
                    };
                }
                dpp::json channel_json = {
                    {"source"s, channel->source},
                    {"target"s, target_json}
                };

                const std::filesystem::path channel_file = m_storagePath / "channel" / (std::to_string(channel->id) + ".json");
                std::ofstream ofs(channel_file, std::ios::out | std::ios::binary | std::ios::trunc);
                if (ofs.is_open()) {
                    ofs << channel_json.dump();
                    bool ok = ofs.good();
                    ofs.close();
                    if (ok)
                        channel = guild->channel.erase(channel);
                    else
                        channel++;
                }
                else
                    channel++;
            }
        }
    }
    else {
        std::cerr << "[Error] Storage channel directory can not be created" << std::endl;
    }

    const std::filesystem::path guild_dir = m_storagePath / "guild";
    bool guild_dir_exists = std::filesystem::is_directory(guild_dir);
    if (!guild_dir_exists)
        guild_dir_exists = std::filesystem::create_directory(guild_dir);

    if (guild_dir_exists) {
        for (auto guild = m_channelCache.begin(); guild != m_channelCache.end();) {
            dpp::json guild_json;
            for (auto channel = guild->channel.begin(); channel != guild->channel.end(); channel++)
                guild_json.push_back(std::to_string(*channel));

            const std::filesystem::path guild_file = m_storagePath / "guild" / (std::to_string(guild->id) + ".json");
            std::ofstream ofs(guild_file, std::ios::out | std::ios::binary | std::ios::trunc);
            if (ofs.is_open()) {
                ofs << guild_json.dump();
                bool ok = ofs.good();
                ofs.close();
                if (ok)
                    guild = m_channelCache.erase(guild);
                else
                    guild++;
            }
            else
                guild++;
        }
    }
    else {
        std::cerr << "[Error] Storage guild directory can not be created" << std::endl;
    }
}
