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

#include "stub.h"
using namespace bot::translator;
using namespace std::string_literals;

stub::stub()
{
}

stub::~stub()
{
}

const std::vector<language> stub::get_languages()
{
    return { {"a*", "Any Language"} };
}

const std::string stub::translate(const std::string &text, [[maybe_unused]] const std::string &source, [[maybe_unused]] const std::string &target)
{
    return text;
}
