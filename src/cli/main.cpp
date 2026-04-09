/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2023-2026 Syping
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

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include "../core/discord_bot.h"
#include "../core/settings.h"

void output_log(const std::string &message, const std::string &type, bool is_error) {
    auto &output = !is_error ? std::cout : std::cerr;
    output << "[" << type << "] " << message << std::endl;
}

int main(int argc, char* argv[]) {
    bool flag_wait_for_translator = false;
    std::vector<std::string> args;
    for (size_t i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--wait-for-translator"))
            flag_wait_for_translator = true;
        else
            args.push_back(argv[i]);
    }
    if (args.size() != 1) {
        std::cout << "Usage: " << argv[0] << " [--wait-for-translator] [json]" << std::endl;
        return 0;
    }

    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK) {
        std::cerr << "[Error] Failed to initialise curl" << std::endl;
        return 1;
    }

    std::cout << "[Launch] Processing configuration..." << std::endl;
    auto settings = std::make_shared<bot::settings::settings>();
    if (!settings->parse_file(args.at(0), &output_log))
        return 1;

    bot::discord_bot bot;
    bot.log_callback_add(&output_log);
    try {
        bot.run(settings, false, flag_wait_for_translator);
    }
    catch (const std::exception &exception) {
        std::cerr << "[Exception] " << exception.what() << std::endl;
        std::cerr << "[Error] Exception while starting bot" << std::endl;
    }

    curl_global_cleanup();

    return 0;
}
