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

#include <gtkmm/alertdialog.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/editable.h>
#include <gtkmm/label.h>
#include <gtkmm/passwordentry.h>
#include <gtkmm/textview.h>
#include <memory>
#include "../core/regex.h"
#include "user_interface.h"
using namespace bot::gui;

user_interface::user_interface()
{
    set_title("dtranslatebot");
    set_default_size(500, 0);
    set_resizable(false);

    auto vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    vertical_box->set_margin(6);
    vertical_box->set_spacing(6);

    auto token_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    token_box->set_spacing(6);
    vertical_box->append(*token_box);

    auto token_label = Gtk::make_managed<Gtk::Label>("Discord Bot Token:");
    token_box->append(*token_label);

    m_token_entry = Gtk::make_managed<Gtk::PasswordEntry>();
    m_token_entry->set_show_peek_icon(true);
    m_token_entry->set_hexpand(true);
    token_box->append(*m_token_entry);

    auto log_textview = Gtk::make_managed<Gtk::TextView>();
    log_textview->set_size_request(-1, 300);
    log_textview->set_editable(false);
    log_textview->set_monospace(true);
    log_textview->set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
    m_log = log_textview->get_buffer();
    vertical_box->append(*log_textview);

    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    button_box->set_spacing(6);
    button_box->set_halign(Gtk::Align::CENTER);
    vertical_box->append(*button_box);

    m_start_button = Gtk::make_managed<Gtk::Button>("Start");
    m_start_button->set_size_request(80, -1);
    m_start_button->set_sensitive(false);
    m_start_button->signal_clicked().connect(sigc::mem_fun(*this, &user_interface::run));
    button_box->append(*m_start_button);

    m_stop_button = Gtk::make_managed<Gtk::Button>("Stop");
    m_stop_button->set_size_request(80, -1);
    m_stop_button->set_sensitive(false);
    m_stop_button->signal_clicked().connect(sigc::mem_fun(*this, &user_interface::terminate));
    button_box->append(*m_stop_button);

    m_token_entry->signal_changed().connect([=]{
        bool token_valid = bot::regex::verify_discord_bot_token(m_token_entry->get_text());
        m_start_button->set_sensitive(!m_bot.is_running() ? token_valid : false);
    });

    set_child(*vertical_box);

    m_log_callback = std::bind(&user_interface::log_append, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_bot.log_callback_add(m_log_callback);
    m_log_dispatcher.connect([=](){
        const std::lock_guard<std::mutex> guard(m_log_buffer_mutex);
        m_log->insert(m_log->end(), m_log->begin() != m_log->end() ? m_log_buffer : m_log_buffer.substr(1));
        m_log_buffer.clear();
    });
}

void user_interface::log_append(const std::string &message, const std::string &type, bool is_error) {
    const std::lock_guard<std::mutex> guard(m_log_buffer_mutex);
    m_log_buffer.append("\n[" + type + "] " + message);
    m_log_dispatcher.emit();
}

void user_interface::run() {
    auto settings = std::make_shared<bot::settings::settings>();
    if (settings->process({{"token", m_token_entry->get_text()}, {"translator", {{"type", "stub"}}}}, m_log_callback)) {
        try {
            m_bot.run(settings, true, false);
            m_start_button->set_sensitive(false);
            m_stop_button->set_sensitive(true);
        }
        catch (const std::exception &exception) {
            auto alert_dialog = Gtk::AlertDialog::create(exception.what());
            alert_dialog->set_modal(true);
            alert_dialog->show(*this);
        }
    }
    else {
        auto alert_dialog = Gtk::AlertDialog::create("Failed to process settings");
        alert_dialog->set_modal(true);
        alert_dialog->show(*this);
    }
}

void user_interface::terminate() {
    log_append("Stopping bot...", "Launch");
    m_bot.terminate();
    bool token_valid = bot::regex::verify_discord_bot_token(m_token_entry->get_text());
    m_start_button->set_sensitive(token_valid);
    m_stop_button->set_sensitive(false);
}
