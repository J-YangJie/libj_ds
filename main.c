/*
  Performance Testing
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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
#include <math.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iterator/iterator_inter.h>
#include <hashmap/hashmap.h>
#include <list/list.h>
#include <vector/vector.h>
#include <deque/deque.h>
#include <stack/stack.h>
#include <queue/queue.h>
#include <priority_queue/priority_queue.h>
#include <map/map.h>
#include <multimap/multimap.h>
#include <set/set.h>
#include <multiset/multiset.h>
#include <operations/ds_ops_string.h>


#define GET_DURATION(_data, _time) do { gettimeofday(&time_begin, NULL); _data gettimeofday(&time_end, NULL); \
                                        _time += time_end.tv_usec - time_begin.tv_usec + 1000000 * (time_end.tv_sec - time_begin.tv_sec); } while (0)

// #define DSL_V
#ifdef DSL_V
#define DSL(_ds, _method) (_ds)->_method
#else
#define DSL(_ds, _method) _ds##_##_method
#endif

//#define TEST_HASHMAP        1
//#define TEST_MAP            1
//#define TEST_SET            1
//#define TEST_MULTIMAP       1
//#define TEST_MULTISET       1
//#define TEST_VECTOR         1
//#define TEST_DEQUE          1
//#define TEST_LIST           1
//#define TEST_STACK_VECTOR   1
//#define TEST_STACK_DEQUE    1
//#define TEST_STACK_LIST     1
//#define TEST_QUEUE_DEQUE    1
//#define TEST_QUEUE_LIST     1
//#define TEST_PQUEUE_VECTOR  1
//#define TEST_PQUEUE_DEQUE   1

#if defined(TEST_STACK_VECTOR) || defined(TEST_STACK_DEQUE) || defined(TEST_STACK_LIST)
#define TEST_STACK 1
#endif

#if defined(TEST_QUEUE_DEQUE) || defined(TEST_QUEUE_LIST)
#define TEST_QUEUE 1
#endif

#if defined(TEST_PQUEUE_VECTOR) || defined(TEST_PQUEUE_DEQUE)
#define TEST_PQUEUE 1
#endif


/* Sequence containers (vector/deque/list) */
#ifndef TIMES_SEQUENCE_INSERT
#define TIMES_SEQUENCE_INSERT       10000000
#endif /* TIMES_SEQUENCE_INSERT */
#ifndef TIMES_SEQUENCE_FIND
#define TIMES_SEQUENCE_FIND         100
#endif /* TIMES_SEQUENCE_FIND */
#ifndef TIMES_SEQUENCE_REMOVE
#define TIMES_SEQUENCE_REMOVE       100
#endif /* TIMES_SEQUENCE_REMOVE */
#ifndef TIMES_SEQUENCE_VALUE_RANGE
#define TIMES_SEQUENCE_VALUE_RANGE  100000000
#endif /* TIMES_SEQUENCE_VALUE_RANGE */

/* Container adaptors (stack/queue/priority_queue) */
#ifndef TIMES_ADAPTER_INSERT
#define TIMES_ADAPTER_INSERT        10000000
#endif /* TIMES_ADAPTER_INSERT */
#ifndef TIMES_ADAPTER_READ
#define TIMES_ADAPTER_READ          100000000
#endif /* TIMES_ADAPTER_READ */
#ifndef TIMES_ADAPTER_VALUE_RANGE
#define TIMES_ADAPTER_VALUE_RANGE   100000000
#endif /* TIMES_ADAPTER_VALUE_RANGE */

/* Associative containers (map/set/multimap/multiset) */
#ifndef TIMES_ASSOC_INSERT
#define TIMES_ASSOC_INSERT          10000000
#endif /* TIMES_ASSOC_INSERT */
#ifndef TIMES_ASSOC_FIND
#define TIMES_ASSOC_FIND            100000000
#endif /* TIMES_ASSOC_FIND */
#ifndef TIMES_ASSOC_REMOVE
#define TIMES_ASSOC_REMOVE          50000000
#endif /* TIMES_ASSOC_REMOVE */
#ifndef TIMES_ASSOC_VALUE_RANGE
#define TIMES_ASSOC_VALUE_RANGE     100000000
#endif /* TIMES_ASSOC_VALUE_RANGE */

/* Hash containers (hashmap / unordered_*) */
#ifndef TIMES_HASH_INSERT
#define TIMES_HASH_INSERT           20000000
#endif /* TIMES_HASH_INSERT */
#ifndef TIMES_HASH_FIND
#define TIMES_HASH_FIND             100000000
#endif /* TIMES_HASH_FIND */
#ifndef TIMES_HASH_REMOVE
#define TIMES_HASH_REMOVE           100000000
#endif /* TIMES_HASH_REMOVE */
#ifndef TIMES_HASH_VALUE_RANGE
#define TIMES_HASH_VALUE_RANGE      100000000
#endif /* TIMES_HASH_VALUE_RANGE */

#ifndef HASHMAP_CAPACITY_INIT
#define HASHMAP_CAPACITY_INIT 2 * TIMES_HASH_INSERT
#endif /* HASHMAP_CAPACITY_INIT */

#if defined(TEST_LIST) || defined(TEST_VECTOR) || defined(TEST_DEQUE)
#define INSERT_OPS TIMES_SEQUENCE_INSERT
#define FIND_OPS   TIMES_SEQUENCE_FIND
#define REMOVE_OPS TIMES_SEQUENCE_REMOVE
#elif defined(TEST_STACK) || defined(TEST_QUEUE) || defined(TEST_PQUEUE)
#define INSERT_OPS TIMES_ADAPTER_INSERT
#define FIND_OPS   TIMES_ADAPTER_READ
#define REMOVE_OPS TIMES_ADAPTER_INSERT
#elif defined(TEST_HASHMAP)
#define INSERT_OPS TIMES_HASH_INSERT
#define FIND_OPS   TIMES_HASH_FIND
#define REMOVE_OPS TIMES_HASH_REMOVE
#elif defined(TEST_MAP) || defined(TEST_SET) || defined(TEST_MULTIMAP) || defined(TEST_MULTISET)
#define INSERT_OPS TIMES_ASSOC_INSERT
#define FIND_OPS   TIMES_ASSOC_FIND
#define REMOVE_OPS TIMES_ASSOC_REMOVE
#else
#error "No container test selected (expected one of the TEST_* macros)"
#endif


#ifndef SORT_SEED
#define SORT_SEED 12345
#endif /* SORT_SEED */

#ifndef BENCH_SEED
#define BENCH_SEED 54321
#endif /* BENCH_SEED */

/*
 *  Extra sequence-container scenarios (deque/vector/list) that are not
 *  listed in the README performance tables. Set to 0 to run only the
 *  README baseline: insert / find / remove / clear.
 */
#ifndef PERFORMANCE_SEQUENCE_EXTRA
#define PERFORMANCE_SEQUENCE_EXTRA 1
#endif /* PERFORMANCE_SEQUENCE_EXTRA */

#define TOUCH_TIMERS() do { (void)time_hashmap; (void)time_map; (void)time_set; \
                            (void)time_multimap; (void)time_multiset; \
                            (void)time_list; (void)time_vector; (void)time_deque; \
                            (void)time_stack; (void)time_queue; (void)time_pqueue; } while (0)


#ifdef TEST_HASHMAP
#define DS_NAME      "hashmap"
#define TIME_DS      time_hashmap
#elif defined(TEST_MAP)
#define DS_NAME      "map"
#define TIME_DS      time_map
#elif defined(TEST_SET)
#define DS_NAME      "set"
#define TIME_DS      time_set
#elif defined(TEST_MULTIMAP)
#define DS_NAME      "multimap"
#define TIME_DS      time_multimap
#elif defined(TEST_MULTISET)
#define DS_NAME      "multiset"
#define TIME_DS      time_multiset
#elif defined(TEST_LIST)
#define DS_NAME      "list"
#define TIME_DS      time_list
#elif defined(TEST_VECTOR)
#define DS_NAME      "vector"
#define TIME_DS      time_vector
#elif defined(TEST_DEQUE)
#define DS_NAME      "deque"
#define TIME_DS      time_deque
#elif defined(TEST_STACK_VECTOR)
#define DS_NAME      "stack_vector"
#define TIME_DS      time_stack
#elif defined(TEST_STACK_DEQUE)
#define DS_NAME      "stack_deque"
#define TIME_DS      time_stack
#elif defined(TEST_STACK_LIST)
#define DS_NAME      "stack_list"
#define TIME_DS      time_stack
#elif defined(TEST_QUEUE_DEQUE)
#define DS_NAME      "queue_deque"
#define TIME_DS      time_queue
#elif defined(TEST_QUEUE_LIST)
#define DS_NAME      "queue_list"
#define TIME_DS      time_queue
#elif defined(TEST_PQUEUE_VECTOR)
#define DS_NAME      "pqueue_vector"
#define TIME_DS      time_pqueue
#elif defined(TEST_PQUEUE_DEQUE)
#define DS_NAME      "pqueue_deque"
#define TIME_DS      time_pqueue
#else
#define DS_NAME      "unknown"
#define TIME_DS      time_vector
#endif

#if defined(TEST_STACK)
#define DS_ACCESS_NAME  "top"
#elif defined(TEST_QUEUE)
#define DS_ACCESS_NAME  "front"
#elif defined(TEST_PQUEUE)
#define DS_ACCESS_NAME  "top"
#else
#define DS_ACCESS_NAME  "find"
#endif

#if defined(TEST_STACK) || defined(TEST_QUEUE) || defined(TEST_PQUEUE)
#define DS_REMOVE_NAME  "pop"
#define REMOVE_OPS_PRINT removed
#else
#define DS_REMOVE_NAME  "remove"
#define REMOVE_OPS_PRINT REMOVE_OPS
#endif

#ifdef TEST_HASHMAP
#define RESULT_BUCKET_FMT   " valid/total=%zd/%zd"
#define RESULT_BUCKET_ARGS(_h) \
    , (ssize_t)DSL(chashmap, bucket_valid_count)(_h), (ssize_t)DSL(chashmap, bucket_count)(_h)
#else
#define RESULT_BUCKET_FMT   ""
#define RESULT_BUCKET_ARGS(_h)
#endif /* TEST_HASHMAP */


/*
 *  String representation used by test_s_rand()
 *
 *  TEST_STR_SSO selects SSO strings; without it, every sequence container
 *  and adapter that supports both forms uses `char*` strings instead.
 */
#ifdef TEST_STR_SSO
#define VECTOR_NEW_STR()          VECTOR_NEW_SSO()
#define DEQUE_NEW_STR()           DEQUE_NEW_SSO()
#define LIST_NEW_STR()            LIST_NEW_SSO()
#define STACK_NEW_STR_T(_BASE)    STACK_NEW_SSO_T(_BASE)
#define QUEUE_NEW_STR_T(_BASE)    QUEUE_NEW_SSO_T(_BASE)
#define PQUEUE_NEW_STR_T(_BASE)   PRIORITY_QUEUE_NEW_SSO_T(_BASE)
#else
#define VECTOR_NEW_STR()          VECTOR_NEW_CHAR()
#define DEQUE_NEW_STR()           DEQUE_NEW_CHAR()
#define LIST_NEW_STR()            LIST_NEW_CHAR()
#define STACK_NEW_STR_T(_BASE)    STACK_NEW_CHAR_T(_BASE)
#define QUEUE_NEW_STR_T(_BASE)    QUEUE_NEW_CHAR_T(_BASE)
#define PQUEUE_NEW_STR_T(_BASE)   PRIORITY_QUEUE_NEW_CHAR_T(_BASE)
#endif

#if defined(TEST_STACK_VECTOR)
#define STACK_TEST_NEW()          STACK_NEW()
#define STACK_TEST_NEW_STRING()   STACK_NEW_STR_T(VECTOR)
#elif defined(TEST_STACK_DEQUE)
#define STACK_TEST_NEW()          STACK_NEW_T(intptr_t, DEQUE)
#define STACK_TEST_NEW_STRING()   STACK_NEW_STR_T(DEQUE)
#elif defined(TEST_STACK_LIST)
#define STACK_TEST_NEW()          STACK_NEW_T(intptr_t, LIST)
#define STACK_TEST_NEW_STRING()   STACK_NEW_STR_T(LIST)
#endif

#if defined(TEST_QUEUE_DEQUE)
#define QUEUE_TEST_NEW()          QUEUE_NEW()
#define QUEUE_TEST_NEW_STRING()   QUEUE_NEW_STR_T(DEQUE)
#elif defined(TEST_QUEUE_LIST)
#define QUEUE_TEST_NEW()          QUEUE_NEW_T(intptr_t, LIST)
#define QUEUE_TEST_NEW_STRING()   QUEUE_NEW_STR_T(LIST)
#endif

#if defined(TEST_PQUEUE_VECTOR)
#define PQUEUE_TEST_NEW()         PRIORITY_QUEUE_NEW()
#define PQUEUE_TEST_NEW_STRING()  PQUEUE_NEW_STR_T(VECTOR)
#elif defined(TEST_PQUEUE_DEQUE)
#define PQUEUE_TEST_NEW()         PRIORITY_QUEUE_NEW_T(intptr_t, DEQUE)
#define PQUEUE_TEST_NEW_STRING()  PQUEUE_NEW_STR_T(DEQUE)
#endif


deque_sort_define_less(sort_dq_int,    deque_data_t)
deque_sort_define_greater(sort_dq_ides, deque_data_t)
vector_sort_define_less(sort_vt_int,   vector_data_t)
vector_sort_define_greater(sort_vt_ides, vector_data_t)

#ifdef TEST_STR_SSO
deque_sort_define_sso_less(sort_dq_str)
deque_sort_define_sso_greater(sort_dq_sdes)
vector_sort_define_sso_less(sort_vt_str)
vector_sort_define_sso_greater(sort_vt_sdes)
#else
deque_sort_define_chars_less(sort_dq_str)
deque_sort_define_chars_greater(sort_dq_sdes)
vector_sort_define_chars_less(sort_vt_str)
vector_sort_define_chars_greater(sort_vt_sdes)
#endif

#ifdef TEST_LIST
static bool list_remove_if_odd(list_data_t data)
{
    return 0 != (data & 1);
}

static bool list_remove_if_char_odd(list_data_t data)
{
    return 0 != (((const char*)(uintptr_t)data)[0] & 1);
}

#ifdef TEST_STR_SSO
#define list_lt_str __list_lt_string
#define list_gt_str __list_gt_string
#define list_sorted_str(_this, ascending) clist_sorted_string((_this), (ascending))
#else
#define list_lt_str __list_lt_chars
#define list_gt_str __list_gt_chars
#define list_sorted_str(_this, ascending) clist_sorted_chars((_this), (ascending))
#endif
#endif /* TEST_LIST */

#ifdef TEST_VECTOR
static bool vector_remove_if_odd(vector_data_t data)
{
    return 0 != (data & 1);
}

static bool vector_remove_if_char_odd(vector_data_t data)
{
    return 0 != (((const char*)(uintptr_t)data)[0] & 1);
}
#endif /* TEST_VECTOR */

#ifdef TEST_DEQUE
static bool deque_remove_if_odd(deque_data_t data)
{
    return 0 != (data & 1);
}

static bool deque_remove_if_char_odd(deque_data_t data)
{
    return 0 != (((const char*)(uintptr_t)data)[0] & 1);
}
#endif /* TEST_DEQUE */

static void test_i_for(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_stack    = 0;
    clock_t time_queue    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_i  = HASHMAP_NEW_3(HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_i      = MAP_NEW();
#elif TEST_SET
    set_t*            ds_set_i      = SET_NEW();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_i = MULTIMAP_NEW();
#elif TEST_MULTISET
    multiset_t*       ds_multiset_i = MULTISET_NEW();
#elif TEST_LIST
    list_t*           ds_list_i     = LIST_NEW();
#elif TEST_VECTOR
    vector_t*         ds_vector_i   = VECTOR_NEW();
#elif TEST_DEQUE
    deque_t*          ds_deque_i    = DEQUE_NEW();
#elif TEST_STACK
    stack_t*          ds_stack_i    = STACK_TEST_NEW();
#elif TEST_QUEUE
    queue_t*          ds_queue_i    = QUEUE_TEST_NEW();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PQUEUE_TEST_NEW();
#endif

    printf("%s\n", __func__);

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_INSERT; ++i)     { DSL(chashmap, insert)(ds_hashmap_i, i, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i)    { DSL(cmap, insert)(ds_map_i, i, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i)    { DSL(cset, insert)(ds_set_i, i);              }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i)    { DSL(cmultimap, insert)(ds_multimap_i, i, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i)    { DSL(cmultiset, insert)(ds_multiset_i, i);    }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, i);         }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, i);     }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, i);       }, time_deque);
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)  { DSL(cstack, push)(ds_stack_i, i);            }, time_stack);
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)  { DSL(cqueue, push)(ds_queue_i, i);            }, time_queue);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)  { DSL(cpqueue, push)(ds_pqueue_i, i);          }, time_pqueue);
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms" RESULT_BUCKET_FMT "\n", __func__, "insert", DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000) RESULT_BUCKET_ARGS(ds_hashmap_i));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_HASH_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_STACK
        ds_size = DSL(cstack, size)(ds_stack_i);
        {
            volatile stack_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(stack_data_t*)__cstack_top(ds_stack_i).d;
            }, time_stack);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#elif TEST_QUEUE
        ds_size = DSL(cqueue, size)(ds_queue_i);
        {
            volatile queue_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(queue_data_t*)__cqueue_front(ds_queue_i).d;
            }, time_queue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        {
            volatile priority_queue_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(priority_queue_data_t*)__cpqueue_top(ds_pqueue_i).d;
            }, time_pqueue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms succ=%zu ds_size=%zd\n", __func__, DS_ACCESS_NAME, DS_NAME, (ssize_t)FIND_OPS, (ssize_t)(TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_HASH_VALUE_RANGE);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_vector); /* only 10^2 */
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_deque);  /* only 10^2 */
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cstack_empty(ds_stack_i))
            {
                DSL(cstack, pop)(ds_stack_i);
                removed++;
            }
        }, time_stack);
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cqueue_empty(ds_queue_i))
            {
                DSL(cqueue, pop)(ds_queue_i);
                removed++;
            }
        }, time_queue);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cpqueue_empty(ds_pqueue_i))
            {
                DSL(cpqueue, pop)(ds_pqueue_i);
                removed++;
            }
        }, time_pqueue);
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms removed=%zd\n", __func__, DS_REMOVE_NAME, DS_NAME, (ssize_t)REMOVE_OPS_PRINT, (ssize_t)(TIME_DS / 1000), removed);
    }

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_i);               }, time_map);
        MAP_DELETE(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_i);               }, time_set);
        SET_DELETE(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_i);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_i);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_i);             }, time_list);
        LIST_DELETE(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_i);         }, time_vector);
        VECTOR_DELETE(&ds_vector_i);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_i);           }, time_deque);
        DEQUE_DELETE(&ds_deque_i);
#elif TEST_STACK
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cstack, push)(ds_stack_i, i);
        GET_DURATION({
            removed = DSL(cstack, clear)(ds_stack_i);
            STACK_DELETE(&ds_stack_i);
        }, time_stack);
#elif TEST_QUEUE
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cqueue, push)(ds_queue_i, i);
        GET_DURATION({
            removed = DSL(cqueue, clear)(ds_queue_i);
            QUEUE_DELETE(&ds_queue_i);
        }, time_queue);
#elif TEST_PQUEUE
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cpqueue, push)(ds_pqueue_i, i);
        GET_DURATION({
            removed = DSL(cpqueue, clear)(ds_pqueue_i);
            PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
        }, time_pqueue);
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms removed=%zd\n", __func__, "deinit", DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000), removed);
    }

}

static void test_i_rand(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_stack    = 0;
    clock_t time_queue    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_i  = HASHMAP_NEW_3(HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_i      = MAP_NEW();
#elif TEST_SET
    set_t*            ds_set_i      = SET_NEW();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_i = MULTIMAP_NEW();
#elif TEST_MULTISET
    multiset_t*       ds_multiset_i = MULTISET_NEW();
#elif TEST_LIST
    list_t*           ds_list_i     = LIST_NEW();
#elif TEST_VECTOR
    vector_t*         ds_vector_i   = VECTOR_NEW();
#elif TEST_DEQUE
    deque_t*          ds_deque_i    = DEQUE_NEW();
#elif TEST_STACK
    stack_t*          ds_stack_i    = STACK_TEST_NEW();
#elif TEST_QUEUE
    queue_t*          ds_queue_i    = QUEUE_TEST_NEW();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PQUEUE_TEST_NEW();
#endif

    srand(BENCH_SEED);
    printf("%s\n", __func__);

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_i, rand() % TIMES_HASH_VALUE_RANGE, i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_i, rand() % TIMES_ASSOC_VALUE_RANGE, i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cset, insert)(ds_set_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_i, rand() % TIMES_ASSOC_VALUE_RANGE, i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_deque);
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cstack, push)(ds_stack_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        }, time_stack);
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cqueue, push)(ds_queue_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        }, time_queue);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        }, time_pqueue);
#endif

        printf("RESULT %-11s insert           %s %10zd %6zd ms" RESULT_BUCKET_FMT "\n", __func__, DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000) RESULT_BUCKET_ARGS(ds_hashmap_i));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_HASH_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, rand() % TIMES_HASH_VALUE_RANGE);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, rand() % TIMES_ASSOC_VALUE_RANGE);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, rand() % TIMES_ASSOC_VALUE_RANGE);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, rand() % TIMES_ASSOC_VALUE_RANGE);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, rand() % TIMES_ASSOC_VALUE_RANGE);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_STACK
        ds_size = DSL(cstack, size)(ds_stack_i);
        {
            volatile stack_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(stack_data_t*)__cstack_top(ds_stack_i).d;
            }, time_stack);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#elif TEST_QUEUE
        ds_size = DSL(cqueue, size)(ds_queue_i);
        {
            volatile queue_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(queue_data_t*)__cqueue_front(ds_queue_i).d;
            }, time_queue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        {
            volatile priority_queue_data_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = *(priority_queue_data_t*)__cpqueue_top(ds_pqueue_i).d;
            }, time_pqueue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms succ=%zu ds_size=%zd\n", __func__, DS_ACCESS_NAME, DS_NAME, (ssize_t)FIND_OPS, (ssize_t)(TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_HASH_VALUE_RANGE);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_ASSOC_VALUE_RANGE);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        }, time_deque);
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cstack_empty(ds_stack_i))
            {
                DSL(cstack, pop)(ds_stack_i);
                removed++;
            }
        }, time_stack);
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cqueue_empty(ds_queue_i))
            {
                DSL(cqueue, pop)(ds_queue_i);
                removed++;
            }
        }, time_queue);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cpqueue_empty(ds_pqueue_i))
            {
                DSL(cpqueue, pop)(ds_pqueue_i);
                removed++;
            }
        }, time_pqueue);
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms removed=%zd\n", __func__, DS_REMOVE_NAME, DS_NAME, (ssize_t)REMOVE_OPS_PRINT, (ssize_t)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {
        /* count */
        {
            deque_count_t cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(cdeque, count)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), cnt, DSL(cdeque, size)(ds_deque_i));
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, DSL(cdeque, next)(DSL(cdeque, begin)(ds_deque_i)), DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i))); }, time_deque);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(cdeque, size)(ds_deque_i), (ssize_t)(time_deque / 1000));
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i));                       }, time_deque);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);                             }, time_deque);
        printf("RESULT %-11s push_front       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_i);                                                   }, time_deque);
        printf("RESULT %-11s pop_front        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), rand() % TIMES_SEQUENCE_VALUE_RANGE);   }, time_deque);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_i, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i)));      }, time_deque);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);                              }, time_deque);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_i);                                                    }, time_deque);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert front+10 + erase front+10 */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert back-10 + erase back-10 */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) - 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) - 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_FIND times */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
        }
        printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cdeque, clear)(ds_deque_i);
        {
            const deque_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), (deque_size_t)TIMES_SEQUENCE_INSERT, 9); }, time_deque);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_i, __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2), (deque_size_t)MID_N, 9); }, time_deque);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            {
                deque_size_t     s  = DSL(cdeque, size)(ds_deque_i);
                deque_iterator_t b  = __cdeque_it(ds_deque_i, (s - MID_N) / 2);
                deque_iterator_t e  = __cdeque_it(ds_deque_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, b, e); }, time_deque);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ removed = DSL(cdeque, remove)(ds_deque_i, 9); }, time_deque);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_i));
        }


        /* insert_n + insert_range */
        DSL(cdeque, clear)(ds_deque_i);
        {
            deque_t* src = DEQUE_NEW();

            DSL(cdeque, insert_n)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(cdeque, insert_n)(src,        DSL(cdeque, end)(src),        (deque_size_t)TIMES_SEQUENCE_INSERT / 1, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            {
                deque_iterator_t pos   = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);
                deque_iterator_t begin = __cdeque_it(src, (deque_size_t)(DSL(cdeque, size)(src) * 0.25));
                deque_iterator_t end   = __cdeque_it(src, (deque_size_t)(DSL(cdeque, size)(src) * 0.75));

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, insert_range)(ds_deque_i, pos, begin, end); }, time_deque);
            }
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            DEQUE_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(cdeque, clear)(ds_deque_i);
        {
            deque_t* src = DEQUE_NEW();

            DSL(cdeque, insert_n)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(cdeque, insert_n)(src,        DSL(cdeque, end)(src),        (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2), rand() % TIMES_SEQUENCE_VALUE_RANGE);

            {
                deque_iterator_t begin = __cdeque_it(src, 1);
                deque_iterator_t end   = __cdeque_it(src, (deque_size_t)TIMES_SEQUENCE_INSERT / 2 - 1);

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, assign_range)(ds_deque_i, begin, end); }, time_deque);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            {
                deque_iterator_t begin = __cdeque_it(src, 1);
                deque_iterator_t end   = __cdeque_it(src, DSL(cdeque, size)(src) - 1);

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, assign_range)(ds_deque_i, begin, end); }, time_deque);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(cdeque, size)(src) - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            DEQUE_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(cdeque, clear)(ds_deque_i);
        {
            DSL(cdeque, insert_n)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, assign)(ds_deque_i, (deque_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, assign)(ds_deque_i, (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));
        }


        /* replace_it + replace_index */
        DSL(cdeque, clear)(ds_deque_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_deque = 0;
            {
                deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cdeque_replace_it(ds_deque_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cdeque_replace_index(ds_deque_i, rand() % DSL(cdeque, size)(ds_deque_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_deque);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));
        }


        /* resize grow + shrink */
        DSL(cdeque, clear)(ds_deque_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, resize)(ds_deque_i, (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2), 7); }, time_deque);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, resize)(ds_deque_i, (deque_size_t)TIMES_SEQUENCE_INSERT / 2, 7); }, time_deque);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));
        }


        /* remove_if */
        DSL(cdeque, clear)(ds_deque_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_deque = 0;
            {
                deque_size_t removed;

                GET_DURATION({ removed = DSL(cdeque, remove_if)(ds_deque_i, deque_remove_if_odd); }, time_deque);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_i));
            }
        }


        /* shrink_to_fit */
        DSL(cdeque, clear)(ds_deque_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(cdeque, erase_range)(ds_deque_i, __cdeque_it(ds_deque_i, (deque_size_t)TIMES_SEQUENCE_INSERT * 3 / 4), DSL(cdeque, end)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, shrink_to_fit)(ds_deque_i); }, time_deque);
            printf("RESULT %-11s shrink_to_fit    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 3) / 4, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_i));
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, (deque_data_t)(rand()));

        {
            volatile deque_data_t sink = 0;
            deque_iterator_t      it   = DSL(cdeque, begin)(ds_deque_i);
            deque_iterator_t      end  = DSL(cdeque, end)(ds_deque_i);

            time_deque = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cdeque, next)(it)) { sink += *it.d; }, time_deque);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }

        {
            volatile deque_data_t sink = 0;
            deque_r_iterator_t    it   = DSL(cdeque, rbegin)(ds_deque_i);
            deque_r_iterator_t    end  = DSL(cdeque, rend)(ds_deque_i);

            time_deque = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cdeque, rnext)(it)) { sink += *it.d; }, time_deque);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + __at + __it */
        {
            volatile deque_data_t sink     = 0;
            deque_size_t          max_size = 0;
            bool                  is_empty = false;
            deque_iterator_t      mid      = DSL(cdeque, end)(ds_deque_i);

            time_deque = 0;
            GET_DURATION({ max_size = DSL(cdeque, max_size)(ds_deque_i); is_empty = __cdeque_empty(ds_deque_i); }, time_deque);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_deque / 1000), max_size, is_empty);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = *DSL(cdeque, front)(ds_deque_i).d; sink = *DSL(cdeque, back)(ds_deque_i).d; }, time_deque);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = *__cdeque_at(ds_deque_i, rand() % DSL(cdeque, size)(ds_deque_i)).d; }, time_deque);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { mid = __cdeque_it(ds_deque_i, rand() % DSL(cdeque, size)(ds_deque_i)); sink += *mid.d; }, time_deque);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, (deque_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort_dq_int(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_deque / 1000),
               sort_dq_int_sorted(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)),
               DSL(cdeque, size)(ds_deque_i));

        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, (deque_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort_dq_ides(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_deque / 1000),
               sort_dq_ides_sorted(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)),
               DSL(cdeque, size)(ds_deque_i));
    }
#endif

#ifdef TEST_VECTOR
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {

#if 0
#define DS_CAP(_ds)   do { printf("%zd - %zd = %zd\n", cvector_capacity(_ds), cvector_size(_ds), cvector_capacity(_ds) - cvector_size(_ds)); } while (0)
#else
#define DS_CAP(_ds)   do { } while (0)
#endif

        /* count */
        {
            vector_count_t cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(cvector, count)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), cnt, DSL(cvector, size)(ds_vector_i));
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, DSL(cvector, next)(DSL(cvector, begin)(ds_vector_i)), DSL(cvector, prev)(DSL(cvector, end)(ds_vector_i))); }, time_vector);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(cvector, size)(ds_vector_i), (ssize_t)(time_vector / 1000));
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_SEQUENCE_INSERT / 500 times */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) { DSL(cvector, insert)(ds_vector_i, DSL(cvector, begin)(ds_vector_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) { DSL(cvector, erase)(ds_vector_i, DSL(cvector, begin)(ds_vector_i));                       }, time_vector);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert/push back + erase/pop back */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, insert)(ds_vector_i, DSL(cvector, end)(ds_vector_i), rand() % TIMES_SEQUENCE_VALUE_RANGE);   }, time_vector);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, erase)(ds_vector_i, DSL(cvector, prev)(DSL(cvector, end)(ds_vector_i)));      }, time_vector);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);                              }, time_vector);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_i);                                                    }, time_vector);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert front+10 + erase front+10 */ /* only TIMES_SEQUENCE_INSERT / 500 times */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert back-10 + erase back-10 */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) - 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) - 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_FIND times */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
        }
        printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            const vector_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_i, DSL(cvector, end)(ds_vector_i), (vector_size_t)TIMES_SEQUENCE_INSERT, 9); }, time_vector);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_i, __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2), (vector_size_t)MID_N, 9); }, time_vector);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            {
                vector_size_t     s  = DSL(cvector, size)(ds_vector_i);
                vector_iterator_t b  = __cvector_it(ds_vector_i, (s - MID_N) / 2);
                vector_iterator_t e  = __cvector_it(ds_vector_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, b, e); }, time_vector);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            GET_DURATION({ removed = DSL(cvector, remove)(ds_vector_i, 9); }, time_vector);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), removed, DSL(cvector, size)(ds_vector_i));
        }


        /* insert_n + insert_range */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            vector_t* src = VECTOR_NEW();

            DSL(cvector, insert_n)(ds_vector_i, DSL(cvector, end)(ds_vector_i), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(cvector, insert_n)(src,         DSL(cvector, end)(src),         (vector_size_t)TIMES_SEQUENCE_INSERT / 1, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_range)(ds_vector_i, 
                                                      __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2), 
                                                      __cvector_it(src, DSL(cvector, size)(src) * 0.25), 
                                                      __cvector_it(src, DSL(cvector, size)(src) * 0.75)); }, time_vector);
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            VECTOR_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            vector_t* src = VECTOR_NEW();

            DSL(cvector, reserve)(ds_vector_i, (vector_size_t)TIMES_SEQUENCE_INSERT);
            DSL(cvector, insert_n)(ds_vector_i, DSL(cvector, end)(ds_vector_i), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(cvector, insert_n)(src,         DSL(cvector, end)(src),         (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2), rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, assign_range)(ds_vector_i, 
                                                      __cvector_it(src, 1), 
                                                      __cvector_it(src, (vector_size_t)TIMES_SEQUENCE_INSERT / 2 - 1)); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, assign_range)(ds_vector_i, 
                                                      __cvector_it(src, 1), 
                                                      __cvector_it(src, DSL(cvector, size)(src) - 1)); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(cvector, size)(src) - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            VECTOR_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            vector_t* src = VECTOR_NEW();

            DSL(cvector, reserve)(ds_vector_i, (vector_size_t)TIMES_SEQUENCE_INSERT);
            DSL(cvector, insert_n)(ds_vector_i, DSL(cvector, end)(ds_vector_i), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, assign)(ds_vector_i, (vector_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, assign)(ds_vector_i, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            VECTOR_DELETE(&src);
        }


        /* replace_it + replace_index */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            {
                vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cvector_replace_it(ds_vector_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cvector_replace_index(ds_vector_i, rand() % DSL(cvector, size)(ds_vector_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_vector);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));
        }


        /* resize grow + shrink */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, resize)(ds_vector_i, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2), 7); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, resize)(ds_vector_i, (vector_size_t)TIMES_SEQUENCE_INSERT / 2, 7); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));
        }


        /* remove_if */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            {
                vector_size_t removed;

                GET_DURATION({ removed = DSL(cvector, remove_if)(ds_vector_i, vector_remove_if_odd); }, time_vector);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), removed, DSL(cvector, size)(ds_vector_i));
            }
        }


        /* shrink_to_fit */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            DSL(cvector, reserve)(ds_vector_i, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2));
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, shrink_to_fit)(ds_vector_i); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s shrink_to_fit    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));
        }


        /* reserve */ /* vector only */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            time_vector = 0;
            DS_CAP(ds_vector_i);
            GET_DURATION({ DSL(cvector, reserve)(ds_vector_i, (vector_size_t)TIMES_SEQUENCE_INSERT); }, time_vector);
            DS_CAP(ds_vector_i);
            printf("RESULT %-11s reserve          %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_i));
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, (vector_data_t)(rand()));

        {
            volatile vector_data_t sink = 0;
            vector_iterator_t      it   = DSL(cvector, begin)(ds_vector_i);
            vector_iterator_t      end  = DSL(cvector, end)(ds_vector_i);

            time_vector = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cvector, next)(it)) { sink += *it.d; }, time_vector);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }

        {
            volatile vector_data_t sink = 0;
            vector_r_iterator_t    it   = DSL(cvector, rbegin)(ds_vector_i);
            vector_r_iterator_t    end  = DSL(cvector, rend)(ds_vector_i);

            time_vector = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cvector, rnext)(it)) { sink += *it.d; }, time_vector);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + __at + __it */
        {
            volatile vector_data_t sink     = 0;
            vector_size_t          max_size = 0;
            bool                   is_empty = false;
            vector_iterator_t      mid      = DSL(cvector, end)(ds_vector_i);

            time_vector = 0;
            GET_DURATION({ max_size = DSL(cvector, max_size)(ds_vector_i); is_empty = __cvector_empty(ds_vector_i); }, time_vector);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_vector / 1000), max_size, is_empty);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = *DSL(cvector, front)(ds_vector_i).d; sink = *DSL(cvector, back)(ds_vector_i).d; }, time_vector);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = *__cvector_at(ds_vector_i, rand() % DSL(cvector, size)(ds_vector_i)).d; }, time_vector);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { mid = __cvector_it(ds_vector_i, rand() % DSL(cvector, size)(ds_vector_i)); sink += *mid.d; }, time_vector);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, (vector_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort_vt_int(DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)); }, time_vector);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_vector / 1000),
               sort_vt_int_sorted(DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)),
               DSL(cvector, size)(ds_vector_i));

        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, (vector_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort_vt_ides(DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)); }, time_vector);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_vector / 1000),
               sort_vt_ides_sorted(DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)),
               DSL(cvector, size)(ds_vector_i));

#undef DS_CAP
    }
#endif

#ifdef TEST_LIST
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {
        /* count */
        {
            list_count_t cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(clist, count)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_list / 1000), cnt, DSL(clist, size)(ds_list_i));
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ DSL(clist, erase_range)(ds_list_i, DSL(clist, next)(DSL(clist, begin)(ds_list_i)), DSL(clist, prev)(DSL(clist, end)(ds_list_i))); }, time_list);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(clist, size)(ds_list_i), (ssize_t)(time_list / 1000));
        GET_DURATION({ DSL(clist, erase_range)(ds_list_i, DSL(clist, begin)(ds_list_i), DSL(clist, end)(ds_list_i)); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, insert)(ds_list_i, DSL(clist, begin)(ds_list_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, erase)(ds_list_i, DSL(clist, begin)(ds_list_i));                       }, time_list);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, push_front)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);                             }, time_list);
        printf("RESULT %-11s push_front       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, pop_front)(ds_list_i);                                                   }, time_list);
        printf("RESULT %-11s pop_front        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, insert)(ds_list_i, DSL(clist, end)(ds_list_i), rand() % TIMES_SEQUENCE_VALUE_RANGE);   }, time_list);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, erase)(ds_list_i, DSL(clist, prev)(DSL(clist, end)(ds_list_i)));      }, time_list);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);                              }, time_list);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, pop_back)(ds_list_i);                                                    }, time_list);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert front+10 + erase front+10 */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_i, it); }, time_list);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert back-10 + erase back-10 */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) - 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) - 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_i, it); }, time_list);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_INSERT / 5 times */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
        {
            list_iterator_t mid = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 5; ++i) { DSL(clist, insert)(ds_list_i, mid, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 5, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }

        {
            list_iterator_t mid = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 5; ++i) { mid = DSL(clist, erase)(ds_list_i, mid); }, time_list);
            printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 5, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(clist, clear)(ds_list_i);
        {
            const list_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_i, DSL(clist, end)(ds_list_i), (list_size_t)TIMES_SEQUENCE_INSERT, 9); }, time_list);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_i, __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2), (list_size_t)MID_N, 9); }, time_list);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            {
                list_size_t     s  = DSL(clist, size)(ds_list_i);
                list_iterator_t b  = __clist_it(ds_list_i, (s - MID_N) / 2);
                list_iterator_t e  = __clist_it(ds_list_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(clist, erase_range)(ds_list_i, b, e); }, time_list);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ removed = DSL(clist, remove)(ds_list_i, 9); }, time_list);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_i));
        }


        /* insert_n + insert_range */
        DSL(clist, clear)(ds_list_i);
        {
            list_t* src = LIST_NEW();

            DSL(clist, insert_n)(ds_list_i, DSL(clist, end)(ds_list_i), (list_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(clist, insert_n)(src,       DSL(clist, end)(src),       (list_size_t)TIMES_SEQUENCE_INSERT / 1, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            {
                list_iterator_t pos   = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);
                list_iterator_t begin = __clist_it(src, DSL(clist, size)(src) * 0.25);
                list_iterator_t end   = __clist_it(src, DSL(clist, size)(src) * 0.75);

                time_list = 0;
                GET_DURATION({ DSL(clist, insert_range)(ds_list_i, pos, begin, end); }, time_list);
            }
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            LIST_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(clist, clear)(ds_list_i);
        {
            list_t* src = LIST_NEW();

            DSL(clist, insert_n)(ds_list_i, DSL(clist, end)(ds_list_i), (list_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            DSL(clist, insert_n)(src,       DSL(clist, end)(src),       (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2), rand() % TIMES_SEQUENCE_VALUE_RANGE);

            {
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, (list_size_t)TIMES_SEQUENCE_INSERT / 2 - 1);

                time_list = 0;
                GET_DURATION({ DSL(clist, assign_range)(ds_list_i, begin, end); }, time_list);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            {
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, DSL(clist, size)(src) - 1);

                time_list = 0;
                GET_DURATION({ DSL(clist, assign_range)(ds_list_i, begin, end); }, time_list);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(clist, size)(src) - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            LIST_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(clist, clear)(ds_list_i);
        {
            DSL(clist, insert_n)(ds_list_i, DSL(clist, end)(ds_list_i), (list_size_t)TIMES_SEQUENCE_INSERT / 2, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            GET_DURATION({ DSL(clist, assign)(ds_list_i, (list_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ DSL(clist, assign)(ds_list_i, (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* replace_it + replace_index */
        DSL(clist, clear)(ds_list_i);
        {
            const int TIMES_INDEX = TIMES_SEQUENCE_FIND / 20;

            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            {
                list_iterator_t it = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __clist_replace_it(ds_list_i, it, rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { __clist_replace_index(ds_list_i, rand() % DSL(clist, size)(ds_list_i), rand() % TIMES_SEQUENCE_VALUE_RANGE); }, time_list);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* resize grow + shrink */
        DSL(clist, clear)(ds_list_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            GET_DURATION({ DSL(clist, resize)(ds_list_i, (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2), 7); }, time_list);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ DSL(clist, resize)(ds_list_i, (list_size_t)TIMES_SEQUENCE_INSERT / 2, 7); }, time_list);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* remove_if */
        DSL(clist, clear)(ds_list_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, remove_if)(ds_list_i, list_remove_if_odd); }, time_list);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_i));
            }
        }


        /* unique + unique_if */
        DSL(clist, clear)(ds_list_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % 2);

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, unique)(ds_list_i); }, time_list);
                printf("RESULT %-11s unique           %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_i));
            }
        }

        DSL(clist, clear)(ds_list_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % 2);

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, unique_if)(ds_list_i, __ds_ops_eq_default); }, time_list);
                printf("RESULT %-11s unique_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_i));
            }
        }


        /* reverse */
        DSL(clist, clear)(ds_list_i);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            GET_DURATION({ DSL(clist, reverse)(ds_list_i); }, time_list);
            printf("RESULT %-11s reverse          %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* splice */
        DSL(clist, clear)(ds_list_i);
        {
            list_t* src = LIST_NEW();

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(src, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
                if (i & 1)
                    DSL(clist, splice)(src, DSL(clist, end)(src), ds_list_i);
                else
                    DSL(clist, splice)(ds_list_i, DSL(clist, end)(ds_list_i), src);
            }, time_list);
            printf("RESULT %-11s splice           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i) + DSL(clist, size)(src));

            LIST_DELETE(&src);
        }


        /* splice_range */
        DSL(clist, clear)(ds_list_i);
        {
            list_t*     src = LIST_NEW();
            list_size_t n   = (list_size_t)TIMES_SEQUENCE_INSERT / 2;

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_SEQUENCE_VALUE_RANGE);
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(src, rand() % TIMES_SEQUENCE_VALUE_RANGE);

            {
                list_iterator_t pos   = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, n);

                time_list = 0;
                GET_DURATION({ DSL(clist, splice_range)(ds_list_i, pos, src, begin, end); }, time_list);
            }
            printf("RESULT %-11s splice_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(n - 1), (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_i));

            LIST_DELETE(&src);
        }


        /* merge */
        DSL(clist, clear)(ds_list_i);
        {
            list_t* src = LIST_NEW();

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));
            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(src, (list_data_t)(rand()));
            DSL(clist, sort)(ds_list_i, __list_lt_default);
            DSL(clist, sort)(src, __list_lt_default);

            time_list = 0;
            GET_DURATION({ DSL(clist, merge)(ds_list_i, src, __list_lt_default); }, time_list);
            printf("RESULT %-11s merge            %s %10zd %6zd ms sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), clist_sorted_default(ds_list_i, true), DSL(clist, size)(ds_list_i));

            LIST_DELETE(&src);
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));

        {
            volatile list_data_t sink = 0;
            list_iterator_t      it   = DSL(clist, begin)(ds_list_i);
            list_iterator_t      end  = DSL(clist, end)(ds_list_i);

            time_list = 0;
            GET_DURATION(for (; it.d != end.d; it = DSL(clist, next)(it)) { sink += *(it.d); }, time_list);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }

        {
            volatile list_data_t sink = 0;
            list_r_iterator_t    it   = DSL(clist, rbegin)(ds_list_i);
            list_r_iterator_t    end  = DSL(clist, rend)(ds_list_i);

            time_list = 0;
            GET_DURATION(for (; it.d != end.d; it = DSL(clist, rnext)(it)) { sink += *(it.d); }, time_list);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + at + it */
        {
            const int            TIMES_INDEX = TIMES_SEQUENCE_FIND / 20;
            volatile list_data_t sink     = 0;
            list_size_t          max_size = 0;
            bool                 is_empty = false;
            list_iterator_t      mid      = DSL(clist, end)(ds_list_i);

            time_list = 0;
            GET_DURATION({ max_size = DSL(clist, max_size)(ds_list_i); is_empty = __clist_empty(ds_list_i); }, time_list);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_list / 1000), max_size, is_empty);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = *DSL(clist, front)(ds_list_i).d; sink = *DSL(clist, back)(ds_list_i).d; }, time_list);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { sink = *__clist_at(ds_list_i, rand() % DSL(clist, size)(ds_list_i)).d; }, time_list);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), (ssize_t)sink);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { mid = __clist_it(ds_list_i, rand() % DSL(clist, size)(ds_list_i)); sink += *mid.d; }, time_list);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_i, __list_lt_default); }, time_list);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_list / 1000),
               clist_sorted_default(ds_list_i, true),
               DSL(clist, size)(ds_list_i));

        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_i); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_i, __list_gt_default); }, time_list);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_list / 1000),
               clist_sorted_default(ds_list_i, false),
               DSL(clist, size)(ds_list_i));
    }
#endif

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_i);               }, time_map);
        MAP_DELETE(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_i);               }, time_set);
        SET_DELETE(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_i);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_i);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_i);             }, time_list);
        LIST_DELETE(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_i);         }, time_vector);
        VECTOR_DELETE(&ds_vector_i);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_i);           }, time_deque);
        DEQUE_DELETE(&ds_deque_i);
#elif TEST_STACK
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cstack, push)(ds_stack_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        GET_DURATION({
            removed = DSL(cstack, clear)(ds_stack_i);
            STACK_DELETE(&ds_stack_i);
        }, time_stack);
#elif TEST_QUEUE
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cqueue, push)(ds_queue_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        GET_DURATION({
            removed = DSL(cqueue, clear)(ds_queue_i);
            QUEUE_DELETE(&ds_queue_i);
        }, time_queue);
#elif TEST_PQUEUE
        for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
            DSL(cpqueue, push)(ds_pqueue_i, rand() % TIMES_ADAPTER_VALUE_RANGE);
        GET_DURATION({
            removed = DSL(cpqueue, clear)(ds_pqueue_i);
            PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
        }, time_pqueue);
#endif

        printf("RESULT %-11s deinit           %s %10zd %6zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000), removed);
    }
}

#ifndef S_POOL_LEN
#define S_POOL_LEN    10000000
#endif /* S_POOL_LEN */
#ifndef S_STR_LEN_MIN
#define S_STR_LEN_MIN 16
#endif /* S_STR_LEN_MIN */
#ifndef S_STR_LEN_MAX
#define S_STR_LEN_MAX 31
#endif /* S_STR_LEN_MAX */

static char* g_s_pool = NULL;

static void s_pool_init(void)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    if (g_s_pool)
        return;

    g_s_pool = malloc(S_POOL_LEN);
    if (!g_s_pool) {
        fprintf(stderr, "%s: pool malloc fail\n", __func__);
        exit(1);
    }
    for (int i = 0; i < S_POOL_LEN; ++i)
        g_s_pool[i] = charset[rand() % (int)(sizeof(charset) - 1)];
}

static char* s_pool_str(char* dst)
{
    int len   = S_STR_LEN_MIN + rand() % (S_STR_LEN_MAX - S_STR_LEN_MIN + 1); /* 16..31 */
    int index = rand() % (S_POOL_LEN - S_STR_LEN_MAX + 1);                    /* index+len ≤ S_POOL_LEN */

    for (int i = 0; i < len; ++i)
        dst[i] = g_s_pool[index + i];
    dst[len] = '\0';
    return dst;
}

static void test_s_rand(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_stack    = 0;
    clock_t time_queue    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_s  = HASHMAP_NEW_KV_3(string, intptr_t, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_s      = MAP_NEW_KV(string, int16_t);
#elif TEST_SET
    set_t*            ds_set_s      = SET_NEW_T(string);
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_s = MULTIMAP_NEW_KV(string, int16_t);
#elif TEST_MULTISET
    multiset_t*       ds_multiset_s = MULTISET_NEW_T(string);
#elif TEST_LIST
    list_t*           ds_list_s     = LIST_NEW_STR();
#elif TEST_VECTOR
    vector_t*         ds_vector_s   = VECTOR_NEW_STR();
#elif TEST_DEQUE
    deque_t*          ds_deque_s    = DEQUE_NEW_STR();
#elif TEST_STACK
    stack_t*          ds_stack_s    = STACK_TEST_NEW_STRING();
#elif TEST_QUEUE
    queue_t*          ds_queue_s    = QUEUE_TEST_NEW_STRING();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_s   = PQUEUE_TEST_NEW_STRING();
#endif

    srand(BENCH_SEED);
    printf("%s\n", __func__);
    s_pool_init();

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst), i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_s, (map_key_t)s_pool_str(dst), i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cset, insert)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_s, (multimap_key_t)s_pool_str(dst), i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            DSL(cdeque, push_back)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cstack, push)(ds_stack_s, (stack_data_t)s_pool_str(dst));
        }, time_stack);
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cqueue, push)(ds_queue_s, (queue_data_t)s_pool_str(dst));
        }, time_queue);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_s, (priority_queue_data_t)s_pool_str(dst));
        }, time_pqueue);
#endif

        printf("RESULT %-11s insert           %s %10zd %6zd ms" RESULT_BUCKET_FMT "\n", __func__, DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000) RESULT_BUCKET_ARGS(ds_hashmap_s));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_s);
        GET_DURATION(for (int i = 0; i < TIMES_HASH_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_s);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_s, (map_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_s);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_s, (set_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_s);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_s);
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_s);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_s, (list_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_s);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_s, (vector_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_s);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_s, (deque_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_STACK
        ds_size = DSL(cstack, size)(ds_stack_s);
        {
            volatile size_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = (size_t)(uintptr_t)*__cstack_top(ds_stack_s).sd;
            }, time_stack);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
        (void)dst;
#elif TEST_QUEUE
        ds_size = DSL(cqueue, size)(ds_queue_s);
        {
            volatile size_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = (size_t)(uintptr_t)*__cqueue_front(ds_queue_s).sd;
            }, time_queue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
        (void)dst;
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_s);
        {
            volatile size_t sink = 0;
            GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_READ; ++i) {
                sink = (size_t)(uintptr_t)*__cpqueue_top(ds_pqueue_s).sd;
            }, time_pqueue);
            (void)sink;
        }
        times_succ = TIMES_ADAPTER_READ;
        (void)dst;
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms succ=%zu ds_size=%zd\n", __func__, DS_ACCESS_NAME, DS_NAME, (ssize_t)FIND_OPS, (ssize_t)(TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_HASH_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_s, (map_key_t)s_pool_str(dst));
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_ASSOC_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(clist, remove)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cvector, remove)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_REMOVE; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_STACK
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cstack_empty(ds_stack_s))
            {
                DSL(cstack, pop)(ds_stack_s);
                removed++;
            }
        }, time_stack);
        (void)dst;
#elif TEST_QUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cqueue_empty(ds_queue_s))
            {
                DSL(cqueue, pop)(ds_queue_s);
                removed++;
            }
        }, time_queue);
        (void)dst;
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i) {
            // if (!__cpqueue_empty(ds_pqueue_s))
            {
                DSL(cpqueue, pop)(ds_pqueue_s);
                removed++;
            }
        }, time_pqueue);
        (void)dst;
#endif

        printf("RESULT %-11s %-16s %s %10zd %6zd ms removed=%zd\n", __func__, DS_REMOVE_NAME, DS_NAME, (ssize_t)REMOVE_OPS_PRINT, (ssize_t)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((deque_data_t)s_pool_str(dst))

        /* count */
        {
            deque_count_t cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(cdeque, count)(ds_deque_s, DS_ARG()); }, time_deque);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), cnt, DSL(cdeque, size)(ds_deque_s));
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, DSL(cdeque, next)(DSL(cdeque, begin)(ds_deque_s)), DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_s))); }, time_deque);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(cdeque, size)(ds_deque_s), (ssize_t)(time_deque / 1000));
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s), DS_ARG());       }, time_deque);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s));                  }, time_deque);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_s, DS_ARG());                                   }, time_deque);
        printf("RESULT %-11s push_front       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_s);                                              }, time_deque);
        printf("RESULT %-11s pop_front        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), DS_ARG());         }, time_deque);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_s, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_s))); }, time_deque);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_s, DS_ARG());                                    }, time_deque);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_s);                                               }, time_deque);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert front+10 + erase front+10 */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert back-10 + erase back-10 */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) - 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) - 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_FIND times */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cdeque, clear)(ds_deque_s);
        s_pool_str(fixbuf);
        {
            const deque_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), (deque_size_t)TIMES_SEQUENCE_INSERT, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_s, __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2), (deque_size_t)MID_N, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            {
                deque_size_t     s  = DSL(cdeque, size)(ds_deque_s);
                deque_iterator_t b  = __cdeque_it(ds_deque_s, (s - MID_N) / 2);
                deque_iterator_t e  = __cdeque_it(ds_deque_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, b, e); }, time_deque);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ removed = DSL(cdeque, remove)(ds_deque_s, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_s));
        }


        /* insert_n + insert_range */
        DSL(cdeque, clear)(ds_deque_s);
        {
            deque_t* src = DEQUE_NEW_STR();

            DSL(cdeque, insert_n)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(cdeque, insert_n)(src,        DSL(cdeque, end)(src),        (deque_size_t)TIMES_SEQUENCE_INSERT / 1, DS_ARG());

            {
                deque_iterator_t pos   = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);
                deque_iterator_t begin = __cdeque_it(src, (deque_size_t)(DSL(cdeque, size)(src) * 0.25));
                deque_iterator_t end   = __cdeque_it(src, (deque_size_t)(DSL(cdeque, size)(src) * 0.75));

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, insert_range)(ds_deque_s, pos, begin, end); }, time_deque);
            }
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            DEQUE_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(cdeque, clear)(ds_deque_s);
        {
            deque_t* src = DEQUE_NEW_STR();

            DSL(cdeque, insert_n)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(cdeque, insert_n)(src,        DSL(cdeque, end)(src),        (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2), DS_ARG());

            {
                deque_iterator_t begin = __cdeque_it(src, 1);
                deque_iterator_t end   = __cdeque_it(src, (deque_size_t)TIMES_SEQUENCE_INSERT / 2 - 1);

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, assign_range)(ds_deque_s, begin, end); }, time_deque);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            {
                deque_iterator_t begin = __cdeque_it(src, 1);
                deque_iterator_t end   = __cdeque_it(src, DSL(cdeque, size)(src) - 1);

                time_deque = 0;
                GET_DURATION({ DSL(cdeque, assign_range)(ds_deque_s, begin, end); }, time_deque);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(cdeque, size)(src) - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            DEQUE_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(cdeque, clear)(ds_deque_s);
        s_pool_str(fixbuf);
        {
            DSL(cdeque, insert_n)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), (deque_size_t)TIMES_SEQUENCE_INSERT / 2, (deque_data_t)fixbuf);

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, assign)(ds_deque_s, (deque_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, assign)(ds_deque_s, (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));
        }


        /* replace_it + replace_index */
        DSL(cdeque, clear)(ds_deque_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

            time_deque = 0;
            {
                deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cdeque_replace_it(ds_deque_s, it, DS_ARG()); }, time_deque);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cdeque_replace_index(ds_deque_s, rand() % DSL(cdeque, size)(ds_deque_s), DS_ARG()); }, time_deque);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));
        }


        /* resize grow + shrink */
        DSL(cdeque, clear)(ds_deque_s);
        s_pool_str(fixbuf);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, resize)(ds_deque_s, (deque_size_t)(TIMES_SEQUENCE_INSERT * 1.2), (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, resize)(ds_deque_s, (deque_size_t)TIMES_SEQUENCE_INSERT / 2, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));
        }


        /* remove_if */
        DSL(cdeque, clear)(ds_deque_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

            time_deque = 0;
            {
                deque_size_t removed;

                GET_DURATION({ removed = DSL(cdeque, remove_if)(ds_deque_s, deque_remove_if_char_odd); }, time_deque);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_s));
            }
        }


        /* shrink_to_fit */
        DSL(cdeque, clear)(ds_deque_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());
            DSL(cdeque, erase_range)(ds_deque_s, __cdeque_it(ds_deque_s, (deque_size_t)TIMES_SEQUENCE_INSERT * 3 / 4), DSL(cdeque, end)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, shrink_to_fit)(ds_deque_s); }, time_deque);
            printf("RESULT %-11s shrink_to_fit    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 3) / 4, (ssize_t)(time_deque / 1000), DSL(cdeque, size)(ds_deque_s));
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        {
            volatile deque_data_t sink = 0;
            deque_iterator_t      it   = DSL(cdeque, begin)(ds_deque_s);
            deque_iterator_t      end  = DSL(cdeque, end)(ds_deque_s);

            time_deque = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cdeque, next)(it)) { sink += (deque_data_t)(uintptr_t)*it.sd; }, time_deque);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }

        {
            volatile deque_data_t sink = 0;
            deque_r_iterator_t    it   = DSL(cdeque, rbegin)(ds_deque_s);
            deque_r_iterator_t    end  = DSL(cdeque, rend)(ds_deque_s);

            time_deque = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cdeque, rnext)(it)) { sink += (deque_data_t)(uintptr_t)*it.sd; }, time_deque);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + __at + __it */
        {
            volatile deque_data_t sink     = 0;
            deque_size_t          max_size = 0;
            bool                  is_empty = false;
            deque_iterator_t      mid      = DSL(cdeque, end)(ds_deque_s);

            time_deque = 0;
            GET_DURATION({ max_size = DSL(cdeque, max_size)(ds_deque_s); is_empty = __cdeque_empty(ds_deque_s); }, time_deque);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_deque / 1000), max_size, is_empty);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = (deque_data_t)(uintptr_t)*DSL(cdeque, front)(ds_deque_s).sd; sink = (deque_data_t)(uintptr_t)*DSL(cdeque, back)(ds_deque_s).sd; }, time_deque);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = (deque_data_t)(uintptr_t)*__cdeque_at(ds_deque_s, rand() % DSL(cdeque, size)(ds_deque_s)).sd; }, time_deque);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { mid = __cdeque_it(ds_deque_s, rand() % DSL(cdeque, size)(ds_deque_s)); sink += (deque_data_t)(uintptr_t)*mid.sd; }, time_deque);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_deque / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort_dq_str(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_deque / 1000),
               sort_dq_str_sorted(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)),
               DSL(cdeque, size)(ds_deque_s));

        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort_dq_sdes(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_deque / 1000),
               sort_dq_sdes_sorted(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)),
               DSL(cdeque, size)(ds_deque_s));

#undef DS_ARG
    }
#endif

#ifdef TEST_VECTOR
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((vector_data_t)s_pool_str(dst))

#if 0
#define DS_CAP(_ds)   do { printf("%zd - %zd = %zd\n", cvector_capacity(_ds), cvector_size(_ds), cvector_capacity(_ds) - cvector_size(_ds)); } while (0)
#else
#define DS_CAP(_ds)   do { } while (0)
#endif

        /* count */
        {
            vector_count_t cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(cvector, count)(ds_vector_s, DS_ARG()); }, time_vector);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), cnt, DSL(cvector, size)(ds_vector_s));
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, DSL(cvector, next)(DSL(cvector, begin)(ds_vector_s)), DSL(cvector, prev)(DSL(cvector, end)(ds_vector_s))); }, time_vector);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(cvector, size)(ds_vector_s), (ssize_t)(time_vector / 1000));
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_SEQUENCE_INSERT / 500 times */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) { DSL(cvector, insert)(ds_vector_s, DSL(cvector, begin)(ds_vector_s), DS_ARG());       }, time_vector);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) { DSL(cvector, erase)(ds_vector_s, DSL(cvector, begin)(ds_vector_s));                  }, time_vector);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert/push back + erase/pop back */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, insert)(ds_vector_s, DSL(cvector, end)(ds_vector_s), DS_ARG());         }, time_vector);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, erase)(ds_vector_s, DSL(cvector, prev)(DSL(cvector, end)(ds_vector_s))); }, time_vector);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_s, DS_ARG());                                    }, time_vector);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_s);                                               }, time_vector);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert front+10 + erase front+10 */ /* only TIMES_SEQUENCE_INSERT / 500 times */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 500, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert back-10 + erase back-10 */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) - 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) - 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_FIND times */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        s_pool_str(fixbuf);
        {
            const vector_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_s, DSL(cvector, end)(ds_vector_s), (vector_size_t)TIMES_SEQUENCE_INSERT, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_s, __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2), (vector_size_t)MID_N, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            {
                vector_size_t     s  = DSL(cvector, size)(ds_vector_s);
                vector_iterator_t b  = __cvector_it(ds_vector_s, (s - MID_N) / 2);
                vector_iterator_t e  = __cvector_it(ds_vector_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, b, e); }, time_vector);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            GET_DURATION({ removed = DSL(cvector, remove)(ds_vector_s, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), removed, DSL(cvector, size)(ds_vector_s));
        }


        /* insert_n + insert_range */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            vector_t* src = VECTOR_NEW_STR();

            DSL(cvector, insert_n)(ds_vector_s, DSL(cvector, end)(ds_vector_s), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(cvector, insert_n)(src,         DSL(cvector, end)(src),         (vector_size_t)TIMES_SEQUENCE_INSERT / 1, DS_ARG());

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_range)(ds_vector_s, 
                                                      __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2), 
                                                      __cvector_it(src, DSL(cvector, size)(src) * 0.25), 
                                                      __cvector_it(src, DSL(cvector, size)(src) * 0.75)); }, time_vector);
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            VECTOR_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            vector_t* src = VECTOR_NEW_STR();

            DSL(cvector, reserve)(ds_vector_s, (vector_size_t)TIMES_SEQUENCE_INSERT);
            DSL(cvector, insert_n)(ds_vector_s, DSL(cvector, end)(ds_vector_s), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(cvector, insert_n)(src,         DSL(cvector, end)(src),         (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2), DS_ARG());

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, assign_range)(ds_vector_s, 
                                                      __cvector_it(src, 1), 
                                                      __cvector_it(src, (vector_size_t)TIMES_SEQUENCE_INSERT / 2 - 1)); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, assign_range)(ds_vector_s, 
                                                      __cvector_it(src, 1), 
                                                      __cvector_it(src, DSL(cvector, size)(src) - 1)); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(cvector, size)(src) - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            VECTOR_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            vector_t* src = VECTOR_NEW_STR();

            DSL(cvector, reserve)(ds_vector_s, (vector_size_t)TIMES_SEQUENCE_INSERT);
            DSL(cvector, insert_n)(ds_vector_s, DSL(cvector, end)(ds_vector_s), (vector_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, assign)(ds_vector_s, (vector_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, DS_ARG()); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, assign)(ds_vector_s, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, DS_ARG()); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            VECTOR_DELETE(&src);
        }


        /* replace_it + replace_index */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

            time_vector = 0;
            {
                vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cvector_replace_it(ds_vector_s, it, DS_ARG()); }, time_vector);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __cvector_replace_index(ds_vector_s, rand() % DSL(cvector, size)(ds_vector_s), DS_ARG()); }, time_vector);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));
        }


        /* resize grow + shrink */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        s_pool_str(fixbuf);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, resize)(ds_vector_s, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2), (vector_data_t)fixbuf); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, resize)(ds_vector_s, (vector_size_t)TIMES_SEQUENCE_INSERT / 2, (vector_data_t)fixbuf); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));
        }


        /* remove_if */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

            time_vector = 0;
            {
                vector_size_t removed;

                GET_DURATION({ removed = DSL(cvector, remove_if)(ds_vector_s, vector_remove_if_char_odd); }, time_vector);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), removed, DSL(cvector, size)(ds_vector_s));
            }
        }


        /* shrink_to_fit */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            DSL(cvector, reserve)(ds_vector_s, (vector_size_t)(TIMES_SEQUENCE_INSERT * 1.2));
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, shrink_to_fit)(ds_vector_s); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s shrink_to_fit    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));
        }


        /* reserve */ /* vector only */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        {
            time_vector = 0;
            DS_CAP(ds_vector_s);
            GET_DURATION({ DSL(cvector, reserve)(ds_vector_s, (vector_size_t)TIMES_SEQUENCE_INSERT); }, time_vector);
            DS_CAP(ds_vector_s);
            printf("RESULT %-11s reserve          %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), DSL(cvector, size)(ds_vector_s));
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        {
            volatile vector_data_t sink = 0;
            vector_iterator_t      it   = DSL(cvector, begin)(ds_vector_s);
            vector_iterator_t      end  = DSL(cvector, end)(ds_vector_s);

            time_vector = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cvector, next)(it)) { sink += (vector_data_t)(uintptr_t)*it.sd; }, time_vector);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }

        {
            volatile vector_data_t sink = 0;
            vector_r_iterator_t    it   = DSL(cvector, rbegin)(ds_vector_s);
            vector_r_iterator_t    end  = DSL(cvector, rend)(ds_vector_s);

            time_vector = 0;
            GET_DURATION(for (; it.cur != end.cur; it = DSL(cvector, rnext)(it)) { sink += (vector_data_t)(uintptr_t)*it.sd; }, time_vector);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + __at + __it */
        {
            volatile vector_data_t sink     = 0;
            vector_size_t          max_size = 0;
            bool                   is_empty = false;
            vector_iterator_t      mid      = DSL(cvector, end)(ds_vector_s);

            time_vector = 0;
            GET_DURATION({ max_size = DSL(cvector, max_size)(ds_vector_s); is_empty = __cvector_empty(ds_vector_s); }, time_vector);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_vector / 1000), max_size, is_empty);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = (vector_data_t)(uintptr_t)*DSL(cvector, front)(ds_vector_s).sd; sink = (vector_data_t)(uintptr_t)*DSL(cvector, back)(ds_vector_s).sd; }, time_vector);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { sink = (vector_data_t)(uintptr_t)*__cvector_at(ds_vector_s, rand() % DSL(cvector, size)(ds_vector_s)).sd; }, time_vector);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { mid = __cvector_it(ds_vector_s, rand() % DSL(cvector, size)(ds_vector_s)); sink += (vector_data_t)(uintptr_t)*mid.sd; }, time_vector);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_vector / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort_vt_str(DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)); }, time_vector);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_vector / 1000),
               sort_vt_str_sorted(DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)),
               DSL(cvector, size)(ds_vector_s));

        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort_vt_sdes(DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)); }, time_vector);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_vector / 1000),
               sort_vt_sdes_sorted(DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)),
               DSL(cvector, size)(ds_vector_s));

#undef DS_CAP
#undef DS_ARG
    }
#endif

#ifdef TEST_LIST
    if (PERFORMANCE_SEQUENCE_EXTRA)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((list_data_t)s_pool_str(dst))

        /* count */
        {
            list_count_t cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_FIND; ++i) { cnt += DSL(clist, count)(ds_list_s, DS_ARG()); }, time_list);
            printf("RESULT %-11s count            %s %10zd %6zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_FIND, (ssize_t)(time_list / 1000), cnt, DSL(clist, size)(ds_list_s));
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ DSL(clist, erase_range)(ds_list_s, DSL(clist, next)(DSL(clist, begin)(ds_list_s)), DSL(clist, prev)(DSL(clist, end)(ds_list_s))); }, time_list);
        printf("RESULT %-11s erase_range      %s %10zd %6zd ms\n", __func__, DS_NAME, TIMES_SEQUENCE_INSERT - DSL(clist, size)(ds_list_s), (ssize_t)(time_list / 1000));
        GET_DURATION({ DSL(clist, erase_range)(ds_list_s, DSL(clist, begin)(ds_list_s), DSL(clist, end)(ds_list_s)); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, insert)(ds_list_s, DSL(clist, begin)(ds_list_s), DS_ARG());       }, time_list);
        printf("RESULT %-11s insert@front     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, erase)(ds_list_s, DSL(clist, begin)(ds_list_s));                  }, time_list);
        printf("RESULT %-11s erase@front      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, push_front)(ds_list_s, DS_ARG());                                   }, time_list);
        printf("RESULT %-11s push_front       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, pop_front)(ds_list_s);                                              }, time_list);
        printf("RESULT %-11s pop_front        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, insert)(ds_list_s, DSL(clist, end)(ds_list_s), DS_ARG());         }, time_list);
        printf("RESULT %-11s insert@back      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, erase)(ds_list_s, DSL(clist, prev)(DSL(clist, end)(ds_list_s))); }, time_list);
        printf("RESULT %-11s erase@back       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, push_back)(ds_list_s, DS_ARG());                                    }, time_list);
        printf("RESULT %-11s push_back        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { DSL(clist, pop_back)(ds_list_s);                                               }, time_list);
        printf("RESULT %-11s pop_back         %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert front+10 + erase front+10 */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_s, it, DS_ARG()); }, time_list);
        }
        printf("RESULT %-11s insert@+10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_s, it); }, time_list);
        }
        printf("RESULT %-11s erase@+10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert back-10 + erase back-10 */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) - 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_s, it, DS_ARG()); }, time_list);
        }
        printf("RESULT %-11s insert@-10       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) - 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_s, it); }, time_list);
        }
        printf("RESULT %-11s erase@-10        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert mid + erase mid */ /* only TIMES_SEQUENCE_INSERT / 5 times */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());
        {
            list_iterator_t mid = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 5; ++i) { DSL(clist, insert)(ds_list_s, mid, DS_ARG()); }, time_list);
            printf("RESULT %-11s insert@mid       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 5, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }

        {
            list_iterator_t mid = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT / 5; ++i) { mid = DSL(clist, erase)(ds_list_s, mid); }, time_list);
            printf("RESULT %-11s erase@mid        %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 5, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            const list_data_t MID_N = TIMES_SEQUENCE_INSERT / 5;
            ds_size_t removed;

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_s, DSL(clist, end)(ds_list_s), (list_size_t)TIMES_SEQUENCE_INSERT, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s insert_n@all     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_s, __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2), (list_size_t)MID_N, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s insert_n@mid     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            {
                list_size_t     s  = DSL(clist, size)(ds_list_s);
                list_iterator_t b  = __clist_it(ds_list_s, (s - MID_N) / 2);
                list_iterator_t e  = __clist_it(ds_list_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(clist, erase_range)(ds_list_s, b, e); }, time_list);
            }
            printf("RESULT %-11s erase_range@mid  %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ removed = DSL(clist, remove)(ds_list_s, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s remove@all       %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_s));
        }


        /* insert_n + insert_range */
        DSL(clist, clear)(ds_list_s);
        {
            list_t* src = LIST_NEW_STR();

            DSL(clist, insert_n)(ds_list_s, DSL(clist, end)(ds_list_s), (list_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(clist, insert_n)(src,       DSL(clist, end)(src),       (list_size_t)TIMES_SEQUENCE_INSERT / 1, DS_ARG());

            {
                list_iterator_t pos   = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);
                list_iterator_t begin = __clist_it(src, DSL(clist, size)(src) * 0.25);
                list_iterator_t end   = __clist_it(src, DSL(clist, size)(src) * 0.75);

                time_list = 0;
                GET_DURATION({ DSL(clist, insert_range)(ds_list_s, pos, begin, end); }, time_list);
            }
            printf("RESULT %-11s insert_range@mid %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            LIST_DELETE(&src);
        }


        /* insert_n + assign_range */
        DSL(clist, clear)(ds_list_s);
        {
            list_t* src = LIST_NEW_STR();

            DSL(clist, insert_n)(ds_list_s, DSL(clist, end)(ds_list_s), (list_size_t)TIMES_SEQUENCE_INSERT / 2, DS_ARG());
            DSL(clist, insert_n)(src,       DSL(clist, end)(src),       (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2), DS_ARG());

            {
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, (list_size_t)TIMES_SEQUENCE_INSERT / 2 - 1);

                time_list = 0;
                GET_DURATION({ DSL(clist, assign_range)(ds_list_s, begin, end); }, time_list);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            {
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, DSL(clist, size)(src) - 1);

                time_list = 0;
                GET_DURATION({ DSL(clist, assign_range)(ds_list_s, begin, end); }, time_list);
            }
            printf("RESULT %-11s assign_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, DSL(clist, size)(src) - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            LIST_DELETE(&src);
        }


        /* insert_n + assign */
        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            DSL(clist, insert_n)(ds_list_s, DSL(clist, end)(ds_list_s), (list_size_t)TIMES_SEQUENCE_INSERT / 2, (list_data_t)fixbuf);

            time_list = 0;
            GET_DURATION({ DSL(clist, assign)(ds_list_s, (list_size_t)TIMES_SEQUENCE_INSERT / 2 - 2, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2 - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ DSL(clist, assign)(ds_list_s, (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s assign           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2) - 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* replace_it + replace_index */
        DSL(clist, clear)(ds_list_s);
        {
            const int TIMES_INDEX = TIMES_SEQUENCE_FIND / 20;

            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

            time_list = 0;
            {
                list_iterator_t it = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);

                GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) { __clist_replace_it(ds_list_s, it, DS_ARG()); }, time_list);
            }
            printf("RESULT %-11s replace_it       %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { __clist_replace_index(ds_list_s, rand() % DSL(clist, size)(ds_list_s), DS_ARG()); }, time_list);
            printf("RESULT %-11s replace_index    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* resize grow + shrink */
        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

            time_list = 0;
            GET_DURATION({ DSL(clist, resize)(ds_list_s, (list_size_t)(TIMES_SEQUENCE_INSERT * 1.2), (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s resize@grow      %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(TIMES_SEQUENCE_INSERT * 1.2), (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ DSL(clist, resize)(ds_list_s, (list_size_t)TIMES_SEQUENCE_INSERT / 2, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %-11s resize@shrink    %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* remove_if */
        DSL(clist, clear)(ds_list_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, remove_if)(ds_list_s, list_remove_if_char_odd); }, time_list);
                printf("RESULT %-11s remove_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_s));
            }
        }


        /* unique + unique_if */
        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, (list_data_t)fixbuf);

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, unique)(ds_list_s); }, time_list);
                printf("RESULT %-11s unique           %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_s));
            }
        }

        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, (list_data_t)fixbuf);

            time_list = 0;
            {
                list_size_t removed;

                GET_DURATION({ removed = DSL(clist, unique_if)(ds_list_s, __ds_ops_eq_default_string); }, time_list);
                printf("RESULT %-11s unique_if        %s %10zd %6zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), removed, DSL(clist, size)(ds_list_s));
            }
        }


        /* reverse */
        DSL(clist, clear)(ds_list_s);
        {
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

            time_list = 0;
            GET_DURATION({ DSL(clist, reverse)(ds_list_s); }, time_list);
            printf("RESULT %-11s reverse          %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* splice */
        DSL(clist, clear)(ds_list_s);
        {
            list_t* src = LIST_NEW_STR();

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());
            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(src, DS_ARG());

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
                if (i & 1)
                    DSL(clist, splice)(src, DSL(clist, end)(src), ds_list_s);
                else
                    DSL(clist, splice)(ds_list_s, DSL(clist, end)(ds_list_s), src);
            }, time_list);
            printf("RESULT %-11s splice           %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s) + DSL(clist, size)(src));

            LIST_DELETE(&src);
        }


        /* splice_range */
        DSL(clist, clear)(ds_list_s);
        {
            list_t*     src = LIST_NEW_STR();
            list_size_t n   = (list_size_t)TIMES_SEQUENCE_INSERT / 2;

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());
            for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(src, DS_ARG());

            {
                list_iterator_t pos   = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);
                list_iterator_t begin = __clist_it(src, 1);
                list_iterator_t end   = __clist_it(src, n);

                time_list = 0;
                GET_DURATION({ DSL(clist, splice_range)(ds_list_s, pos, src, begin, end); }, time_list);
            }
            printf("RESULT %-11s splice_range     %s %10zd %6zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)(n - 1), (ssize_t)(time_list / 1000), DSL(clist, size)(ds_list_s));

            LIST_DELETE(&src);
        }


        /* merge */
        DSL(clist, clear)(ds_list_s);
        {
            list_t* src = LIST_NEW_STR();

            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());
            for (int i = 0; i < TIMES_SEQUENCE_INSERT / 2; ++i) DSL(clist, push_back)(src, DS_ARG());
            DSL(clist, sort)(ds_list_s, list_lt_str);
            DSL(clist, sort)(src, list_lt_str);

            time_list = 0;
            GET_DURATION({ DSL(clist, merge)(ds_list_s, src, list_lt_str); }, time_list);
            printf("RESULT %-11s merge            %s %10zd %6zd ms sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT / 2, (ssize_t)(time_list / 1000), list_sorted_str(ds_list_s, true), DSL(clist, size)(ds_list_s));

            LIST_DELETE(&src);
        }


        /* iteration */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        {
            volatile list_data_t sink = 0;
            list_iterator_t      it   = DSL(clist, begin)(ds_list_s);
            list_iterator_t      end  = DSL(clist, end)(ds_list_s);

            time_list = 0;
            GET_DURATION(for (; it.d != end.d; it = DSL(clist, next)(it)) { sink += (list_data_t)(uintptr_t)*it.sd; }, time_list);
            printf("RESULT %-11s iterate@fwd      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }

        {
            volatile list_data_t sink = 0;
            list_r_iterator_t    it   = DSL(clist, rbegin)(ds_list_s);
            list_r_iterator_t    end  = DSL(clist, rend)(ds_list_s);

            time_list = 0;
            GET_DURATION(for (; it.d != end.d; it = DSL(clist, rnext)(it)) { sink += (list_data_t)(uintptr_t)*it.sd; }, time_list);
            printf("RESULT %-11s iterate@rev      %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }


        /* max_size + empty + front/back + at + it */
        {
            const int            TIMES_INDEX = TIMES_SEQUENCE_FIND / 20;
            volatile list_data_t sink     = 0;
            list_size_t          max_size = 0;
            bool                 is_empty = false;
            list_iterator_t      mid      = DSL(clist, end)(ds_list_s);

            time_list = 0;
            GET_DURATION({ max_size = DSL(clist, max_size)(ds_list_s); is_empty = __clist_empty(ds_list_s); }, time_list);
            printf("RESULT %-11s max_size         %s %10zd %6zd ms max_size=%zd empty=%d\n", __func__, DS_NAME, (ssize_t)1, (ssize_t)(time_list / 1000), max_size, is_empty);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) {
                sink = (list_data_t)(uintptr_t)*DSL(clist, front)(ds_list_s).sd;
                sink = (list_data_t)(uintptr_t)*DSL(clist, back)(ds_list_s).sd;
            }, time_list);
            printf("RESULT %-11s front_back       %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT, (ssize_t)(time_list / 1000), (ssize_t)sink);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { sink = (list_data_t)(uintptr_t)*__clist_at(ds_list_s, rand() % DSL(clist, size)(ds_list_s)).sd; }, time_list);
            printf("RESULT %-11s __at             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), (ssize_t)sink);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INDEX; ++i) { mid = __clist_it(ds_list_s, rand() % DSL(clist, size)(ds_list_s)); sink += (list_data_t)(uintptr_t)*mid.sd; }, time_list);
            printf("RESULT %-11s __it             %s %10zd %6zd ms sink=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INDEX, (ssize_t)(time_list / 1000), (ssize_t)sink);
        }


        /* sort */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_s, list_lt_str); }, time_list);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_list / 1000),
               list_sorted_str(ds_list_s, true),
               DSL(clist, size)(ds_list_s));

        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_s); for (int i = 0; i < TIMES_SEQUENCE_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_s, list_gt_str); }, time_list);
        printf("RESULT %-11s sort             %s %10zd %6zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_SEQUENCE_INSERT,
               (ssize_t)(time_list / 1000),
               list_sorted_str(ds_list_s, false),
               DSL(clist, size)(ds_list_s));
#undef DS_ARG
    }
#endif

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_stack    = 0;
        time_queue    = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        ds_size_t removed = 0;

#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_s);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_s);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_s);               }, time_map);
        MAP_DELETE(&ds_map_s);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_s);               }, time_set);
        SET_DELETE(&ds_set_s);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_s);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_s);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_s);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_s);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_s);             }, time_list);
        LIST_DELETE(&ds_list_s);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_s);         }, time_vector);
        VECTOR_DELETE(&ds_vector_s);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_s);           }, time_deque);
        DEQUE_DELETE(&ds_deque_s);
#elif TEST_STACK
        {
            char dst[S_STR_LEN_MAX + 1];
            for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
                DSL(cstack, push)(ds_stack_s, (stack_data_t)s_pool_str(dst));
        }
        GET_DURATION({
            removed = DSL(cstack, clear)(ds_stack_s);
            STACK_DELETE(&ds_stack_s);
        }, time_stack);
#elif TEST_QUEUE
        {
            char dst[S_STR_LEN_MAX + 1];
            for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
                DSL(cqueue, push)(ds_queue_s, (queue_data_t)s_pool_str(dst));
        }
        GET_DURATION({
            removed = DSL(cqueue, clear)(ds_queue_s);
            QUEUE_DELETE(&ds_queue_s);
        }, time_queue);
#elif TEST_PQUEUE
        {
            char dst[S_STR_LEN_MAX + 1];
            for (int i = 0; i < TIMES_ADAPTER_INSERT; ++i)
                DSL(cpqueue, push)(ds_pqueue_s, (priority_queue_data_t)s_pool_str(dst));
        }
        GET_DURATION({
            removed = DSL(cpqueue, clear)(ds_pqueue_s);
            PRIORITY_QUEUE_DELETE(&ds_pqueue_s);
        }, time_pqueue);
#endif

        printf("RESULT %-11s deinit           %s %10zd %6zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)INSERT_OPS, (ssize_t)(TIME_DS / 1000), removed);
    }
}

int main(int argc, char** argv)
{
    int n;

    if (argc <= 1) {
        test_i_for();
        malloc_trim(0);
        sleep(1);
        test_i_rand();
        malloc_trim(0);
        sleep(1);
        test_s_rand();
    } else if (1 == (n = atoi(argv[1])))
        test_i_for();
    else if (2 == n)
        test_i_rand();
    else if (3 == n)
        test_s_rand();

    return 0;
}
