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

#include <glibmm/dispatcher.h>
#include <gtkmm/button.h>
#include <gtkmm/passwordentry.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/window.h>
#include <mutex>
#include "../core/discord_bot.h"

namespace bot {
    namespace gui {
        class user_interface : public Gtk::Window {
        public:
            explicit user_interface();
            void log_append(const std::string &message, const std::string &type = "Log", bool is_error = false);
            void run();
            void terminate();

        private:
            bot::discord_bot m_bot;
            Glib::RefPtr<Gtk::TextBuffer> m_log;
            Gtk::Button* m_start_button;
            Gtk::Button* m_stop_button;
            Gtk::PasswordEntry* m_token_entry;
            std::string m_log_buffer;
            Glib::Dispatcher m_log_dispatcher;
            std::mutex m_log_buffer_mutex;
            bot::log::log_message_callback m_log_callback;
        };
    }
}
