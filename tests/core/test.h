/*
  libj_ds permanent tests core
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

#ifndef __J_TEST_H
#define __J_TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Case registration
 */
typedef void (*test_case_fn)(const void* ctx);

void test_register_case(const char* suite, const char* name,
                        test_case_fn fn, const void* ctx,
                        const char* file, int line);

#if defined(__GNUC__) || defined(__clang__)
#define TEST_CTOR __attribute__((constructor))
#else
#error "The test system needs constructor support (GCC or Clang)."
#endif

#define TEST_CASE(_suite, _name)                                                \
    static void test_body_##_suite##_##_name(void);                             \
    static void test_entry_##_suite##_##_name(const void* ctx)                        \
    {                                                                           \
        (void)ctx;                                                              \
        test_body_##_suite##_##_name();                                         \
    }                                                                           \
    TEST_CTOR static void test_register_##_suite##_##_name(void)                \
    {                                                                           \
        test_register_case(#_suite, #_name, test_entry_##_suite##_##_name,      \
                           NULL, __FILE__, __LINE__);                           \
    }                                                                           \
    static void test_body_##_suite##_##_name(void)

/*
 *  Assertions. Failures longjmp out of the running case.
 */
void test_assert_true(bool cond, const char* expr, const char* file, int line);
void test_assert_false(bool cond, const char* expr, const char* file, int line);
void test_assert_eq(long long expected, long long actual,
                    const char* expr, const char* file, int line);
void test_assert_ne(long long a, long long b,
                    const char* expr, const char* file, int line);
void test_assert_null(const void* ptr, const char* expr,
                      const char* file, int line);
void test_assert_not_null(const void* ptr, const char* expr,
                          const char* file, int line);
void test_assert_ptr_eq(const void* expected, const void* actual,
                        const char* expr, const char* file, int line);
void test_assert_str_eq(const char* expected, const char* actual,
                        const char* expr, const char* file, int line);
void test_assert_mem_eq(const void* expected, const void* actual, size_t size,
                        const char* expr, const char* file, int line);

#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 3, 4)))
#endif
void test_fail_at(const char* file, int line, const char* fmt, ...);

#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 3, 4)))
#endif
void test_skip_at(const char* file, int line, const char* fmt, ...);

#define TEST_ASSERT_TRUE(_cond)     test_assert_true((_cond), #_cond, __FILE__, __LINE__)
#define TEST_ASSERT_FALSE(_cond)    test_assert_false((_cond), #_cond, __FILE__, __LINE__)
#define TEST_ASSERT_EQ(_expected, _actual) \
    test_assert_eq((_expected), (_actual), #_expected " == " #_actual, __FILE__, __LINE__)
#define TEST_ASSERT_NE(_a, _b) \
    test_assert_ne((_a), (_b), #_a " != " #_b, __FILE__, __LINE__)
#define TEST_ASSERT_NULL(_ptr)      test_assert_null((_ptr), #_ptr, __FILE__, __LINE__)
#define TEST_ASSERT_NOT_NULL(_ptr)  test_assert_not_null((_ptr), #_ptr, __FILE__, __LINE__)
#define TEST_ASSERT_PTR_EQ(_expected, _actual) \
    test_assert_ptr_eq((_expected), (_actual), #_expected " == " #_actual, __FILE__, __LINE__)
#define TEST_ASSERT_STR_EQ(_expected, _actual) \
    test_assert_str_eq((_expected), (_actual), #_expected " == " #_actual, __FILE__, __LINE__)
#define TEST_ASSERT_MEM_EQ(_expected, _actual, _size) \
    test_assert_mem_eq((_expected), (_actual), (_size), #_expected " == " #_actual, __FILE__, __LINE__)
#define TEST_ASSERT_OK(_expr)       TEST_ASSERT_TRUE(_expr)
#define TEST_FAIL(...)              test_fail_at(__FILE__, __LINE__, __VA_ARGS__)
#define TEST_SKIP(...)              test_skip_at(__FILE__, __LINE__, __VA_ARGS__)

/*
 *  Cleanup stack
 */
void test_cleanup_push(void (*fn)(void* arg), void* arg);
void test_cleanup_pop(bool run);

/*
 *  Deterministic RNG
 */
extern uint64_t test_rng_seed;

void     test_rng_init(uint64_t seed);
uint64_t test_rng_u64(void);
uint32_t test_rng_u32(void);
size_t   test_rng_bounded(size_t bound);        /* [0, bound); 0 for bound 0 */
size_t   test_rng_range(size_t lo, size_t hi);  /* [lo, hi] */
bool     test_rng_percent(unsigned percent);

/*
 *  Bounded operation trace
 */
void test_trace(const char* op, long long a, long long b, long long c);
void test_trace_reset(void);

/*
 *  Test allocator: strong _p_* overrides with accounting and fail-at-N.
 */
void   test_alloc_reset_stats(void);
size_t test_alloc_calls(void);
size_t test_alloc_live_blocks(void);
size_t test_alloc_live_bytes(void);
size_t test_alloc_peak_blocks(void);
void   test_alloc_count_begin(void);
size_t test_alloc_count_end(void);
void   test_alloc_fail_at(size_t n);            /* 0 disables injection */
void   test_alloc_disarm(void);

typedef struct test_alloc_scenario {
    const char* name;
    void* (*setup)(void);
    bool  (*operate)(void* state);
    void  (*verify)(void* state, bool succeeded);
    void  (*teardown)(void* state);
} test_alloc_scenario_t;

void test_alloc_sweep(const test_alloc_scenario_t* scenario);

/*
 *  Death tests (fork + SIGABRT)
 */
void test_death_expect_abort(void (*fn)(void* arg), void* arg, const char* what);

/*
 *  Runner state shared with container drivers
 */
extern size_t test_model_iters;
extern uint64_t test_global_seed;

/*
 *  Tracked owned value fixture
 */
typedef struct test_tracked {
    uint64_t magic;
    int32_t  payload;
    int32_t  id;
    int32_t* owned;
} test_tracked_t;

test_tracked_t test_tracked_make(int32_t payload);
void  test_tracked_dispose(test_tracked_t* value);
const test_tracked_t* test_tracked_slot_at(const void* slot);
bool  test_tracked_valid_at(const test_tracked_t* value);
int32_t test_tracked_payload_at(const test_tracked_t* value);
size_t test_tracked_live(void);
size_t test_tracked_copies(void);
size_t test_tracked_frees(void);
size_t test_tracked_moves(void);

/*
 *  Sequence ops tables for fixtures
 */
struct class_sequence_container_ops;
const struct class_sequence_container_ops* test_ops_tracked(void);

#ifdef __cplusplus
}
#endif

#endif /* __J_TEST_H */
