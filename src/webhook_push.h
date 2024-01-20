/*****************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2024 Syping
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

#ifndef WEBHOOK_PUSH_H
#define WEBHOOK_PUSH_H

#include <dpp/dpp.h>
#include "submit_queue.h"

namespace bot {
    class webhook_push {
    public:
        static void run(const dpp::webhook &webhook, const bot::translated_message &message, dpp::cluster *bot);
    };
}

#endif // WEBHOOK_PUSH_H
