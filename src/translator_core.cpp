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

#include <iostream>
#include "translator_core.h"
using namespace bot::translator;

translator::translator()
{
}

translator::~translator()
{
}

const std::vector<language> translator::get_languages()
{
    std::cerr << "WARNING: translator::get_languages() have being called." << std::endl;
    return {};
}

const std::string translator::translate(const std::string &text, const std::string &source, const std::string &target)
{
    std::cerr << "WARNING: translator:translate(const std::string&, const std::string&, const std::string&) have being called." << std::endl;
    return {};
}
