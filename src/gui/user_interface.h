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
#include <gtkmm/dropdown.h>
#include <gtkmm/passwordentry.h>
#include <gtkmm/textview.h>
#include <gtkmm/window.h>
#include <mutex>
#include "../core/discord_bot.h"
#include "user_config.h"

namespace bot {
    namespace gui {
        class user_interface : public Gtk::Window {
        public:
            explicit user_interface();
            static const std::vector<Glib::ustring> get_translator();
            static const char* get_translator_name(guint translator_id);
            void log_append(const std::string &message, const std::string &type = "Log", bool is_error = false);
            void log_scroll_down();
            void run();
            void terminate();
            void terminate_async();
            void on_log_dispatched();
            void on_terminate_dispatched();
            void on_token_entry_changed();
            void on_translator_configure_pressed();
            void on_translator_dropdown_changed();

        private:
            bot::discord_bot m_bot;
            Glib::RefPtr<Gtk::TextBuffer> m_log;
            std::string m_log_buffer;
            Glib::Dispatcher m_log_dispatcher;
            std::mutex m_log_buffer_mutex;
            bot::log::log_message_callback m_log_callback;
            Gtk::TextView* m_log_textview;
            Gtk::Button* m_start_button;
            Gtk::Button* m_stop_button;
            Glib::Dispatcher m_terminate_dispatcher;
            std::unique_ptr<std::thread> m_terminate_thread;
            Gtk::PasswordEntry* m_token_entry;
            Gtk::Button* m_translator_configure_button;
            Gtk::DropDown* m_translator_dropdown;
            user_config m_user_config;
        };
    }
}
