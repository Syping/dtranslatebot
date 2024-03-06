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
find_package(Perl REQUIRED)
find_program(MAKE_EXECUTABLE NAMES make gmake)
if (NOT DEFINED MAKE_EXECUTABLE)
    message(SEND_ERROR "make not found")
endif()

find_program(NPROC_EXECUTABLE nproc)
if (DEFINED NPROC_EXECUTABLE)
    execute_process(
        COMMAND "${NPROC_EXECUTABLE}"
        OUTPUT_VARIABLE NPROC
    )
    string(STRIP "${NPROC}" NPROC)
    set(JOBS_ARGUMENT "-j${NPROC}" CACHE INTERNAL "make jobs argument")
endif()

include(ExternalProject)
ExternalProject_Add(ZLIB
    URL https://www.zlib.net/zlib-1.3.1.tar.xz
    URL_HASH SHA256=38ef96b8dfe510d42707d9c781877914792541133e1870841463bfa73f883e32
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        "${CMAKE_PASSTHROUGH_ARGS}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DZLIB_BUILD_EXAMPLES=OFF
)
ExternalProject_Get_Property(ZLIB INSTALL_DIR)
set(ZLIB_INSTALL_DIR "${INSTALL_DIR}")

ExternalProject_Add(OpenSSL
    URL https://www.openssl.org/source/openssl-3.0.13.tar.gz
    URL_HASH SHA256=88525753f79d3bec27d2fa7c66aa0b92b3aa9498dafd93d7cfa4b3780cdae313
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
    BUILD_COMMAND "${MAKE_EXECUTABLE}" "${JOBS_ARGUMENT}"
    INSTALL_COMMAND "${MAKE_EXECUTABLE}" "${JOBS_ARGUMENT}" install_sw
)
ExternalProject_Get_Property(OpenSSL INSTALL_DIR)
set(OpenSSL_INSTALL_DIR "${INSTALL_DIR}")

ExternalProject_Add(DPP
    URL https://github.com/Syping/DPP/archive/dcd00001dfa145a03f80a0ad5317bc3e63252ead.tar.gz
    URL_HASH SHA256=42ff04b13a384591e819c100c4f40e12e31c8076769c168631eef85a8eeb0be3
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_VOICE_SUPPORT=OFF
        "${CMAKE_PASSTHROUGH_ARGS}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        -DDPP_BUILD_TEST=OFF
        -DDPP_NO_VCPKG=ON
        -DRUN_LDCONFIG=OFF
        "-DOpenSSL_ROOT=${OpenSSL_INSTALL_DIR}"
        "-DZLIB_ROOT=${ZLIB_INSTALL_DIR}"
    DEPENDS OpenSSL
    DEPENDS ZLIB
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
    $<IF:$<BOOL:WIN32>,zlibstatic,z>
    -Wl,-Bdynamic
)
if (WIN32)
    set(DPP_DEFINITIONS DPP_STATIC)
    list(APPEND DPP_LIBRARIES
        ws2_32
        gdi32
        crypt32
    )
endif()
