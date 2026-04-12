/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2026 Syping
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

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include <dpp/json.h>
#include <glibmm/keyfile.h>

namespace bot {
    namespace gui {
        class user_config {
        public:
            explicit user_config();
            ~user_config();
            static const std::string get_app_config_directory();
            static const std::string get_app_config_file();
            const std::string get_token() const;
            const std::string get_translator() const;
            void get_translator_settings(const std::string &group, dpp::json &json) const;
            const dpp::json get_translator_settings(const std::string &translator) const;
            bool save();
            void set_token(const std::string &token);
            void set_translator(const std::string &translator);
            void set_translator_settings(const dpp::json &json);
            void set_translator_settings(const std::string &group, const dpp::json &json);

        private:
            Glib::RefPtr<Glib::KeyFile> m_key_file;
            bool m_is_loaded;
            bool m_is_saved;
        };
    }
}

#endif // USER_CONFIG_H
