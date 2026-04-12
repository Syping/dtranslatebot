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

#ifndef TRANSLATOR_DIALOG_H
#define TRANSLATOR_DIALOG_H

#include <dpp/json.h>
#include <gtkmm/dialog.h>
#include "user_config.h"

namespace bot {
    namespace gui {
        class translator_dialog : public Gtk::Dialog {
        public:
            explicit translator_dialog(Gtk::Window &parent, const std::string &translator, user_config &user_config);
            static void configure(Gtk::Window &parent, const std::string &translator, user_config &user_config);
            void on_save_button_clicked();

        private:
            dpp::json m_json;
            user_config& m_user_config;
        };
    }
}

#endif // TRANSLATOR_DIALOG_H
