/*
  libj_ds permanent tests entry point
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_internal.h"

#ifndef TEST_MODEL_ITERS
#define TEST_MODEL_ITERS 2000
#endif

#ifndef TEST_DEFAULT_SEED
#define TEST_DEFAULT_SEED 0x9e3779b97f4a7c15ULL
#endif

#define TEST_SUITE_FILTER_MAX  32

size_t test_model_iters = TEST_MODEL_ITERS;
uint64_t test_global_seed = TEST_DEFAULT_SEED;

static char suite_filters[TEST_SUITE_FILTER_MAX][TEST_SUITE_MAX];
static size_t suite_filter_count;

static void usage(FILE* out, const char* program)
{
    fprintf(out,
            "Usage: %s [options]\n"
            "  --suite NAME[,NAME]  run only these suites (repeatable, `all` selects everything)\n"
            "  --filter TEXT        run cases whose suite or name contains TEXT\n"
            "  --seed N             override the default seed\n"
            "  --iters N            override the model and stress iteration count\n"
            "  --trace-last N       number of trace rows printed on failure (default 32)\n"
            "  --list               list selected cases and exit\n"
            "  --verbose            print passing cases and run details\n"
            "  --help               show this help\n",
            program);
}

static void add_suite_filters(const char* text)
{
    const char* p = text;

    while (*p != '\0') {
        const char* comma = strchr(p, ',');
        size_t len = comma != NULL ? (size_t)(comma - p) : strlen(p);

        if (len > 0 && suite_filter_count < TEST_SUITE_FILTER_MAX) {
            snprintf(suite_filters[suite_filter_count], TEST_SUITE_MAX,
                     "%.*s", (int)len, p);
            ++suite_filter_count;
        }
        if (comma == NULL)
            break;
        p = comma + 1;
    }
}

static const char* option_value(int argc, char** argv, int* index,
                                const char* arg, const char* name, bool* error)
{
    size_t len = strlen(name);

    if (strncmp(arg, name, len) == 0 && arg[len] == '=') {
        if (arg[len + 1] == '\0') {
            fprintf(stderr, "Option `%s` needs a value.\n", name);
            *error = true;
            return NULL;
        }
        return arg + len + 1;
    }
    if (strcmp(arg, name) == 0) {
        if (*index + 1 >= argc) {
            fprintf(stderr, "Option `%s` needs a value.\n", name);
            *error = true;
            return NULL;
        }
        ++*index;
        return argv[*index];
    }
    return NULL;
}

void test_parse_args(int argc, char** argv, const char** text_filter,
                     bool* verbose, bool* list_only)
{
    int i;

    *text_filter = NULL;
    *verbose = false;
    *list_only = false;
    suite_filter_count = 0;

    for (i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        const char* value;
        bool error = false;

        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            usage(stdout, argv[0]);
            exit(0);
        }
        if (strcmp(arg, "--list") == 0) {
            *list_only = true;
            continue;
        }
        if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0) {
            *verbose = true;
            continue;
        }

        value = option_value(argc, argv, &i, arg, "--suite", &error);
        if (error) {
            usage(stderr, argv[0]);
            exit(2);
        }
        if (value != NULL) {
            add_suite_filters(value);
            continue;
        }

        value = option_value(argc, argv, &i, arg, "--filter", &error);
        if (error) {
            usage(stderr, argv[0]);
            exit(2);
        }
        if (value != NULL) {
            *text_filter = value;
            continue;
        }

        value = option_value(argc, argv, &i, arg, "--seed", &error);
        if (error) {
            usage(stderr, argv[0]);
            exit(2);
        }
        if (value != NULL) {
            test_global_seed = strtoull(value, NULL, 0);
            continue;
        }

        value = option_value(argc, argv, &i, arg, "--iters", &error);
        if (error) {
            usage(stderr, argv[0]);
            exit(2);
        }
        if (value != NULL) {
            test_model_iters = strtoull(value, NULL, 0);
            continue;
        }

        value = option_value(argc, argv, &i, arg, "--trace-last", &error);
        if (error) {
            usage(stderr, argv[0]);
            exit(2);
        }
        if (value != NULL) {
            test_trace_set_limit(strtoull(value, NULL, 0));
            continue;
        }

        fprintf(stderr, "Unknown argument `%s`.\n", arg);
        usage(stderr, argv[0]);
        exit(2);
    }
}

bool test_suite_selected(const char* suite)
{
    size_t i;

    if (suite_filter_count == 0) {
        if (strcmp(suite, "stress") == 0 || strcmp(suite, "bench") == 0)
            return false;
        if (strncmp(suite, "stress_", 7) == 0 || strncmp(suite, "bench_", 6) == 0)
            return false;
        return true;
    }
    for (i = 0; i < suite_filter_count; ++i) {
        if (strcmp(suite_filters[i], "all") == 0)
            return true;
        if (strcmp(suite_filters[i], suite) == 0)
            return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    return test_run_all(argc, argv);
}
