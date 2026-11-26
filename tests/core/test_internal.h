/*
  libj_ds permanent tests core internals
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

#ifndef __J_TEST_INTERNAL_H
#define __J_TEST_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "test.h"

#ifndef TEST_CONFIG_NAME
#define TEST_CONFIG_NAME "unknown"
#endif

#define TEST_SUITE_MAX  64
#define TEST_NAME_MAX   96
#define TEST_FILE_MAX   160

typedef struct test_case_record {
    char        suite[TEST_SUITE_MAX];
    char        name[TEST_NAME_MAX];
    char        file[TEST_FILE_MAX];
    int         line;
    test_case_fn fn;
    const void* ctx;
} test_case_record_t;

size_t test_registry_count(void);
test_case_record_t* test_registry_at(size_t index);
bool test_registry_overflow(void);

bool test_suite_selected(const char* suite);

void test_parse_args(int argc, char** argv, const char** text_filter,
                     bool* verbose, bool* list_only);
int test_run_all(int argc, char** argv);

void test_trace_dump(FILE* out, size_t limit);
size_t test_trace_limit(void);
void test_trace_set_limit(size_t limit);

#endif /* __J_TEST_INTERNAL_H */
