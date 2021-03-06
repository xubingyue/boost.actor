/******************************************************************************\
 *                                                                            *
 *           ____                  _        _        _                        *
 *          | __ )  ___   ___  ___| |_     / \   ___| |_ ___  _ __            *
 *          |  _ \ / _ \ / _ \/ __| __|   / _ \ / __| __/ _ \| '__|           *
 *          | |_) | (_) | (_) \__ \ |_ _ / ___ \ (__| || (_) | |              *
 *          |____/ \___/ \___/|___/\__(_)_/   \_\___|\__\___/|_|              *
 *                                                                            *
 *                                                                            *
 *                                                                            *
 * Copyright (C) 2011 - 2015                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef BOOST_ACTOR_CONFIG_HPP
#define BOOST_ACTOR_CONFIG_HPP

// this header must be generated by the build system (may be empty)
#include "boost/actor/detail/build_config.hpp"

// Config pararameters defined by the build system (usually CMake):
//
// BOOST_ACTOR_ENABLE_RUNTIME_CHECKS:
//   - check requirements at runtime
//
// BOOST_ACTOR_LOG_LEVEL:
//   - denotes the amount of logging, ranging from error messages only (0)
//     to complete traces (4)

/// Denotes version of CAF in the format {MAJOR}{MINOR}{PATCH},
/// whereas each number is a two-digit decimal number without
/// leading zeros (e.g. 900 is version 0.9.0).
#define BOOST_ACTOR_VERSION 1401

/// Defined to the major version number of CAF.
#define BOOST_ACTOR_MAJOR_VERSION (BOOST_ACTOR_VERSION / 10000)

/// Defined to the minor version number of CAF.
#define BOOST_ACTOR_MINOR_VERSION ((BOOST_ACTOR_VERSION / 100) % 100)

/// Defined to the patch version number of CAF.
#define BOOST_ACTOR_PATCH_VERSION (BOOST_ACTOR_VERSION % 100)

// This compiler-specific block defines:
// - BOOST_ACTOR_DEPRECATED to annotate deprecated functions
// - BOOST_ACTOR_PUSH_WARNINGS/BOOST_ACTOR_POP_WARNINGS to surround "noisy" header includes
// - BOOST_ACTOR_ANNOTATE_FALLTHROUGH to suppress warnings in switch/case statements
// - BOOST_ACTOR_COMPILER_VERSION to retrieve the compiler version in BOOST_ACTOR_VERSION format
// - One of the following:
//   + BOOST_ACTOR_CLANG
//   + BOOST_ACTOR_GCC
//   + BOOST_ACTOR_MSVC

// sets BOOST_ACTOR_DEPRECATED, BOOST_ACTOR_ANNOTATE_FALLTHROUGH,
// BOOST_ACTOR_PUSH_WARNINGS and BOOST_ACTOR_POP_WARNINGS
#if defined(__clang__)
#  define BOOST_ACTOR_CLANG
#  define BOOST_ACTOR_DEPRECATED __attribute__((__deprecated__))
#  define BOOST_ACTOR_PUSH_WARNINGS                                                    \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wall\"")                              \
    _Pragma("clang diagnostic ignored \"-Wextra\"")                            \
    _Pragma("clang diagnostic ignored \"-Werror\"")                            \
    _Pragma("clang diagnostic ignored \"-Wdeprecated\"")                       \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")         \
    _Pragma("clang diagnostic ignored \"-Wextra-semi\"")                       \
    _Pragma("clang diagnostic ignored \"-Wdocumentation\"")                    \
    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"")                     \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")                 \
    _Pragma("clang diagnostic ignored \"-Wswitch-enum\"")                      \
    _Pragma("clang diagnostic ignored \"-Wshadow\"")                           \
    _Pragma("clang diagnostic ignored \"-Wconversion\"")                       \
    _Pragma("clang diagnostic ignored \"-Wcast-align\"")                       \
    _Pragma("clang diagnostic ignored \"-Wundef\"")                            \
    _Pragma("clang diagnostic ignored \"-Wnested-anon-types\"")                \
    _Pragma("clang diagnostic ignored \"-Wdeprecated\"")                       \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")         \
    _Pragma("clang diagnostic ignored \"-Wdocumentation\"")                    \
    _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")                      \
    _Pragma("clang diagnostic ignored \"-Wimplicit-fallthrough\"")             \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"")                   \
    _Pragma("clang diagnostic ignored \"-Wshadow\"")                           \
    _Pragma("clang diagnostic ignored \"-Wshorten-64-to-32\"")                 \
    _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")                  \
    _Pragma("clang diagnostic ignored \"-Wundef\"")                            \
    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"")                     \
    _Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"")           \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")         \
    _Pragma("clang diagnostic ignored \"-Wunreachable-code\"")                 \
    _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")
#  define BOOST_ACTOR_PUSH_NON_VIRTUAL_DTOR_WARNING                                    \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wnon-virtual-dtor\"")
#  define BOOST_ACTOR_PUSH_NO_DEPRECATED_WARNING                                       \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wdeprecated\"")
#  define BOOST_ACTOR_POP_WARNINGS                                                     \
    _Pragma("clang diagnostic pop")
#  define BOOST_ACTOR_ANNOTATE_FALLTHROUGH [[clang::fallthrough]]
#  define BOOST_ACTOR_COMPILER_VERSION                                                 \
    (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(__GNUC__)
#  define BOOST_ACTOR_GCC
#  define BOOST_ACTOR_DEPRECATED __attribute__((__deprecated__))
#  define BOOST_ACTOR_PUSH_WARNINGS
#  define BOOST_ACTOR_PUSH_NON_VIRTUAL_DTOR_WARNING                                    \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wnon-virtual-dtor\"")
#  define BOOST_ACTOR_PUSH_NO_DEPRECATED_WARNING                                       \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#  define BOOST_ACTOR_POP_WARNINGS                                                     \
    _Pragma("GCC diagnostic pop")
#  define BOOST_ACTOR_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#  define BOOST_ACTOR_COMPILER_VERSION                                                 \
     (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#  define BOOST_ACTOR_MSVC
#  define BOOST_ACTOR_DEPRECATED
#  define BOOST_ACTOR_PUSH_WARNINGS
#  define BOOST_ACTOR_PUSH_NON_VIRTUAL_DTOR_WARNING
#  define BOOST_ACTOR_POP_WARNINGS
#  define BOOST_ACTOR_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#  define BOOST_ACTOR_COMPILER_VERSION _MSC_FULL_VER
#  pragma warning( disable : 4624 )
#  pragma warning( disable : 4800 )
#  pragma warning( disable : 4503 )
#  define NOMINMAX
#else
#  define BOOST_ACTOR_DEPRECATED
#  define BOOST_ACTOR_PUSH_WARNINGS
#  define BOOST_ACTOR_POP_WARNINGS
#  define BOOST_ACTOR_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#endif

// This OS-specific block defines one of the following:
// - BOOST_ACTOR_MACOS
// - BOOST_ACTOR_LINUX
// - BOOST_ACTOR_BSD
// - BOOST_ACTOR_WINDOWS
// It also defines BOOST_ACTOR_POSIX for POSIX-compatible systems
#if defined(__APPLE__)
#  include "TargetConditionals.h"
#  if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#    define BOOST_ACTOR_IOS
#  else
#    define BOOST_ACTOR_MACOS
#    ifndef _GLIBCXX_HAS_GTHREADS
#      define _GLIBCXX_HAS_GTHREADS
#    endif
#  endif
#elif defined(__ANDROID__)
#  define BOOST_ACTOR_ANDROID
#elif defined(__linux__)
#  define BOOST_ACTOR_LINUX
#  include <linux/version.h>
#  if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,16)
#    define BOOST_ACTOR_POLL_IMPL
#  endif
#elif defined(__FreeBSD__)
#  define BOOST_ACTOR_BSD
#elif defined(WIN32) || defined(_WIN32)
#  define BOOST_ACTOR_WINDOWS
#else
#  error Platform and/or compiler not supportet
#endif
#if defined(BOOST_ACTOR_MACOS) || defined(BOOST_ACTOR_LINUX) || defined(BOOST_ACTOR_BSD)
#  define BOOST_ACTOR_POSIX
#endif

#include <cstdio>
#include <cstdlib>

// Optionally enable BOOST_ACTOR_ASSERT
#ifndef BOOST_ACTOR_ENABLE_RUNTIME_CHECKS
# define BOOST_ACTOR_ASSERT(unused) static_cast<void>(0)
#elif defined(BOOST_ACTOR_WINDOWS) || defined(BOOST_ACTOR_BSD)
# define BOOST_ACTOR_ASSERT(stmt)                                                      \
  if (static_cast<bool>(stmt) == false) {                                      \
    printf("%s:%u: requirement failed '%s'\n", __FILE__, __LINE__, #stmt);     \
    abort();                                                                   \
  } static_cast<void>(0)
#else // defined(BOOST_ACTOR_LINUX) || defined(BOOST_ACTOR_MACOS)
# include <execinfo.h>
# define BOOST_ACTOR_ASSERT(stmt)                                                      \
  if (static_cast<bool>(stmt) == false) {                                      \
    printf("%s:%u: requirement failed '%s'\n", __FILE__, __LINE__, #stmt);     \
    void* array[10];                                                           \
    auto caf_bt_size = ::backtrace(array, 10);                                 \
    ::backtrace_symbols_fd(array, caf_bt_size, 2);                             \
    abort();                                                                   \
  } static_cast<void>(0)
#endif

#define BOOST_ACTOR_CRITICAL(error)                                                    \
  printf("%s:%u: critical error: '%s'\n", __FILE__, __LINE__, error);          \
  abort()

#endif // BOOST_ACTOR_CONFIG_HPP
