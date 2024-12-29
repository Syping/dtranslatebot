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
cmake_policy(VERSION 3.16...3.27)
project(dtranslatebot VERSION 0.2.0 LANGUAGES CXX)
include(GNUInstallDirs)

# dtranslatebot Source files
set(DTRANSLATEBOT_HEADERS
    src/core/database.h
    src/core/message_queue.h
    src/core/regex.h
    src/core/settings.h
    src/core/settings_types.h
    src/core/slashcommands.h
    src/core/submit_queue.h
    src/core/translator.h
    src/core/webhook_push.h
    src/database/file/file.h
    src/translator/deepl/deepl.h
    src/translator/mozhi/mozhi.h
    src/translator/libretranslate/libretranslate.h
    src/translator/lingvatranslate/lingvatranslate.h
    src/translator/stub/stub.h
)
set(DTRANSLATEBOT_SOURCES
    src/core/database.cpp
    src/core/main.cpp
    src/core/message_queue.cpp
    src/core/settings.cpp
    src/core/slashcommands.cpp
    src/core/submit_queue.cpp
    src/core/translator.cpp
    src/core/webhook_push.cpp
    src/database/file/file.cpp
    src/translator/deepl/deepl.cpp
    src/translator/mozhi/mozhi.cpp
    src/translator/libretranslate/libretranslate.cpp
    src/translator/lingvatranslate/lingvatranslate.cpp
    src/translator/stub/stub.cpp
)

# dtranslatebot Module Path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Boost C++ Libraries
option(WITH_BOOST "Build with Boost C++ Libraries" OFF)
if (WITH_BOOST)
    find_package(Boost COMPONENTS regex)
    if (Boost_regex_FOUND)
        list(APPEND DTRANSLATEBOT_LIBRARIES
            Boost::regex
        )
        set(DTRANSLATEBOT_USE_BOOST_REGEX TRUE)
    endif()
endif()

# D++ Discord API Library for Bots
option(WITH_DPP_STATIC_BUNDLE "Build with DPP Static Bundle" OFF)
if (WITH_DPP_STATIC_BUNDLE)
    include(DPPStaticBundle)
else()
    find_package(DPP REQUIRED)
endif()

# pthread Support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# dtranslatebot Win32 Shared Resources
if (WIN32)
    configure_file(src/resources/win32/dtranslatebot.rc.in "${dtranslatebot_BINARY_DIR}/resources/win32/dtranslatebot.rc" @ONLY)
    list(APPEND DTRANSLATEBOT_RESOURCES
        "${dtranslatebot_BINARY_DIR}/resources/win32/dtranslatebot.rc"
    )
endif()

# dtranslatebot systemd Service
if (UNIX AND NOT APPLE)
    option(WITH_SYSTEMD "Build with systemd Support" OFF)
    if (WITH_SYSTEMD)
        find_program(SYSTEMD_ESCAPE_EXECUTABLE NAMES systemd-escape)
        if (DEFINED SYSTEMD_ESCAPE_EXECUTABLE)
            execute_process(
                COMMAND "${SYSTEMD_ESCAPE_EXECUTABLE}" "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/dtranslatebot"
                OUTPUT_VARIABLE dtranslatebot_SERVICE_WORKDIR
            )
            string(STRIP "${dtranslatebot_SERVICE_WORKDIR}" dtranslatebot_SERVICE_WORKDIR)
        else()
            set(dtranslatebot_SERVICE_WORKDIR "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/dtranslatebot")
        endif()
        configure_file(src/systemd/dtranslatebot.service.in "${dtranslatebot_BINARY_DIR}/systemd/service/dtranslatebot.service" @ONLY)
        configure_file(src/systemd/dtranslatebot.sysusersd.in "${dtranslatebot_BINARY_DIR}/systemd/sysusers.d/dtranslatebot.conf" @ONLY)
        install(FILES "${dtranslatebot_BINARY_DIR}/systemd/service/dtranslatebot.service" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/system")
        install(FILES "${dtranslatebot_BINARY_DIR}/systemd/sysusers.d/dtranslatebot.conf" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/sysusers.d")
    endif()
endif()

# dtranslatebot Target + Installs
add_executable(dtranslatebot ${DTRANSLATEBOT_HEADERS} ${DTRANSLATEBOT_SOURCES} ${DTRANSLATEBOT_RESOURCES})
if (WITH_DPP_STATIC_BUNDLE)
    add_dependencies(dtranslatebot DPP)
endif()
target_compile_definitions(dtranslatebot PRIVATE
    ${DPP_DEFINITIONS}
    $<$<BOOL:${DTRANSLATEBOT_USE_BOOST_REGEX}>:DTRANSLATEBOT_USE_BOOST_REGEX>
)
if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914)
    target_compile_options(dtranslatebot PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>)
endif()
target_link_libraries(dtranslatebot PRIVATE ${DTRANSLATEBOT_LIBRARIES} ${DPP_LIBRARIES} Threads::Threads)
target_include_directories(dtranslatebot PRIVATE ${DPP_INCLUDE_DIR})
set_target_properties(dtranslatebot PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
install(TARGETS dtranslatebot DESTINATION "${CMAKE_INSTALL_BINDIR}")
