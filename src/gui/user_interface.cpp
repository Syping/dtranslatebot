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
#include <gtkmm/dropdown.h>
#include <gtkmm/label.h>
#include <gtkmm/passwordentry.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/textview.h>
#include <memory>
#include "../core/regex.h"
#include "translator_dialog.h"
#include "user_interface.h"
using namespace bot::gui;

const char* translators[] = {"stub", "libretranslate", "lingvatranslate", "mozhi", "deepl"};

user_interface::user_interface()
{
    set_title("dtranslatebot");
    set_default_size(500, 0);
    set_resizable(false);

    auto vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    vertical_box->set_margin(6);

    auto token_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    vertical_box->append(*token_box);

    auto token_label = Gtk::make_managed<Gtk::Label>("Discord Bot Token:");
    token_box->append(*token_label);

    m_token_entry = Gtk::make_managed<Gtk::PasswordEntry>();
    m_token_entry->set_hexpand(true);
    m_token_entry->set_show_peek_icon(true);
    m_token_entry->signal_changed().connect(sigc::mem_fun(*this, &user_interface::on_token_entry_changed));
    token_box->append(*m_token_entry);

    auto translator_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    vertical_box->append(*translator_box);

    auto translator_label = Gtk::make_managed<Gtk::Label>("Translator:");
    translator_box->append(*translator_label);

    auto translator_list = Gtk::StringList::create({"Stub", "LibreTranslate", "Lingva Translate", "Mozhi", "DeepL"});
    m_translator_dropdown = Gtk::make_managed<Gtk::DropDown>(translator_list);
    m_translator_dropdown->property_selected().signal_changed().connect(sigc::mem_fun(*this, &user_interface::on_translator_dropdown_changed));
    translator_box->append(*m_translator_dropdown);

    auto translator_spacer = Gtk::make_managed<Gtk::Box>();
    translator_spacer->set_hexpand(true);
    translator_box->append(*translator_spacer);

    m_translator_configure_button = Gtk::make_managed<Gtk::Button>("Configure...");
    m_translator_configure_button->set_size_request(80, -1);
    m_translator_configure_button->set_sensitive(false);
    m_translator_configure_button->signal_clicked().connect(sigc::mem_fun(*this, &user_interface::on_translator_configure_pressed));
    translator_box->append(*m_translator_configure_button);

    auto log_scrolledwindow = Gtk::make_managed<Gtk::ScrolledWindow>();
    log_scrolledwindow->set_size_request(-1, 300);
    log_scrolledwindow->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    vertical_box->append(*log_scrolledwindow);

    m_log_textview = Gtk::make_managed<Gtk::TextView>();
    m_log_textview->set_editable(false);
    m_log_textview->set_monospace(true);
    m_log_textview->set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
    m_log = m_log_textview->get_buffer();
    log_scrolledwindow->set_child(*m_log_textview);

    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
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

    const std::string token = m_user_config.get_token();
    m_token_entry->set_text(token);

    const std::string translator = m_user_config.get_translator();
    for (guint i = 0; i < sizeof(translators); i++) {
        if (translators[i] != translator)
            continue;
        m_translator_dropdown->set_selected(i);
        break;
    }

    m_log_callback = std::bind(&user_interface::log_append, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_bot.log_callback_add(m_log_callback);
    m_log_dispatcher.connect(sigc::mem_fun(*this, &user_interface::on_log_dispatched));

    set_child(*vertical_box);
}

void user_interface::log_append(const std::string &message, const std::string &type, bool is_error) {
    const std::lock_guard<std::mutex> guard(m_log_buffer_mutex);
    m_log_buffer.append("\n[" + type + "] " + message);
    m_log_dispatcher.emit();
}

void user_interface::log_scroll_down() {
    m_log->place_cursor(m_log->end());
    m_log_textview->scroll_to(m_log->get_insert(), 0, 1, 1);
}

void user_interface::run() {
    const std::string token = m_token_entry->get_text();
    const std::string translator = translators[m_translator_dropdown->get_selected()];
    m_user_config.set_token(token);
    m_user_config.set_translator(translator);
    m_user_config.save();
    auto settings = std::make_shared<bot::settings::settings>();
    if (settings->process({{"token", token}, {"translator", m_user_config.get_translator_settings(translator)}}, m_log_callback)) {
        try {
            m_bot.run(settings, true, false);
            m_start_button->set_sensitive(false);
            m_stop_button->set_sensitive(true);
            m_token_entry->set_sensitive(false);
            m_translator_configure_button->set_sensitive(false);
            m_translator_dropdown->set_sensitive(false);
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
    m_token_entry->set_sensitive(true);
    bool translator_configureable = m_translator_dropdown->get_selected() > 0;
    m_translator_configure_button->set_sensitive(translator_configureable);
    m_translator_dropdown->set_sensitive(true);
}

void user_interface::on_log_dispatched() {
    const std::lock_guard<std::mutex> guard(m_log_buffer_mutex);
    m_log->insert(m_log->end(), m_log->begin() != m_log->end() ? m_log_buffer : m_log_buffer.substr(1));
    m_log_buffer.clear();
    Glib::signal_idle().connect_once(sigc::mem_fun(*this, &user_interface::log_scroll_down));
}

void user_interface::on_token_entry_changed() {
    bool token_valid = bot::regex::verify_discord_bot_token(m_token_entry->get_text());
    m_start_button->set_sensitive(!m_bot.is_running() ? token_valid : false);
}

void user_interface::on_translator_configure_pressed() {
    translator_dialog::configure(*this, translators[m_translator_dropdown->get_selected()], m_user_config);
}

void user_interface::on_translator_dropdown_changed() {
    bool translator_configureable = m_translator_dropdown->get_selected() > 0;
    m_translator_configure_button->set_sensitive(translator_configureable);
}
