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

if (DEFINED CMAKE_BUILD_TYPE)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()
if (DEFINED CMAKE_C_COMPILER)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
    list(APPEND CMAKE_PASSTHROUGH_ENV "CC=${CMAKE_C_COMPILER}")
elseif ($ENV{MSYSTEM} STREQUAL "CLANG64")
    list(APPEND CMAKE_PASSTHROUGH_ENV "CC=clang")
endif()
if (DEFINED CMAKE_C_COMPILER_TARGET)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_C_COMPILER_TARGET=${CMAKE_C_COMPILER_TARGET}")
endif()
if (DEFINED CMAKE_CXX_COMPILER)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
    list(APPEND CMAKE_PASSTHROUGH_ENV "CXX=${CMAKE_CXX_COMPILER}")
elseif ($ENV{MSYSTEM} STREQUAL "CLANG64")
    list(APPEND CMAKE_PASSTHROUGH_ENV "CXX=clang++")
endif()
if (DEFINED CMAKE_CXX_COMPILER_TARGET)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}")
endif()
if (DEFINED CMAKE_RC_COMPILER)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
endif()
if (DEFINED CMAKE_STRIP)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_STRIP=${CMAKE_STRIP}")
endif()
if (DEFINED CMAKE_SYSROOT)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_SYSROOT=${CMAKE_SYSROOT}")
endif()
if (DEFINED CMAKE_SYSTEM_NAME AND NOT CMAKE_SYSTEM_NAME STREQUAL CMAKE_HOST_SYSTEM_NAME)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
endif()
if (DEFINED CMAKE_SYSTEM_PROCESSOR AND NOT CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
endif()
if (DEFINED CMAKE_SYSTEM_VERSION AND NOT CMAKE_SYSTEM_VERSION STREQUAL CMAKE_HOST_SYSTEM_VERSION)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}")
endif()
if (DEFINED CMAKE_TOOLCHAIN_FILE)
    list(APPEND CMAKE_PASSTHROUGH_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()
if (DEFINED CMAKE_PASSTHROUGH_ENV)
    set(CMAKE_PASSTHROUGH_ENV
        "${CMAKE_COMMAND}" -E env ${CMAKE_PASSTHROUGH_ENV}
    )
endif()
