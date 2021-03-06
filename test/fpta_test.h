﻿/*
 * Copyright 2017-2018 libfpta authors: please see AUTHORS file.
 *
 * This file is part of libfpta, aka "Fast Positive Tables".
 *
 * libfpta is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfpta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfpta.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "fast_positive/tables_internal.h"

#ifdef _MSC_VER
#pragma warning(disable : 4625) /* constructor was implicitly defined          \
                                   as deleted */
#pragma warning(disable : 4626) /* assignment operator was implicitly defined  \
                                   as deleted */
#pragma warning(disable : 5026) /* move constructor was implicitly defined     \
                                   as deleted */
#pragma warning(disable : 5027) /* move assignment operator was implicitly     \
                                   defined as deleted */
#pragma warning(disable : 4820) /* 'xyz': 'N' bytes padding added after        \
                                   data member */

#pragma warning(push, 1)
#pragma warning(disable : 4530) /* C++ exception handler used, but             \
                                   unwind semantics are not enabled.           \
                                   Specify /EHsc */
#pragma warning(disable : 4577) /* 'noexcept' used with no exception           \
                                   handling mode specified; termination on     \
                                   exception is not guaranteed.                \
                                   Specify /EHsc */
#pragma warning(disable : 4738) /* storing 32-bit float result in memory,      \
                                   possible loss of performance */
#endif                          /* _MSC_VER (warnings) */

#include <gtest/gtest.h>

#ifndef GTEST_TUPLE_NAMESPACE_
#if GTEST_USE_OWN_TR1_TUPLE || GTEST_HAS_TR1_TUPLE
#define GTEST_TUPLE_NAMESPACE_ ::std::tr1
#else
#define GTEST_TUPLE_NAMESPACE_ ::std
#endif
#endif /* GTEST_TUPLE_NAMESPACE_ */

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

/* LY: reduce test runtime (significantly on Elbrus) */
#if defined(__LCC__) && defined(NDEBUG) && defined(__OPTIMIZE__) &&            \
    !defined(ENABLE_GPROF)
#undef SCOPED_TRACE
#define SCOPED_TRACE(message) __noop()
#define SCOPED_TRACE_ONLY __maybe_unused
#else
#define SCOPED_TRACE_ONLY
#endif /* __LCC__ */

//----------------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)

fptu_time fptu_now_fine_crutch(void);
#define NOW_FINE() fptu_now_fine_crutch()

int unlink_crutch(const char *pathname);
#define REMOVE_FILE(pathname) unlink_crutch(pathname)
#define TEST_DB_DIR ""

#else

#define NOW_FINE() fptu_now_fine()
#define REMOVE_FILE(pathname) unlink(pathname)
#ifdef __linux__
#define TEST_DB_DIR "/dev/shm/"
#else
#define TEST_DB_DIR "/tmp/"
#endif

#endif

//----------------------------------------------------------------------------

/* Ограничитель по времени выполнения.
 * Нужен для предотвращения таумаута тестов в CI. Предполагается, что он
 * используется вместе с установкой GTEST_SHUFFLE=1, что в сумме дает
 * выполнение части тестов в случайном порядке, пока не будет превышен лимит
 * заданный через переменную среды окружения GTEST_RUNTIME_LIMIT. */
class runtime_limiter {
  const time_t edge;

  static time_t fetch() {
    const char *GTEST_RUNTIME_LIMIT = getenv("GTEST_RUNTIME_LIMIT");
    if (GTEST_RUNTIME_LIMIT) {
      long limit = atol(GTEST_RUNTIME_LIMIT);
      if (limit > 0)
        return time(nullptr) + limit;
    }
    return 0;
  }

public:
  runtime_limiter() : edge(fetch()) {}

  bool is_timeout() {
    if (edge && time(nullptr) > edge) {
      std::cout << "[  SKIPPED ] RUNTIME_LIMIT was reached" << std::endl;
      GTEST_SUCCESS_("Skipped") << "SKIPPEND by RUNTIME_LIMIT";
      return true;
    }
    return false;
  }
};

extern runtime_limiter ci_runtime_limiter;

#define GTEST_IS_EXECUTION_TIMEOUT() ci_runtime_limiter.is_timeout()
