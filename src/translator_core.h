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

#ifndef TRANSLATOR_CORE_H
#define TRANSLATOR_CORE_H

#include <string>
#include <vector>

namespace bot {
    namespace translator {
        struct language {
            std::string code;
            std::string name;
        };

        class translator {
        public:
            explicit translator();
            virtual ~translator();
            virtual const std::vector<language> get_languages();
            virtual const std::string translate(const std::string &text, const std::string &source, const std::string &target);
        };
    }
}

#endif // TRANSLATOR_CORE_H
