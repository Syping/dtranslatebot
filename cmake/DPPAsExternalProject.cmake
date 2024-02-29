#[[**************************************************************************
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
****************************************************************************]]

# OpenSSL needs to build with make
find_program(MAKE_EXECUTABLE NAMES make gmake)
if (NOT MAKE_EXECUTABLE)
    message(SEND_ERROR "make not found")
endif()

find_program(NPROC_EXECUTABLE nproc)
if (NPROC_EXECUTABLE)
    execute_process(
        COMMAND ${NPROC_EXECUTABLE}
        OUTPUT_VARIABLE NPROC
    )
    set(JOBS_ARGUMENT "-j${NPROC}" CACHE STRING "make jobs argument")
endif()

include(ExternalProject)
ExternalProject_Add(ZLIB
    URL https://www.zlib.net/zlib-1.3.1.tar.xz
    URL_HASH SHA256=38ef96b8dfe510d42707d9c781877914792541133e1870841463bfa73f883e32
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DZLIB_BUILD_EXAMPLES=OFF
)
ExternalProject_Get_Property(ZLIB INSTALL_DIR)
set(ZLIB_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Add(OpenSSL
    URL https://www.openssl.org/source/openssl-3.0.13.tar.gz
    URL_HASH SHA256=88525753f79d3bec27d2fa7c66aa0b92b3aa9498dafd93d7cfa4b3780cdae313
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    CONFIGURE_COMMAND
        "<SOURCE_DIR>/config"
        "--prefix=<INSTALL_DIR>"
        "--api=1.1.1"
        no-deprecated
        no-dtls
        no-shared
        no-zlib
    BUILD_COMMAND ${MAKE_EXECUTABLE} ${JOBS_ARGUMENT}
    INSTALL_COMMAND ${MAKE_EXECUTABLE} ${JOBS_ARGUMENT} install_sw
)
ExternalProject_Get_Property(OpenSSL INSTALL_DIR)
set(OpenSSL_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Add(DPP
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
    GIT_TAG 46044c8f80134633f7b396b4d5814f83d54a96d3
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_VOICE_SUPPORT=OFF
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DDPP_NO_VCPKG=ON
        "-DOpenSSL_ROOT=${OpenSSL_INSTALL_DIR}"
        "-DZLIB_ROOT=${ZLIB_INSTALL_DIR}"
    DEPENDS OpenSSL
    DEPENDS ZLIB
)
