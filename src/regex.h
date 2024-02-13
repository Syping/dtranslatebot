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

#ifndef REGEX_H
#define REGEX_H

#ifdef DTRANSLATEBOT_USE_BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif
#include <string_view>

namespace bot {
#ifdef DTRANSLATEBOT_USE_BOOST_REGEX
    using boost::regex;
    using boost::regex_match;
    using boost::match_results;
    typedef boost::match_results<std::string_view::const_iterator> svmatch;
#else
    using std::regex;
    using std::regex_match;
    using std::match_results;
    typedef std::match_results<std::string_view::const_iterator> svmatch;
#endif
}

#endif // REGEX_H
