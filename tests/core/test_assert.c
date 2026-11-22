/*
  libj_ds permanent tests assertions
  Copyright (C) 2026  YangJie <yangjie98765@yeah.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "test.h"

void test_assert_true(bool cond, const char* expr, const char* file, int line)
{
    if (!cond)
        test_fail_at(file, line, "Assertion `%s` failed: expected true.", expr);
}

void test_assert_false(bool cond, const char* expr, const char* file, int line)
{
    if (cond)
        test_fail_at(file, line, "Assertion `%s` failed: expected false.", expr);
}

void test_assert_eq(long long expected, long long actual,
                    const char* expr, const char* file, int line)
{
    if (expected != actual)
        test_fail_at(file, line,
                     "Assertion `%s` failed: expected %lld, got %lld.",
                     expr, expected, actual);
}

void test_assert_ne(long long a, long long b,
                    const char* expr, const char* file, int line)
{
    if (a == b)
        test_fail_at(file, line,
                     "Assertion `%s` failed: both values are %lld.", expr, a);
}

void test_assert_null(const void* ptr, const char* expr,
                      const char* file, int line)
{
    if (ptr != NULL)
        test_fail_at(file, line,
                     "Assertion `%s` failed: expected NULL, got %p.",
                     expr, ptr);
}

void test_assert_not_null(const void* ptr, const char* expr,
                          const char* file, int line)
{
    if (ptr == NULL)
        test_fail_at(file, line, "Assertion `%s` failed: got NULL.", expr);
}

void test_assert_ptr_eq(const void* expected, const void* actual,
                        const char* expr, const char* file, int line)
{
    if (expected != actual)
        test_fail_at(file, line,
                     "Assertion `%s` failed: expected %p, got %p.",
                     expr, expected, actual);
}

void test_assert_str_eq(const char* expected, const char* actual,
                        const char* expr, const char* file, int line)
{
    if (expected == NULL || actual == NULL) {
        if (expected != actual)
            test_fail_at(file, line,
                         "Assertion `%s` failed: expected %s, got %s.",
                         expr, expected != NULL ? expected : "(null)",
                         actual != NULL ? actual : "(null)");
        return;
    }
    if (strcmp(expected, actual) != 0)
        test_fail_at(file, line,
                     "Assertion `%s` failed: expected \"%s\", got \"%s\".",
                     expr, expected, actual);
}

void test_assert_mem_eq(const void* expected, const void* actual, size_t size,
                        const char* expr, const char* file, int line)
{
    const unsigned char* lhs = expected;
    const unsigned char* rhs = actual;
    size_t i;

    if (size == 0)
        return;
    if (expected == NULL || actual == NULL) {
        if (expected != actual)
            test_fail_at(file, line,
                         "Assertion `%s` failed: one buffer is NULL.", expr);
        return;
    }
    for (i = 0; i < size; ++i) {
        if (lhs[i] != rhs[i]) {
            test_fail_at(file, line,
                         "Assertion `%s` failed at byte %zu: expected 0x%02x, got 0x%02x.",
                         expr, i, (unsigned)lhs[i], (unsigned)rhs[i]);
            return;
        }
    }
}
