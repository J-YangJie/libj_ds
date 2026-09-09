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
#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iterator/iterator_inter.h>
#include <hashmap/hashmap.h>
#include <list/list.h>
#include <vector/vector.h>
#include <deque/deque.h>
#include <priority_queue/priority_queue.h>
#include <map/map.h>
#include <multimap/multimap.h>
#include <set/set.h>
#include <multiset/multiset.h>
#include <operations/ds_ops_string.h>

#define cpqueue cpriority_queue

#define GET_DURATION(_data, _time) do { gettimeofday(&time_begin, NULL); _data gettimeofday(&time_end, NULL); \
                                        _time += time_end.tv_usec - time_begin.tv_usec + 1000000 * (time_end.tv_sec - time_begin.tv_sec); } while (0)

//#define DSL_V
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
//#define TEST_LIST           1
//#define TEST_VECTOR         1
//#define TEST_PQUEUE         1

#ifndef TIMES_INSERT
#define TIMES_INSERT   10000000
#endif /* TIMES_INSERT */

#ifndef TIMES_FIND
#define TIMES_FIND     100000000
#endif /* TIMES_FIND */
#ifndef TIMES_FIND_V_L
#define TIMES_FIND_V_L 100
#endif /* TIMES_FIND_V_L */

#ifndef TIMES_REMOVE
#define TIMES_REMOVE   50000000
#endif /* TIMES_REMOVE */
#ifndef TIMES_REMOVE_V_L
#define TIMES_REMOVE_V_L 100
#endif /* TIMES_REMOVE_V_L */

#ifndef HASHMAP_CAPACITY_INIT
#define HASHMAP_CAPACITY_INIT 2 * TIMES_INSERT
#endif /* HASHMAP_CAPACITY_INIT */

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
#elif defined(TEST_PQUEUE)
#define DS_NAME      "priority_queue"
#define TIME_DS      time_pqueue
#else
#define DS_NAME      "unknown"
#define TIME_DS      time_vector
#endif

#if defined(TEST_LIST) || defined(TEST_VECTOR) || defined(TEST_DEQUE)
#define FIND_OPS   TIMES_FIND_V_L
#define REMOVE_OPS TIMES_REMOVE_V_L
#else
#define FIND_OPS   TIMES_FIND
#define REMOVE_OPS TIMES_REMOVE
#endif

#define TOUCH_TIMERS() do { (void)time_hashmap; (void)time_map; (void)time_set; \
                            (void)time_multimap; (void)time_multiset; (void)time_list; \
                            (void)time_vector; (void)time_vector_s; (void)time_pqueue; \
                            (void)time_deque; } while (0)

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
    clock_t time_vector_s = 0;
    clock_t time_deque    = 0;
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
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PRIORITY_QUEUE_NEW();
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(chashmap, insert)(ds_hashmap_i, i, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmap, insert)(ds_map_i, i, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cset, insert)(ds_set_i, i);              }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmultimap, insert)(ds_multimap_i, i, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmultiset, insert)(ds_multiset_i, i);    }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, i);         }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, i);     }, time_vector);
#elif TEST_DEQUE
        DSL(cdeque, push_back)(ds_deque_i, TIMES_INSERT + 1);
        DSL(cdeque, push_back)(ds_deque_i, TIMES_INSERT + 2);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i)), i);       }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cpqueue, push)(ds_pqueue_i, i);          }, time_pqueue);
#endif

        printf("RESULT %s insert %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_vector);
        GET_DURATION({ DSL(cvector, sort)(ds_vector_i, NULL); }, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        {
            deque_iterator_t e_it = DSL(cdeque, end)(ds_deque_i);
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
                deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, i);
                if (it.cur != e_it.cur) times_succ++;
            }, time_deque);
        }
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        /* unsupport */
#endif

        printf("RESULT %s find %s %ld %ld ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (long)FIND_OPS, (long)(TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;
        time_deque    = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector); /* only 10^2 */
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_FIND);
        }, time_deque);  /* only 10^2 */
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        deque_size_t n;

        time_deque = 0;
        n = DSL(cdeque, size)(ds_deque_i);
        {
            deque_iterator_t b = DSL(cdeque, begin)(ds_deque_i);
            deque_iterator_t e = DSL(cdeque, end)(ds_deque_i);
            GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, b, e); }, time_deque);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, i);  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_i);     }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_i, i); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_i);      }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        list_size_t n;

        time_list = 0;
        n = DSL(clist, size)(ds_list_i);
        {
            list_iterator_t b = DSL(clist, begin)(ds_list_i);
            list_iterator_t e = DSL(clist, end)(ds_list_i);
            GET_DURATION({ DSL(clist, erase_range)(ds_list_i, b, e); }, time_list);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, i);  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_front)(ds_list_i);     }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_front)(ds_list_i, i); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_back)(ds_list_i);      }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        vector_size_t n;

        time_vector = 0;
        n = DSL(cvector, size)(ds_vector_i);
        {
            vector_iterator_t b = DSL(cvector, begin)(ds_vector_i);
            vector_iterator_t e = DSL(cvector, end)(ds_vector_i);
            GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, b, e); }, time_vector);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, i);  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_i);      }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_i));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

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
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_i);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
#endif

        printf("RESULT %s deinit %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
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
    clock_t time_vector_s = 0;
    clock_t time_deque    = 0;
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
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PRIORITY_QUEUE_NEW();
#endif

    srand(time(0));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_i, rand() % TIMES_FIND, i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_i, rand() % TIMES_FIND, i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cset, insert)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_i, rand() % TIMES_FIND, i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector);
#elif TEST_DEQUE
        DSL(cdeque, push_back)(ds_deque_i, TIMES_FIND + 1);
        DSL(cdeque, push_back)(ds_deque_i, TIMES_FIND + 2);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i)), rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_i, rand() % TIMES_FIND);
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_vector);
        GET_DURATION({ DSL(cvector, sort)(ds_vector_i, NULL); }, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        {
            deque_iterator_t e_it = DSL(cdeque, end)(ds_deque_i);
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
                deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, rand() % TIMES_FIND);
                if (it.cur != e_it.cur) times_succ++;
            }, time_deque);
        }
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        /* unsupport */
#endif

        printf("RESULT %s find %s %ld %ld ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (long)FIND_OPS, (long)(TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;
        time_deque    = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        deque_size_t n;

        time_deque = 0;
        n = DSL(cdeque, size)(ds_deque_i);
        {
            deque_iterator_t b = DSL(cdeque, begin)(ds_deque_i);
            deque_iterator_t e = DSL(cdeque, end)(ds_deque_i);
            GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, b, e); }, time_deque);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_i);     }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_i, rand() % TIMES_FIND); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_i);      }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_i));
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        list_size_t n;

        time_list = 0;
        n = DSL(clist, size)(ds_list_i);
        {
            list_iterator_t b = DSL(clist, begin)(ds_list_i);
            list_iterator_t e = DSL(clist, end)(ds_list_i);
            GET_DURATION({ DSL(clist, erase_range)(ds_list_i, b, e); }, time_list);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_front)(ds_list_i);     }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_front)(ds_list_i, rand() % TIMES_FIND); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_back)(ds_list_i);      }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_i));
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        vector_size_t n;

        time_vector = 0;
        n = DSL(cvector, size)(ds_vector_i);
        {
            vector_iterator_t b = DSL(cvector, begin)(ds_vector_i);
            vector_iterator_t e = DSL(cvector, end)(ds_vector_i);
            GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, b, e); }, time_vector);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_i);      }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_i));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

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
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_i);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
#endif

        printf("RESULT %s deinit %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
    }
}

#define S_POOL_LEN    10000000
#define S_STR_LEN_MIN 16
#define S_STR_LEN_MAX 31

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
    clock_t time_vector_s = 0;
    clock_t time_deque    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    class_hashmap_ops_t tops_hashmap = {
        .__hash      = __ds_ops_hash_default_string,
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .__eq        = __ds_ops_eq_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MAP
    class_map_ops_t tops_map = {
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MULTIMAP
    class_multimap_ops_t tops_multimap = {
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#endif

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_s  = HASHMAP_NEW_OPS_3(&tops_hashmap, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_s      = MAP_NEW_OPS(&tops_map);
#elif TEST_SET
    set_t*            ds_set_s      = SET_NEW_STRING();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_s = MULTIMAP_NEW_OPS(&tops_multimap);
#elif TEST_MULTISET
    multiset_t*       ds_multiset_s = MULTISET_NEW_STRING();
#elif TEST_LIST
    list_t*           ds_list_s     = LIST_NEW_STRING();
#elif TEST_VECTOR
    vector_t*         ds_vector_s   = VECTOR_NEW_STRING();
#elif TEST_DEQUE
    deque_t*          ds_deque_s    = DEQUE_NEW_STRING();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_s   = PRIORITY_QUEUE_NEW_STRING();
#endif

    srand(time(0));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst), i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_s, (map_key_t)s_pool_str(dst), i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cset, insert)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_s, (multimap_key_t)s_pool_str(dst), i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        DSL(cdeque, push_back)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        DSL(cdeque, push_back)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cdeque, insert)(ds_deque_s, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_s)), (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_s, (priority_queue_data_t)s_pool_str(dst));
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000));
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_s, (map_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_s, (set_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_s, (list_data_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_s, (vector_data_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_vector);
        GET_DURATION({ DSL(cvector, sort)(ds_vector_s, __ds_ops_lt_default_string); }, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_s);
        {
            deque_iterator_t e_it = DSL(cdeque, end)(ds_deque_s);
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
                deque_iterator_t it = DSL(cdeque, find)(ds_deque_s, (deque_data_t)s_pool_str(dst));
                if (it.cur != e_it.cur) times_succ++;
            }, time_deque);
        }
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_s);
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s find %s %ld %ld ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (long)FIND_OPS, (long)(TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;
        time_deque    = 0;

        ds_size_t removed = 0;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_s, (map_key_t)s_pool_str(dst));
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s remove %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        deque_size_t n;
        char dst[S_STR_LEN_MAX + 1];

        time_deque = 0;
        n = DSL(cdeque, size)(ds_deque_s);
        {
            deque_iterator_t b = DSL(cdeque, begin)(ds_deque_s);
            deque_iterator_t e = DSL(cdeque, end)(ds_deque_s);
            GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, b, e); }, time_deque);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_s, (deque_data_t)s_pool_str(dst));  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_s);     }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_s, (deque_data_t)s_pool_str(dst)); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_s);      }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)DSL(cdeque, size)(ds_deque_s));
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        list_size_t n;
        char dst[S_STR_LEN_MAX + 1];

        time_list = 0;
        n = DSL(clist, size)(ds_list_s);
        {
            list_iterator_t b = DSL(clist, begin)(ds_list_s);
            list_iterator_t e = DSL(clist, end)(ds_list_s);
            GET_DURATION({ DSL(clist, erase_range)(ds_list_s, b, e); }, time_list);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_s, (list_data_t)s_pool_str(dst));  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_front)(ds_list_s);     }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_front)(ds_list_s, (list_data_t)s_pool_str(dst)); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_back)(ds_list_s);      }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)DSL(clist, size)(ds_list_s));
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        vector_size_t n;
        char dst[S_STR_LEN_MAX + 1];

        time_vector = 0;
        n = DSL(cvector, size)(ds_vector_s);
        {
            vector_iterator_t b = DSL(cvector, begin)(ds_vector_s);
            vector_iterator_t e = DSL(cvector, end)(ds_vector_s);
            GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, b, e); }, time_vector);
        }
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_s, (vector_data_t)s_pool_str(dst));  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_s);      }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)DSL(cvector, size)(ds_vector_s));
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
        time_vector_s = 0;
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
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_s);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_s);
#endif

        printf("RESULT %s deinit %s %ld %ld ms removed=%zd\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
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
