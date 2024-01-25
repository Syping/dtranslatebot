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

#ifndef TRANSLATE_LIBRETRANSLATE_H
#define TRANSLATE_LIBRETRANSLATE_H

#include <cstdint>
#include <string>
#include "translate_core.h"

namespace bot {
    namespace translate {
        class libretranslate : public translator {
        public:
            explicit libretranslate(const std::string &hostname, uint16_t port, const std::string &url, bool tls, const std::string apiKey = {});
            const std::vector<bot::translate::language> get_languages();
            const std::string translate(const std::string &text, const std::string &source, const std::string &target);

        private:
            std::string m_apiKey;
            std::string m_hostname;
            uint16_t m_port;
            std::string m_url;
            bool m_tls;
        };
    }
}

#endif // TRANSLATE_LIBRETRANSLATE_H
