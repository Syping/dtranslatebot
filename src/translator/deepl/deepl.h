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

#ifndef TRANSLATOR_DEEPL_H
#define TRANSLATOR_DEEPL_H

#include "../../core/translator.h"

namespace bot {
    namespace translator {
        class deepl : public translator {
        public:
            explicit deepl(const std::string &hostname, const std::string apiKey = {});
            ~deepl() override;
            const std::vector<language> get_languages() override;
            const std::string translate(const std::string &text, const std::string &source, const std::string &target) override;

        private:
            std::string m_apiKey;
            std::string m_hostname;
            supported_languages m_languages;
        };
    }
}

#endif // TRANSLATOR_DEEPL_H
