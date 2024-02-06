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

#ifndef NDEBUG
#include <iostream>
#endif
#include "database_core.h"
using namespace bot::database;

database::database()
{
}

database::~database()
{
}

bool database::add_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const bot::settings::target &target)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::add_channel_target(dpp::snowflake, dpp::snowflake, const bot::settings::target&) have being called." << std::endl;
#endif
    return false;
}

std::variant<std::monostate,bot::settings::target> database::find_channel_target(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &target)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::find_channel_target(dpp::snowflake, dpp::snowflake, const std::string&) have being called." << std::endl;
#endif
    return {};
}

std::vector<dpp::snowflake> database::get_channels(dpp::snowflake guild_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channels(dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::string database::get_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channel_source(dpp::snowflake, dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::vector<bot::settings::target> database::get_channel_targets(dpp::snowflake guild_id, dpp::snowflake channel_id)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_channel_targets(dpp::snowflake, dpp::snowflake) have being called." << std::endl;
#endif
    return {};
}

std::vector<dpp::snowflake> database::get_guilds()
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::get_guilds() have being called." << std::endl;
#endif
    return {};
}

bool database::set_channel_source(dpp::snowflake guild_id, dpp::snowflake channel_id, const std::string &source)
{
#ifndef NDEBUG
    std::cerr << "[Debug] database::set_channel_source(dpp::snowflake, dpp::snowflake, const std::string&) have being called." << std::endl;
#endif
    return false;
}