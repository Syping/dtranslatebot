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

#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include "translator_dialog.h"
using namespace bot::gui;

translator_dialog::translator_dialog(Gtk::Window &parent, const std::string &translator, user_config &user_config) :
    Gtk::Dialog("dtranslatebot - Translator", parent, true), m_user_config(user_config) {
    m_json = m_user_config.get_translator_settings(translator);

    set_default_size(300, 0);
    set_resizable(false);

    auto vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    vertical_box->set_margin(6);

    auto hostname_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    vertical_box->append(*hostname_box);

    auto hostname_label = Gtk::make_managed<Gtk::Label>("Hostname");
    hostname_box->append(*hostname_label);

    auto hostname_entry = Gtk::make_managed<Gtk::Entry>();
    hostname_entry->set_hexpand(true);
    auto json_hostname = m_json.find("hostname");
    if (json_hostname != m_json.end())
        hostname_entry->set_text(static_cast<const std::string>(*json_hostname));
    hostname_entry->signal_changed().connect([=]() {
        m_json["hostname"] = hostname_entry->get_text();
    });
    hostname_box->append(*hostname_entry);

    if (translator != "deepl") {
        auto url_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        vertical_box->append(*url_box);

        auto url_label = Gtk::make_managed<Gtk::Label>("URL");
        url_box->append(*url_label);

        auto url_entry = Gtk::make_managed<Gtk::Entry>();
        url_entry->set_hexpand(true);
        auto json_url = m_json.find("url");
        if (json_url != m_json.end())
            url_entry->set_text(static_cast<const std::string>(*json_url));
        url_entry->signal_changed().connect([=]() {
            m_json["url"] = url_entry->get_text();
        });
        url_box->append(*url_entry);

        auto port_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        vertical_box->append(*port_box);

        auto port_label = Gtk::make_managed<Gtk::Label>("Port");
        port_box->append(*port_label);

        auto port_entry = Gtk::make_managed<Gtk::Entry>();
        port_entry->set_hexpand(true);
        auto json_port = m_json.find("port");
        if (json_port != m_json.end())
            port_entry->set_text(std::to_string(static_cast<int>(*json_port)));
        else
            port_entry->set_text("443");
        port_entry->signal_changed().connect([=]() {
            try {
                m_json["port"] = std::stoi(port_entry->get_text());
            }
            catch (const std::exception &exception) {
                // TODO: Enforce Entry being Number only
            }
        });
        port_box->append(*port_entry);

        auto tls_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        vertical_box->append(*tls_box);

        auto tls_label = Gtk::make_managed<Gtk::Label>("TLS");
        tls_box->append(*tls_label);

        auto tls_checkbutton = Gtk::make_managed<Gtk::CheckButton>("Enabled");
        auto json_tls = m_json.find("tls");
        if (json_tls != m_json.end())
            tls_checkbutton->set_active(*json_tls);
        else
            tls_checkbutton->set_active(true);
        tls_checkbutton->signal_toggled().connect([=]() {
            m_json["tls"] = tls_checkbutton->get_active();
            if (tls_checkbutton->get_active() && port_entry->get_text() == "80")
                port_entry->set_text("443");
            else if (!tls_checkbutton->get_active() && port_entry->get_text() == "443")
                port_entry->set_text("80");
        });
        tls_box->append(*tls_checkbutton);
    }

    if (translator == "mozhi") {
        auto engine_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        vertical_box->append(*engine_box);

        auto engine_label = Gtk::make_managed<Gtk::Label>("Engine");
        engine_box->append(*engine_label);

        auto engine_entry = Gtk::make_managed<Gtk::Entry>();
        engine_entry->set_hexpand(true);
        auto json_engine = m_json.find("engine");
        if (json_engine != m_json.end())
            engine_entry->set_text(static_cast<const std::string>(*json_engine));
        engine_entry->signal_changed().connect([=]() {
            m_json["engine"] = engine_entry->get_text();
        });
        engine_box->append(*engine_entry);
    }

    if (translator != "lingvatranslate" && translator != "mozhi") {
        auto apikey_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        vertical_box->append(*apikey_box);

        auto apikey_label = Gtk::make_managed<Gtk::Label>("API Key");
        apikey_box->append(*apikey_label);

        auto apikey_entry = Gtk::make_managed<Gtk::Entry>();
        apikey_entry->set_hexpand(true);
        auto json_apiKey = m_json.find("apiKey");
        if (json_apiKey != m_json.end())
            apikey_entry->set_text(static_cast<const std::string>(*json_apiKey));
        apikey_entry->signal_changed().connect([=]() {
            m_json["apiKey"] = apikey_entry->get_text();
        });
        apikey_box->append(*apikey_entry);
    }

    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    button_box->set_halign(Gtk::Align::CENTER);
    vertical_box->append(*button_box);

    auto save_button = Gtk::make_managed<Gtk::Button>("Save");
    save_button->set_size_request(80, -1);
    save_button->signal_clicked().connect(sigc::mem_fun(*this, &translator_dialog::on_save_button_clicked));
    button_box->append(*save_button);

    auto close_button = Gtk::make_managed<Gtk::Button>("Close");
    close_button->set_size_request(80, -1);
    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Dialog::close));
    button_box->append(*close_button);

    set_child(*vertical_box);
}

void translator_dialog::configure(Gtk::Window &parent, const std::string &translator, user_config &user_config) {
    auto dialog = Gtk::make_managed<translator_dialog>(parent, translator, user_config);
    dialog->set_visible();
}

void translator_dialog::on_save_button_clicked() {
    m_user_config.set_translator_settings(m_json);
    m_user_config.save();
    close();
}
