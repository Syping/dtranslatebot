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

#include "curl_exception.h"
using namespace bot::exception;

curl_exception::curl_exception(const std::string &message, CURLcode error_code) : m_message(message), m_error_code(error_code) {
}

curl_exception::~curl_exception() noexcept {
}

const char* curl_exception::what() const noexcept {
    return m_message.c_str();
}
