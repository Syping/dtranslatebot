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

#include <giomm/file.h>
#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include "user_config.h"
using namespace bot::gui;

user_config::user_config() {
    m_key_file = Glib::KeyFile::create();
    try {
        m_key_file->load_from_file(get_app_config_file());
        m_is_loaded = true;
    }
    catch (const Glib::FileError &exception) {
        m_is_loaded = false;
    }
    catch (const Glib::KeyFileError &exception) {
        m_is_loaded = false;
    }
    m_is_saved = true;
}

user_config::~user_config() {
    if (!m_is_saved)
        save();
}

const std::string user_config::get_app_config_directory() {
    return Glib::build_filename(Glib::get_user_config_dir(), "dtranslatebot");
}

const std::string user_config::get_app_config_file() {
    return Glib::build_filename(get_app_config_directory(), "dtranslatebot.conf");
}

const std::string user_config::get_token() const {
    if (!m_key_file->has_group("Discord"))
        return {};
    if (!m_key_file->has_key("Discord", "Token"))
        return {};
    return m_key_file->get_string("Discord", "Token");
}

const std::string user_config::get_translator() const {
    if (!m_key_file->has_group("Translator"))
        return "stub";
    if (!m_key_file->has_key("Translator", "Type"))
        return "stub";
    return m_key_file->get_string("Translator", "Type");
}

void user_config::get_translator_settings(const std::string &group, dpp::json &json) const {
    if (m_key_file->has_key(group, "Hostname"))
        json["hostname"] = m_key_file->get_string(group, "Hostname");
    if (m_key_file->has_key(group, "TLS"))
        json["tls"] = m_key_file->get_boolean(group, "TLS");
    if (m_key_file->has_key(group, "Port"))
        json["port"] = m_key_file->get_integer(group, "Port");
    if (m_key_file->has_key(group, "URL"))
        json["url"] = m_key_file->get_string(group, "URL");
    if (m_key_file->has_key(group, "ApiKey"))
        json["apiKey"] = m_key_file->get_string(group, "ApiKey");
}

const dpp::json user_config::get_translator_settings(const std::string &translator) const {
    dpp::json json = {
        {"type", translator}
    };
    if (translator == "deepl") {
        if (!m_key_file->has_group("DeepL"))
            return json;
        if (m_key_file->has_key("DeepL", "Hostname"))
            json["hostname"] = m_key_file->get_string("DeepL", "Hostname");
        if (m_key_file->has_key("DeepL", "ApiKey"))
            json["apiKey"] = m_key_file->get_string("DeepL", "ApiKey");
    }
    else if (translator == "mozhi") {
        if (!m_key_file->has_group("Mozhi"))
            return json;
        get_translator_settings("Mozhi", json);
        if (m_key_file->has_key("Mozhi", "Engine"))
            json["engine"] = m_key_file->get_string("Mozhi", "Engine");
    }
    else if (translator == "libretranslate") {
        if (!m_key_file->has_group("LibreTranslate"))
            return json;
        get_translator_settings("LibreTranslate", json);
    }
    else if (translator == "lingvatranslate") {
        if (!m_key_file->has_group("LingvaTranslate"))
            return json;
        get_translator_settings("LingvaTranslate", json);
    }
    return json;
}

bool user_config::save() {
    auto app_config_directory = Gio::File::create_for_path(get_app_config_directory());
    if (!app_config_directory->query_exists() && !app_config_directory->make_directory_with_parents())
        return false;
    if (!m_key_file->save_to_file(get_app_config_file()))
        return false;
    m_is_saved = true;
    return true;
}

void user_config::set_token(const std::string &token) {
    m_key_file->set_string("Discord", "Token", token);
    m_is_loaded = false;
    m_is_saved = false;
}

void user_config::set_translator(const std::string &translator) {
    m_key_file->set_string("Translator", "Type", translator);
    m_is_loaded = false;
    m_is_saved = false;
}

void user_config::set_translator_settings(const dpp::json &json) {
    if (!json.is_object())
        return;
    auto json_type = json.find("type");
    if (json_type == json.end())
        return;
    const std::string translator = *json_type;
    if (translator == "deepl") {
        auto json_deepl_hostname = json.find("hostname");
        if (json_deepl_hostname != json.end())
            m_key_file->set_string("DeepL", "Hostname", static_cast<const std::string>(*json_deepl_hostname));
        auto json_deepl_apiKey = json.find("apiKey");
        if (json_deepl_apiKey != json.end())
            m_key_file->set_string("DeepL", "ApiKey", static_cast<const std::string>(*json_deepl_apiKey));
    }
    else if (translator == "mozhi") {
        set_translator_settings("Mozhi", json);
        auto json_mozhi_engine = json.find("engine");
        if (json_mozhi_engine != json.end())
            m_key_file->set_string("Mozhi", "Engine", static_cast<const std::string>(*json_mozhi_engine));
    }
    else if (translator == "libretranslate") {
        set_translator_settings("LibreTranslate", json);
    }
    else if (translator == "lingvatranslate") {
        set_translator_settings("LingvaTranslate", json);
    }
    m_is_loaded = false;
    m_is_saved = false;
}

void user_config::set_translator_settings(const std::string &group, const dpp::json &json) {
    auto json_hostname = json.find("hostname");
    if (json_hostname != json.end())
        m_key_file->set_string(group, "Hostname", static_cast<const std::string>(*json_hostname));
    auto json_tls = json.find("tls");
    if (json_tls != json.end())
        m_key_file->set_boolean(group, "TLS", *json_tls);
    auto json_port = json.find("port");
    if (json_port != json.end())
        m_key_file->set_integer(group, "Port", *json_port);
    auto json_url = json.find("url");
    if (json_url != json.end())
        m_key_file->set_string(group, "URL", static_cast<const std::string>(*json_hostname));
    auto json_apiKey = json.find("apiKey");
    if (json_url != json.end())
        m_key_file->set_string(group, "ApiKey", static_cast<const std::string>(*json_apiKey));
}
