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

include(ArgumentPassthrough)

# OpenSSL needs to be configured with perl and build with make
find_program(MAKE_EXECUTABLE NAMES make gmake)
if (NOT DEFINED MAKE_EXECUTABLE)
    message(SEND_ERROR "make not found")
endif()

find_program(NPROC_EXECUTABLE nproc)
if (DEFINED NPROC_EXECUTABLE)
    execute_process(
        COMMAND "${NPROC_EXECUTABLE}"
        OUTPUT_VARIABLE NPROC
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(MAKE_JOBS_ARG "-j${NPROC}")
endif()

find_program(PERL_EXECUTABLE NAMES perl)
if (NOT DEFINED PERL_EXECUTABLE)
    message(SEND_ERROR "perl not found")
endif()

include(ExternalProject)
ExternalProject_Add(ZLIB
    URL https://www.zlib.net/zlib-1.3.1.tar.xz
    URL_HASH SHA256=38ef96b8dfe510d42707d9c781877914792541133e1870841463bfa73f883e32
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        ${CMAKE_PASSTHROUGH_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DZLIB_BUILD_EXAMPLES=OFF
        ${ZLIB_CONFIGURE_ARGS}
)
ExternalProject_Get_Property(ZLIB INSTALL_DIR)
set(ZLIB_INSTALL_DIR "${INSTALL_DIR}")

set(OPENSSL_PLATFORM_ARG $<$<BOOL:$ENV{MSYSTEM}>:mingw64>)
ExternalProject_Add(OpenSSL
    URL https://github.com/openssl/openssl/releases/download/openssl-3.0.16/openssl-3.0.16.tar.gz
    URL_HASH SHA256=57e03c50feab5d31b152af2b764f10379aecd8ee92f16c985983ce4a99f7ef86
    CONFIGURE_COMMAND
        ${CMAKE_PASSTHROUGH_ENV}
        "${PERL_EXECUTABLE}"
        "<SOURCE_DIR>/Configure"
        "--prefix=<INSTALL_DIR>"
        $<$<CONFIG:Debug>:-d>
        no-deprecated
        no-dso
        no-dtls
        no-engine
        no-shared
        no-zlib
        ${OPENSSL_PLATFORM_ARG}
        ${OPENSSL_CONFIGURE_ARGS}
    BUILD_COMMAND "${MAKE_EXECUTABLE}" ${MAKE_JOBS_ARG} build_libs
    INSTALL_COMMAND "${MAKE_EXECUTABLE}" ${MAKE_JOBS_ARG} install_dev
)
ExternalProject_Get_Property(OpenSSL INSTALL_DIR)
set(OpenSSL_INSTALL_DIR "${INSTALL_DIR}")

ExternalProject_Add(DPP
    URL https://github.com/brainboxdotcc/DPP/releases/download/v10.1.2/DPP-10.1.2.tar.gz
    URL_HASH SHA256=587ef044775e6bdd560ec17afc302c1048ebb3454455116d7241431fbb16a823
    CMAKE_ARGS
        -DAVX_TYPE=AVX0
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_VOICE_SUPPORT=OFF
        ${CMAKE_PASSTHROUGH_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DDPP_BUILD_TEST=OFF
        -DDPP_NO_VCPKG=ON
        -DRUN_LDCONFIG=OFF
        "-DOpenSSL_ROOT=${OpenSSL_INSTALL_DIR}"
        "-DZLIB_ROOT=${ZLIB_INSTALL_DIR}"
        ${DPP_CONFIGURE_ARGS}
    DEPENDS OpenSSL ZLIB
)
ExternalProject_Get_Property(DPP INSTALL_DIR)
set(DPP_INSTALL_DIR "${INSTALL_DIR}")
set(DPP_INCLUDE_DIR "${DPP_INSTALL_DIR}/include")
set(DPP_LIBRARIES
    -Wl,-Bstatic
    "-L${DPP_INSTALL_DIR}/lib"
    "-L${DPP_INSTALL_DIR}/lib64"
    dpp
    "-L${OpenSSL_INSTALL_DIR}/lib"
    "-L${OpenSSL_INSTALL_DIR}/lib64"
    ssl
    crypto
    "-L${ZLIB_INSTALL_DIR}/lib"
    "-L${ZLIB_INSTALL_DIR}/lib64"
    $<IF:$<BOOL:${WIN32}>,zlibstatic,z>
    -Wl,-Bdynamic
)
if (WIN32)
    set(DPP_DEFINITIONS DPP_STATIC)
    list(APPEND DPP_LIBRARIES
        ws2_32
    )
endif()
