#[[**************************************************************************
* dtranslatebot Discord Translate Bot
* Copyright (C) 2023-2024 Syping
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
****************************************************************************]]

cmake_minimum_required(VERSION 3.16)
project(dtranslatebot VERSION 0.1 LANGUAGES CXX)
include(GNUInstallDirs)

# dtranslatebot Source files
set(DTRANSLATEBOT_HEADERS
    src/message_queue.h
    src/settings.h
    src/submit_queue.h
    src/webhook_push.h
)
set(DTRANSLATEBOT_SOURCES
    src/main.cpp
    src/message_queue.cpp
    src/settings.cpp
    src/submit_queue.cpp
    src/webhook_push.cpp
)

# dtranslatebot Module Path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# D++ Discord API Library for Bots
find_package(DPP REQUIRED)

# pthread Support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# dtranslatebot Target + Installs
add_executable(dtranslatebot ${DTRANSLATEBOT_HEADERS} ${DTRANSLATEBOT_SOURCES})
target_link_libraries(dtranslatebot Threads::Threads ${DPP_LIBRARIES})
target_include_directories(dtranslatebot PRIVATE ${DPP_INCLUDE_DIR})
set_target_properties(dtranslatebot PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
install(TARGETS dtranslatebot DESTINATION "${CMAKE_INSTALL_BINDIR}")
