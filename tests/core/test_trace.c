/*
  libj_ds permanent tests bounded operation trace
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

#include <stddef.h>
#include <stdio.h>

#include "test_internal.h"

#define TEST_TRACE_MAX  256

typedef struct test_trace_entry {
    const char* op;
    long long   a;
    long long   b;
    long long   c;
} test_trace_entry_t;

static test_trace_entry_t trace_ring[TEST_TRACE_MAX];
static size_t trace_next;
static size_t trace_count;
static size_t trace_limit = 32;

void test_trace(const char* op, long long a, long long b, long long c)
{
    trace_ring[trace_next].op = op;
    trace_ring[trace_next].a = a;
    trace_ring[trace_next].b = b;
    trace_ring[trace_next].c = c;
    trace_next = (trace_next + 1) % TEST_TRACE_MAX;
    if (trace_count < TEST_TRACE_MAX)
        ++trace_count;
}

void test_trace_reset(void)
{
    trace_next = 0;
    trace_count = 0;
}

void test_trace_set_limit(size_t limit)
{
    trace_limit = limit;
}

size_t test_trace_limit(void)
{
    return trace_limit;
}

void test_trace_dump(FILE* out, size_t limit)
{
    size_t shown;
    size_t start;
    size_t i;

    if (out == NULL || trace_count == 0)
        return;
    shown = limit < trace_count ? limit : trace_count;
    if (shown == 0)
        return;

    start = (trace_next + TEST_TRACE_MAX - shown) % TEST_TRACE_MAX;
    fprintf(out, "         recent trace (last %zu):\n", shown);
    for (i = 0; i < shown; ++i) {
        const test_trace_entry_t* e = &trace_ring[(start + i) % TEST_TRACE_MAX];

        fprintf(out, "           %s(%lld, %lld, %lld)\n",
                e->op, e->a, e->b, e->c);
    }
}
