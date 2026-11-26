/*
  libj_ds permanent tests strong allocator
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

/*
  Pointer and integer conversions are confined to this file: a test
  allocator has to align payloads and store metadata in front of them.
*/
#include <_compiler.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#define TEST_ALLOC_MAGIC   0x6a647363616c6c6fULL
#define TEST_ALLOC_TAIL    0x7461696c2d63616eULL
#define TEST_ALLOC_TAIL_SZ sizeof(uint64_t)

#ifndef JDSC_ALIGN_DEFAULT
#define JDSC_ALIGN_DEFAULT (sizeof(long double))
#endif

#define TEST_ALLOC_ALIGN  \
    (JDSC_ALIGN_DEFAULT > sizeof(void*) ? JDSC_ALIGN_DEFAULT : sizeof(void*))

typedef struct test_alloc_hdr {
    uint64_t magic;
    size_t   size;
    size_t   align;
    void*    raw;
} test_alloc_hdr_t;

#define TEST_ALLOC_HDR_SZ \
    ((sizeof(test_alloc_hdr_t) + TEST_ALLOC_ALIGN - 1) & ~(TEST_ALLOC_ALIGN - 1))

static size_t alloc_calls;
static size_t alloc_attempts;
static size_t alloc_live_blocks;
static size_t alloc_live_bytes;
static size_t alloc_peak_blocks;
static size_t alloc_fail_at;

/* Public overrides, declared before use. */
void* _p_malloc(size_t size);
void* _p_calloc(size_t nmemb, size_t size);
void* _p_realloc(void* ptr, size_t size);
void  _p_free(void** ptr);
void* _p_aligned_malloc(size_t alignment, size_t size);
void* _p_aligned_realloc(void* ptr, size_t alignment, size_t size,
                         size_t old_size);
void  _p_aligned_free(void** ptr, size_t alignment);

static void alloc_corruption(const char* what, const void* ptr)
{
    fprintf(stderr, "Test allocator detected %s at %p.\n", what, ptr);
    abort();
}

static test_alloc_hdr_t* alloc_header(void* ptr)
{
    unsigned char* bytes = ptr;

    return (test_alloc_hdr_t*)(bytes - TEST_ALLOC_HDR_SZ);
}

static void alloc_check(const test_alloc_hdr_t* hdr, const void* ptr)
{
    uint64_t tail = 0;
    const unsigned char* bytes = ptr;

    if (hdr->magic != TEST_ALLOC_MAGIC)
        alloc_corruption("a corrupt block header", ptr);
    memcpy(&tail, bytes + hdr->size, TEST_ALLOC_TAIL_SZ);
    if (tail != TEST_ALLOC_TAIL)
        alloc_corruption("a buffer overrun", ptr);
}

static bool alloc_denied(void)
{
    ++alloc_attempts;
    return alloc_fail_at != 0 && alloc_attempts == alloc_fail_at;
}

static void* alloc_block(size_t size, size_t alignment)
{
    size_t align = alignment > TEST_ALLOC_ALIGN ? alignment : TEST_ALLOC_ALIGN;
    size_t total;
    void* raw;
    uintptr_t start;
    uintptr_t user;
    test_alloc_hdr_t* hdr;
    const uint64_t tail = TEST_ALLOC_TAIL;

    if (align == 0 || (align & (align - 1)) != 0)
        alloc_corruption("an invalid alignment request", NULL);
    if (size > SIZE_MAX - TEST_ALLOC_HDR_SZ - align - TEST_ALLOC_TAIL_SZ)
        return NULL;

    total = TEST_ALLOC_HDR_SZ + align + size + TEST_ALLOC_TAIL_SZ;
    raw = malloc(total);
    if (raw == NULL)
        return NULL;

    start = (uintptr_t)raw + TEST_ALLOC_HDR_SZ;
    user = (start + align - 1) & ~(uintptr_t)(align - 1);
    hdr = (test_alloc_hdr_t*)(user - TEST_ALLOC_HDR_SZ);
    hdr->magic = TEST_ALLOC_MAGIC;
    hdr->size = size;
    hdr->align = align;
    hdr->raw = raw;
    memcpy((unsigned char*)user + size, &tail, TEST_ALLOC_TAIL_SZ);

    ++alloc_live_blocks;
    alloc_live_bytes += size;
    if (alloc_live_blocks > alloc_peak_blocks)
        alloc_peak_blocks = alloc_live_blocks;

    return (void*)user;
}

static void alloc_release(void* ptr)
{
    test_alloc_hdr_t* hdr = alloc_header(ptr);
    size_t size;

    alloc_check(hdr, ptr);
    size = hdr->size;
    hdr->magic = 0;
    free(hdr->raw);
    --alloc_live_blocks;
    alloc_live_bytes -= size;
}

void* _p_malloc(size_t size)
{
    void* ptr;

    if (alloc_denied())
        return NULL;
    ptr = alloc_block(size, TEST_ALLOC_ALIGN);
    if (ptr != NULL)
        ++alloc_calls;
    return ptr;
}

void* _p_calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size_t total;

    if (nmemb != 0 && size > SIZE_MAX / nmemb)
        return NULL;
    total = nmemb * size;
    if (alloc_denied())
        return NULL;
    ptr = alloc_block(total, TEST_ALLOC_ALIGN);
    if (ptr == NULL)
        return NULL;
    memset(ptr, 0, total);
    ++alloc_calls;
    return ptr;
}

void* _p_realloc(void* ptr, size_t size)
{
    void* fresh;
    size_t copy;

    if (ptr == NULL)
        return _p_malloc(size);

    if (size == 0) {
        void* old = ptr;

        _p_free(&old);
        return NULL;
    }

    if (alloc_denied())
        return NULL;

    fresh = alloc_block(size, TEST_ALLOC_ALIGN);
    if (fresh == NULL)
        return NULL;

    {
        test_alloc_hdr_t* hdr = alloc_header(ptr);

        alloc_check(hdr, ptr);
        copy = hdr->size < size ? hdr->size : size;
        memcpy(fresh, ptr, copy);
    }
    alloc_release(ptr);
    ++alloc_calls;
    return fresh;
}

void _p_free(void** ptr)
{
    if (ptr == NULL || *ptr == NULL)
        return;
    alloc_release(*ptr);
    *ptr = NULL;
}

void* _p_aligned_malloc(size_t alignment, size_t size)
{
    void* ptr;

    if (alignment == 0)
        alignment = TEST_ALLOC_ALIGN;
    if (alloc_denied())
        return NULL;
    ptr = alloc_block(size, alignment);
    if (ptr != NULL)
        ++alloc_calls;
    return ptr;
}

void* _p_aligned_realloc(void* ptr, size_t alignment, size_t size,
                         size_t old_size)
{
    void* fresh;
    size_t copy;

    (void)old_size;

    if (size == 0) {
        void* old = ptr;

        if (ptr != NULL)
            _p_aligned_free(&old, alignment);
        return NULL;
    }
    if (ptr == NULL)
        return _p_aligned_malloc(alignment, size);
    if (alignment == 0)
        alignment = TEST_ALLOC_ALIGN;
    if (alloc_denied())
        return NULL;

    fresh = alloc_block(size, alignment);
    if (fresh == NULL)
        return NULL;

    {
        test_alloc_hdr_t* hdr = alloc_header(ptr);

        alloc_check(hdr, ptr);
        copy = hdr->size < size ? hdr->size : size;
        memcpy(fresh, ptr, copy);
    }
    alloc_release(ptr);
    ++alloc_calls;
    return fresh;
}

void _p_aligned_free(void** ptr, size_t alignment)
{
    (void)alignment;
    _p_free(ptr);
}

void test_alloc_reset_stats(void)
{
    if (alloc_live_blocks != 0) {
        fprintf(stderr, "Test allocator reset with %zu live blocks.\n",
                alloc_live_blocks);
        abort();
    }
    alloc_calls = 0;
    alloc_attempts = 0;
    alloc_live_bytes = 0;
    alloc_peak_blocks = 0;
    alloc_fail_at = 0;
}

size_t test_alloc_calls(void)
{
    return alloc_calls;
}

size_t test_alloc_live_blocks(void)
{
    return alloc_live_blocks;
}

size_t test_alloc_live_bytes(void)
{
    return alloc_live_bytes;
}

size_t test_alloc_peak_blocks(void)
{
    return alloc_peak_blocks;
}

void test_alloc_count_begin(void)
{
    alloc_attempts = 0;
}

size_t test_alloc_count_end(void)
{
    return alloc_attempts;
}

void test_alloc_fail_at(size_t n)
{
    alloc_fail_at = n;
    alloc_attempts = 0;
}

void test_alloc_disarm(void)
{
    alloc_fail_at = 0;
}

void test_alloc_sweep(const test_alloc_scenario_t* scenario)
{
    size_t base_blocks;
    size_t base_bytes;
    size_t max_attempts;
    size_t i;
    void* state;
    bool ok;

    if (scenario == NULL || scenario->setup == NULL
        || scenario->operate == NULL || scenario->verify == NULL
        || scenario->teardown == NULL)
        TEST_FAIL("Allocation sweep needs a complete scenario.");

    base_blocks = test_alloc_live_blocks();
    base_bytes = test_alloc_live_bytes();

    state = scenario->setup();
    if (state == NULL)
        TEST_FAIL("Allocation scenario `%s` setup returned NULL.", scenario->name);
    test_cleanup_push(scenario->teardown, state);
    test_alloc_count_begin();
    ok = scenario->operate(state);
    max_attempts = test_alloc_count_end();
    (void)ok;
    test_cleanup_pop(true);
    if (test_alloc_live_blocks() != base_blocks
        || test_alloc_live_bytes() != base_bytes)
        TEST_FAIL("Allocation scenario `%s` leaked during its baseline run.",
                  scenario->name);

    for (i = 1; i <= max_attempts; ++i) {
        state = scenario->setup();
        if (state == NULL)
            TEST_FAIL("Allocation scenario `%s` setup returned NULL.",
                      scenario->name);
        test_cleanup_push(scenario->teardown, state);
        test_alloc_fail_at(i);
        ok = scenario->operate(state);
        test_alloc_disarm();
        test_trace("alloc_sweep", (long long)i, ok ? 1 : 0, 0);
        scenario->verify(state, ok);
        test_cleanup_pop(true);
        if (test_alloc_live_blocks() != base_blocks
            || test_alloc_live_bytes() != base_bytes)
            TEST_FAIL("Allocation scenario `%s` leaked at failure position %zu.",
                      scenario->name, i);
    }

    test_alloc_disarm();
}
