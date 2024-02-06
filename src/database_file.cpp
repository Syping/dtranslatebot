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
#include <iostream>
#include "database_file.h"
using namespace bot::database;

file::file(const std::filesystem::path &storage_path) : m_storagePath(storage_path)
{
    std::cout << "[Launch] Checking storage directory..." << std::endl;
    if (!std::filesystem::exists(storage_path)) {
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
#ifdef __unix__
    close(fd);
#endif
}

bool file::add_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const bot::settings::target &target)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::add_channel_target(dpp::snowflake, dpp::snowflake, const bot::settings::target&) have being called." << std::endl;
#endif
    return false;
}

std::variant<std::monostate,bot::settings::target> file::find_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::find_channel_target(dpp::snowflake, dpp::snowflake, const std::string&) have being called." << std::endl;
#endif
    return {};
}

std::vector<dpp::snowflake> file::get_channels(dpp::snowflake guild_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channels(dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::string file::get_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channel_source(dpp::snowflake, dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::vector<bot::settings::target> file::get_channel_targets(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channel_targets(dpp::snowflake, dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::vector<dpp::snowflake> file::get_guilds()
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_guilds() have being called." << std::endl;
#endif
    return {};
}

bool file::set_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &source)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::set_channel_source(dpp::snowflake, dpp::snowflake, const std::string&) have being called." << std::endl;
#endif
    return false;
}
