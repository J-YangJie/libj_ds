/*
  libj_ds permanent tests case registry and runner
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

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "test_internal.h"

#define TEST_REGISTRY_MAX  4096
#define TEST_CLEANUP_MAX   256
#define TEST_MESSAGE_MAX   1024

static test_case_record_t registry[TEST_REGISTRY_MAX];
static size_t registry_count;
static bool registry_overflowed;

static struct {
    jmp_buf jb;
    bool    active;
    bool    failed;
    bool    skipped;
    char    message[TEST_MESSAGE_MAX];
    char    file[TEST_FILE_MAX];
    int     line;
} run_state;

static struct {
    void (*fn)(void*);
    void* arg;
} cleanup_stack[TEST_CLEANUP_MAX];
static size_t cleanup_top;

void test_register_case(const char* suite, const char* name,
                        test_case_fn fn, const void* ctx,
                        const char* file, int line)
{
    test_case_record_t* rec;

    if (registry_count >= TEST_REGISTRY_MAX) {
        registry_overflowed = true;
        return;
    }

    rec = &registry[registry_count++];
    snprintf(rec->suite, sizeof(rec->suite), "%s", suite);
    snprintf(rec->name, sizeof(rec->name), "%s", name);
    snprintf(rec->file, sizeof(rec->file), "%s", file);
    rec->line = line;
    rec->fn = fn;
    rec->ctx = ctx;
}

size_t test_registry_count(void)
{
    return registry_count;
}

test_case_record_t* test_registry_at(size_t index)
{
    return &registry[index];
}

bool test_registry_overflow(void)
{
    return registry_overflowed;
}

void test_cleanup_push(void (*fn)(void* arg), void* arg)
{
    if (cleanup_top >= TEST_CLEANUP_MAX) {
        test_fail_at(__FILE__, __LINE__, "The cleanup stack is full.");
        return;
    }
    cleanup_stack[cleanup_top].fn = fn;
    cleanup_stack[cleanup_top].arg = arg;
    ++cleanup_top;
}

void test_cleanup_pop(bool run)
{
    if (cleanup_top == 0) {
        test_fail_at(__FILE__, __LINE__, "The cleanup stack is empty.");
        return;
    }
    --cleanup_top;
    if (run)
        cleanup_stack[cleanup_top].fn(cleanup_stack[cleanup_top].arg);
}

static void run_cleanups(void)
{
    test_alloc_disarm();
    while (cleanup_top > 0) {
        --cleanup_top;
        cleanup_stack[cleanup_top].fn(cleanup_stack[cleanup_top].arg);
    }
}

void test_fail_at(const char* file, int line, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(run_state.message, sizeof(run_state.message), fmt, ap);
    va_end(ap);

    snprintf(run_state.file, sizeof(run_state.file), "%s", file);
    run_state.line = line;
    run_state.failed = true;
    run_state.skipped = false;

    if (!run_state.active) {
        fprintf(stderr, "fatal: %s:%d: %s\n", file, line, run_state.message);
        exit(2);
    }
    longjmp(run_state.jb, 1);
}

void test_skip_at(const char* file, int line, const char* fmt, ...)
{
    va_list ap;

    (void)file;
    (void)line;
    va_start(ap, fmt);
    vsnprintf(run_state.message, sizeof(run_state.message), fmt, ap);
    va_end(ap);

    run_state.failed = false;
    run_state.skipped = true;

    if (!run_state.active) {
        fprintf(stderr, "skip: %s\n", run_state.message);
        exit(0);
    }
    longjmp(run_state.jb, 2);
}

static int compare_records(const void* a, const void* b)
{
    const test_case_record_t* ra = a;
    const test_case_record_t* rb = b;
    int rc = strcmp(ra->suite, rb->suite);

    if (rc != 0)
        return rc;
    rc = strcmp(ra->name, rb->name);
    if (rc != 0)
        return rc;
    return strcmp(ra->file, rb->file);
}

static uint64_t fnv1a(const char* text)
{
    uint64_t hash = 0xcbf29ce484222325ULL;

    while (*text != '\0') {
        hash ^= (uint64_t)(unsigned char)*text++;
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

static uint64_t splitmix64(uint64_t* state)
{
    uint64_t z = (*state += 0x9e3779b97f4a7c15ULL);

    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

static double monotonic_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

static void print_trace_on_failure(void)
{
    test_trace_dump(stderr, test_trace_limit());
}

static bool case_selected(const test_case_record_t* rec, const char* text_filter)
{
    if (!test_suite_selected(rec->suite))
        return false;
    if (text_filter != NULL && text_filter[0] != '\0'
        && strstr(rec->name, text_filter) == NULL
        && strstr(rec->suite, text_filter) == NULL)
        return false;
    return true;
}

static void run_one(const test_case_record_t* rec, bool verbose,
                    size_t* passed, size_t* failed, size_t* skipped)
{
    uint64_t case_seed = fnv1a(rec->suite) ^ (fnv1a(rec->name) << 1);
    uint64_t mixed = test_global_seed ^ case_seed;
    size_t base_blocks = test_alloc_live_blocks();
    size_t base_bytes = test_alloc_live_bytes();
    size_t live_blocks;
    size_t live_bytes;
    int jump_code;
    double start;

    case_seed = splitmix64(&mixed);

    test_trace_reset();
    cleanup_top = 0;
    run_state.failed = false;
    run_state.skipped = false;
    run_state.message[0] = '\0';
    run_state.file[0] = '\0';
    run_state.line = 0;

    test_rng_init(case_seed);
    test_alloc_disarm();
    start = monotonic_ms();

    if (verbose)
        fprintf(stdout, "[ RUN  ] %s.%s (seed 0x%016llx)\n",
                rec->suite, rec->name, (unsigned long long)case_seed);

    jump_code = setjmp(run_state.jb);
    if (jump_code == 0) {
        run_state.active = true;
        rec->fn(rec->ctx);
        run_state.active = false;
    } else {
        run_state.active = false;
    }

    run_cleanups();

    live_blocks = test_alloc_live_blocks();
    live_bytes = test_alloc_live_bytes();
    if (!run_state.skipped && live_blocks != base_blocks) {
        if (!run_state.failed) {
            snprintf(run_state.message, sizeof(run_state.message),
                     "The case leaked %zu allocation blocks.",
                     live_blocks > base_blocks ? live_blocks - base_blocks
                                               : base_blocks - live_blocks);
            snprintf(run_state.file, sizeof(run_state.file), "%s", rec->file);
            run_state.line = rec->line;
        }
        run_state.failed = true;
    }
    if (!run_state.skipped && live_bytes != base_bytes && !run_state.failed) {
        snprintf(run_state.message, sizeof(run_state.message),
                 "The case leaked %zu allocation bytes.",
                 live_bytes > base_bytes ? live_bytes - base_bytes
                                         : base_bytes - live_bytes);
        snprintf(run_state.file, sizeof(run_state.file), "%s", rec->file);
        run_state.line = rec->line;
        run_state.failed = true;
    }

    if (run_state.failed) {
        ++*failed;
        fprintf(stdout, "[ FAIL ] %s.%s (%.2f ms)\n",
                rec->suite, rec->name, monotonic_ms() - start);
        fprintf(stdout, "         at %s:%d: %s\n",
                run_state.file, run_state.line, run_state.message);
        print_trace_on_failure();
    } else if (run_state.skipped) {
        ++*skipped;
        fprintf(stdout, "[ SKIP ] %s.%s: %s\n",
                rec->suite, rec->name, run_state.message);
    } else {
        ++*passed;
        if (verbose)
            fprintf(stdout, "[ PASS ] %s.%s (%.2f ms)\n",
                    rec->suite, rec->name, monotonic_ms() - start);
    }
}

int test_run_all(int argc, char** argv)
{
    size_t i;
    size_t selected = 0;
    size_t passed = 0;
    size_t failed = 0;
    size_t skipped = 0;
    const char* text_filter = NULL;
    bool verbose = false;
    bool list_only = false;
    double start;

    test_parse_args(argc, argv, &text_filter, &verbose, &list_only);

    if (registry_overflowed) {
        fprintf(stderr, "The case registry overflowed; tests were lost.\n");
        return 2;
    }

    qsort(registry, registry_count, sizeof(registry[0]), compare_records);

    for (i = 1; i < registry_count; ++i) {
        if (strcmp(registry[i - 1].suite, registry[i].suite) == 0
            && strcmp(registry[i - 1].name, registry[i].name) == 0) {
            fprintf(stderr, "Duplicate case `%s.%s` registered at %s:%d and %s:%d.\n",
                    registry[i].suite, registry[i].name,
                    registry[i - 1].file, registry[i - 1].line,
                    registry[i].file, registry[i].line);
            return 2;
        }
    }

    if (list_only) {
        for (i = 0; i < registry_count; ++i) {
            if (!case_selected(&registry[i], text_filter))
                continue;
            fprintf(stdout, "%s.%s\n", registry[i].suite, registry[i].name);
        }
        return 0;
    }

    if (verbose) {
        fprintf(stdout, "Configuration: %s\n", TEST_CONFIG_NAME);
        fprintf(stdout, "Global seed: 0x%016llx\n",
                (unsigned long long)test_global_seed);
        fprintf(stdout, "Model iterations: %zu\n", test_model_iters);
    }

    start = monotonic_ms();
    for (i = 0; i < registry_count; ++i) {
        if (!case_selected(&registry[i], text_filter))
            continue;
        ++selected;
        run_one(&registry[i], verbose, &passed, &failed, &skipped);
    }

    fprintf(stdout, "\nSummary: %zu selected, %zu passed, %zu failed, %zu skipped (%.2f ms)\n",
            selected, passed, failed, skipped, monotonic_ms() - start);

    return failed == 0 ? 0 : 1;
}
