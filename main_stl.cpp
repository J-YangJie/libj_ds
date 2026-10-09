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
#include <unistd.h>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <string>
#include <algorithm>
#include <cstdint>

using namespace std;

typedef intptr_t ds_data_t;
typedef ssize_t  ds_size_t;

#define GET_DURATION(_data, _time) do { gettimeofday(&time_begin, NULL); _data gettimeofday(&time_end, NULL); \
                                        _time += time_end.tv_usec - time_begin.tv_usec + 1000000 * (time_end.tv_sec - time_begin.tv_sec); } while (0)

//#define TEST_HASHMAP        1
//#define TEST_MAP            1
//#define TEST_SET            1
//#define TEST_MULTIMAP       1
//#define TEST_MULTISET       1
//#define TEST_LIST           1
//#define TEST_VECTOR         1
//#define TEST_DEQUE          1
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

#ifndef SORT_SEED
#define SORT_SEED 12345
#endif /* SORT_SEED */

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
                            (void)time_vector; (void)time_pqueue; \
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
    clock_t time_deque    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    unordered_map<ds_data_t, ds_data_t> ds_hashmap_i;
    ds_hashmap_i.reserve(HASHMAP_CAPACITY_INIT);
    printf("Unordered_map reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map<ds_data_t, ds_data_t> ds_map_i;
#elif TEST_SET
    set<ds_data_t> ds_set_i;
#elif TEST_MULTIMAP
    multimap<ds_data_t, ds_data_t> ds_multimap_i;
#elif TEST_MULTISET
    multiset<ds_data_t> ds_multiset_i;
#elif TEST_LIST
    list<ds_data_t> ds_list_i;
#elif TEST_VECTOR
    vector<ds_data_t> ds_vector_i;
#elif TEST_DEQUE
    deque<ds_data_t> ds_deque_i;
#elif TEST_PQUEUE
    priority_queue<ds_data_t> ds_pqueue_i;
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
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_hashmap_i.insert({i, i});  }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_map_i.insert({i, i});      }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_set_i.insert(i);           }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_multimap_i.insert({i, i}); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_multiset_i.insert(i);      }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_back(i);       }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.push_back(i);     }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_back(i);      }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_pqueue_i.push(i);          }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
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
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size = 0;
#ifdef TEST_HASHMAP
        ds_size = ds_hashmap_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_hashmap_i.find(i);
            if (it != ds_hashmap_i.end()) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = ds_map_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_map_i.find(i);
            if (it != ds_map_i.end()) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = ds_set_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_set_i.find(i);
            if (it != ds_set_i.end()) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = ds_multimap_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multimap_i.find(i);
            if (it != ds_multimap_i.end()) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = ds_multiset_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multiset_i.find(i);
            if (it != ds_multiset_i.end()) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = ds_list_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_list_i.begin(), ds_list_i.end(), i);
            if (it != ds_list_i.end()) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = ds_vector_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_vector_i.begin(), ds_vector_i.end(), i);
            if (it != ds_vector_i.end()) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = ds_deque_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_deque_i.begin(), ds_deque_i.end(), i);
            if (it != ds_deque_i.end()) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = ds_pqueue_i.size();
        /* unsupport */
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;

        size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_hashmap_i.erase(rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_map_i.erase(rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_set_i.erase(rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multimap_i.erase(rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multiset_i.erase(rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_list_i.size();
            ds_list_i.remove(rand() % TIMES_FIND);
            removed += curr - ds_list_i.size();
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_vector_i.size();
            ds_vector_i.erase(  remove(ds_vector_i.begin(),
                                        ds_vector_i.end(),
                                        rand() % TIMES_FIND),
                                ds_vector_i.end());
            removed += curr - ds_vector_i.size();
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_deque_i.size();
            ds_deque_i.erase(  remove(ds_deque_i.begin(),
                                      ds_deque_i.end(),
                                      rand() % TIMES_FIND),
                              ds_deque_i.end());
            removed += curr - ds_deque_i.size();
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
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
        time_pqueue   = 0;

        size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = ds_hashmap_i.size();  ds_hashmap_i.clear();  }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ removed = ds_map_i.size();      ds_map_i.clear();      }, time_map);
#elif TEST_SET
        GET_DURATION({ removed = ds_set_i.size();      ds_set_i.clear();      }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = ds_multimap_i.size(); ds_multimap_i.clear(); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ removed = ds_multiset_i.size(); ds_multiset_i.clear(); }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ removed = ds_list_i.size();     ds_list_i.clear();     }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ removed = ds_vector_i.size();   ds_vector_i.clear();   }, time_vector);
#elif TEST_DEQUE
        GET_DURATION({ removed = ds_deque_i.size();    ds_deque_i.clear();    }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_i.size();
            priority_queue<ds_data_t>().swap(ds_pqueue_i);
        }, time_pqueue);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
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
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    unordered_map<ds_data_t, ds_data_t> ds_hashmap_i;
    ds_hashmap_i.reserve(HASHMAP_CAPACITY_INIT);
    printf("Unordered_map reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map<ds_data_t, ds_data_t> ds_map_i;
#elif TEST_SET
    set<ds_data_t> ds_set_i;
#elif TEST_MULTIMAP
    multimap<ds_data_t, ds_data_t> ds_multimap_i;
#elif TEST_MULTISET
    multiset<ds_data_t> ds_multiset_i;
#elif TEST_LIST
    list<ds_data_t> ds_list_i;
#elif TEST_VECTOR
    vector<ds_data_t> ds_vector_i;
#elif TEST_DEQUE
    deque<ds_data_t> ds_deque_i;
#elif TEST_PQUEUE
    priority_queue<ds_data_t> ds_pqueue_i;
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
        time_deque    = 0;
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_hashmap_i.insert({rand() % TIMES_FIND, i});
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_map_i.insert({rand() % TIMES_FIND, i});
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_set_i.insert(rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_multimap_i.insert({rand() % TIMES_FIND, i});
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_multiset_i.insert(rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_list_i.push_back(rand() % TIMES_FIND);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_vector_i.push_back(rand() % TIMES_FIND);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_deque_i.push_back(rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_pqueue_i.push(rand() % TIMES_FIND);
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
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
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size = 0;
#ifdef TEST_HASHMAP
        ds_size = ds_hashmap_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_hashmap_i.find(rand() % TIMES_FIND);
            if (it != ds_hashmap_i.end()) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = ds_map_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_map_i.find(rand() % TIMES_FIND);
            if (it != ds_map_i.end()) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = ds_set_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_set_i.find(rand() % TIMES_FIND);
            if (it != ds_set_i.end()) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = ds_multimap_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multimap_i.find(rand() % TIMES_FIND);
            if (it != ds_multimap_i.end()) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = ds_multiset_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multiset_i.find(rand() % TIMES_FIND);
            if (it != ds_multiset_i.end()) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = ds_list_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_list_i.begin(), ds_list_i.end(), rand() % TIMES_FIND);
            if (it != ds_list_i.end()) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = ds_vector_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_vector_i.begin(), ds_vector_i.end(), rand() % TIMES_FIND);
            if (it != ds_vector_i.end()) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = ds_deque_i.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_deque_i.begin(), ds_deque_i.end(), rand() % TIMES_FIND);
            if (it != ds_deque_i.end()) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = ds_pqueue_i.size();
        /* unsupport */
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;

        size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_hashmap_i.erase(rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_map_i.erase(rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_set_i.erase(rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multimap_i.erase(rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multiset_i.erase(rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_list_i.size();
            ds_list_i.remove(rand() % TIMES_FIND);
            removed += curr - ds_list_i.size();
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_vector_i.size();
            ds_vector_i.erase(  remove(ds_vector_i.begin(),
                                        ds_vector_i.end(),
                                        rand() % TIMES_FIND),
                                ds_vector_i.end());
            removed += curr - ds_vector_i.size();
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_deque_i.size();
            ds_deque_i.erase(  remove(ds_deque_i.begin(),
                                      ds_deque_i.end(),
                                      rand() % TIMES_FIND),
                              ds_deque_i.end());
            removed += curr - ds_deque_i.size();
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        /* count */
        {
            long cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_deque_i.begin(), ds_deque_i.end(), rand() % TIMES_FIND); }, time_deque);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), cnt, ds_deque_i.size());
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ ds_deque_i.erase(ds_deque_i.begin() + 1, ds_deque_i.end() - 1); }, time_deque);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, (TIMES_INSERT - ds_deque_i.size()), (time_deque / 1000));
        GET_DURATION({ ds_deque_i.erase(ds_deque_i.begin(), ds_deque_i.end()); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.insert(ds_deque_i.begin(), rand() % TIMES_FIND); }, time_deque);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.erase(ds_deque_i.begin());                       }, time_deque);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_front(rand() % TIMES_FIND);                 }, time_deque);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_front();                                     }, time_deque);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.insert(ds_deque_i.end(), rand() % TIMES_FIND);   }, time_deque);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.erase(ds_deque_i.end() - 1);                     }, time_deque);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_back(rand() % TIMES_FIND);                  }, time_deque);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_back();                                      }, time_deque);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());


        /* insert front+10 + erase front+10 */
        ds_deque_i.clear();
        for (int i = 0; i < 20; ++i) ds_deque_i.push_back(rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_i.begin() + 10;
            GET_DURATION({ ds_deque_i.insert(it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_i.begin() + 10;
            GET_DURATION({ ds_deque_i.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());


        /* insert back-10 + erase back-10 */
        ds_deque_i.clear();
        for (int i = 0; i < 20; ++i) ds_deque_i.push_back(rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_i.begin() + (ds_deque_i.size() - 10);
            GET_DURATION({ ds_deque_i.insert(it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_i.begin() + (ds_deque_i.size() - 10);
            GET_DURATION({ ds_deque_i.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());


        /* insert mid + erase mid */
        ds_deque_i.clear();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_i.push_back(rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_deque_i.begin() + (ds_deque_i.size() / 2);
            GET_DURATION({ ds_deque_i.insert(it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), ds_deque_i.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_deque_i.begin() + (ds_deque_i.size() / 2);
            GET_DURATION({ ds_deque_i.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), ds_deque_i.size());


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_deque_i.clear();
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_deque = 0;
            GET_DURATION({ ds_deque_i.insert(ds_deque_i.end(), (size_t)TIMES_INSERT, 9); }, time_deque);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_i.size());

            time_deque = 0;
            GET_DURATION({ ds_deque_i.insert(ds_deque_i.begin() + (ds_deque_i.size() / 2), (size_t)MID_N, 9); }, time_deque);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_deque / 1000), ds_deque_i.size());

            time_deque = 0;
            {
                size_t s = ds_deque_i.size();
                auto   b = ds_deque_i.begin() + ((s - (size_t)MID_N) / 2);
                auto   e = ds_deque_i.begin() + ((s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_deque_i.erase(b, e); }, time_deque);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_deque / 1000), ds_deque_i.size());

            time_deque = 0;
            {
                size_t curr = ds_deque_i.size();
                GET_DURATION({ ds_deque_i.erase(remove(ds_deque_i.begin(), ds_deque_i.end(), 9), ds_deque_i.end()); }, time_deque);
                removed = (ds_size_t)(curr - ds_deque_i.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), removed, ds_deque_i.size());
        }


        /* sort */
        srand(SORT_SEED);
        ds_deque_i.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_i.push_back((ds_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort(ds_deque_i.begin(), ds_deque_i.end()); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000), (int)is_sorted(ds_deque_i.begin(), ds_deque_i.end()), ds_deque_i.size());

        srand(SORT_SEED);
        ds_deque_i.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_i.push_back((ds_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort(ds_deque_i.begin(), ds_deque_i.end(), greater<ds_data_t>()); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000), (int)is_sorted(ds_deque_i.begin(), ds_deque_i.end(), greater<ds_data_t>()), ds_deque_i.size());
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        /* count */
        {
            long cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_vector_i.begin(), ds_vector_i.end(), rand() % TIMES_FIND); }, time_vector);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), cnt, ds_vector_i.size());
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ ds_vector_i.erase(ds_vector_i.begin() + 1, ds_vector_i.end() - 1); }, time_vector);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, (TIMES_INSERT - ds_vector_i.size()), (time_vector / 1000));
        GET_DURATION({ ds_vector_i.erase(ds_vector_i.begin(), ds_vector_i.end()); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_INSERT / 500 times */
        time_vector = 0;
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { ds_vector_i.insert(ds_vector_i.begin(), rand() % TIMES_FIND); }, time_vector);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { ds_vector_i.erase(ds_vector_i.begin());                       }, time_vector);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_i.size());


        /* insert/push back + erase/pop back */
        time_vector = 0;
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.insert(ds_vector_i.end(), rand() % TIMES_FIND);   }, time_vector);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.erase(ds_vector_i.end() - 1);                     }, time_vector);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.push_back(rand() % TIMES_FIND);                  }, time_vector);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.pop_back();                                     }, time_vector);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());


        /* insert front+10 + erase front+10 */ /* only TIMES_INSERT / 500 times */
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        for (int i = 0; i < 20; ++i) ds_vector_i.push_back(rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            auto it = ds_vector_i.begin() + 10;
            GET_DURATION({ ds_vector_i.insert(it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            auto it = ds_vector_i.begin() + 10;
            GET_DURATION({ ds_vector_i.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_i.size());


        /* insert back-10 + erase back-10 */
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        for (int i = 0; i < 20; ++i) ds_vector_i.push_back(rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_vector_i.begin() + (ds_vector_i.size() - 10);
            GET_DURATION({ ds_vector_i.insert(it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_vector_i.begin() + (ds_vector_i.size() - 10);
            GET_DURATION({ ds_vector_i.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_i.push_back(rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_vector_i.begin() + (ds_vector_i.size() / 2);
            GET_DURATION({ ds_vector_i.insert(it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), ds_vector_i.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_vector_i.begin() + (ds_vector_i.size() / 2);
            GET_DURATION({ ds_vector_i.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), ds_vector_i.size());


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit();
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_vector = 0;
            GET_DURATION({ ds_vector_i.insert(ds_vector_i.end(), (size_t)TIMES_INSERT, 9); }, time_vector);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_i.size());

            time_vector = 0;
            GET_DURATION({ ds_vector_i.insert(ds_vector_i.begin() + (ds_vector_i.size() / 2), (size_t)MID_N, 9); }, time_vector);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_vector / 1000), ds_vector_i.size());

            time_vector = 0;
            {
                size_t s = ds_vector_i.size();
                auto   b = ds_vector_i.begin() + ((s - (size_t)MID_N) / 2);
                auto   e = ds_vector_i.begin() + ((s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_vector_i.erase(b, e); }, time_vector);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_vector / 1000), ds_vector_i.size());

            time_vector = 0;
            {
                size_t curr = ds_vector_i.size();
                GET_DURATION({ ds_vector_i.erase(remove(ds_vector_i.begin(), ds_vector_i.end(), 9), ds_vector_i.end()); }, time_vector);
                removed = (ds_size_t)(curr - ds_vector_i.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), removed, ds_vector_i.size());
        }


        /* sort */
        srand(SORT_SEED);
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit(); for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_i.push_back((ds_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort(ds_vector_i.begin(), ds_vector_i.end()); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000), (int)is_sorted(ds_vector_i.begin(), ds_vector_i.end()), ds_vector_i.size());

        srand(SORT_SEED);
        ds_vector_i.clear(); ds_vector_i.shrink_to_fit(); for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_i.push_back((ds_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort(ds_vector_i.begin(), ds_vector_i.end(), greater<ds_data_t>()); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000), (int)is_sorted(ds_vector_i.begin(), ds_vector_i.end(), greater<ds_data_t>()), ds_vector_i.size());
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        /* count */
        {
            long cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_list_i.begin(), ds_list_i.end(), rand() % TIMES_FIND); }, time_list);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_list / 1000), cnt, ds_list_i.size());
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ ds_list_i.erase(next(ds_list_i.begin()), prev(ds_list_i.end())); }, time_list);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - ds_list_i.size(), (time_list / 1000));
        GET_DURATION({ ds_list_i.erase(ds_list_i.begin(), ds_list_i.end()); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.insert(ds_list_i.begin(), rand() % TIMES_FIND); }, time_list);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.erase(ds_list_i.begin());                       }, time_list);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_front(rand() % TIMES_FIND);               }, time_list);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_front();                                  }, time_list);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.insert(ds_list_i.end(), rand() % TIMES_FIND);   }, time_list);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.erase(prev(ds_list_i.end()));                  }, time_list);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_back(rand() % TIMES_FIND);                }, time_list);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_back();                                   }, time_list);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());


        /* insert front+10 + erase front+10 */
        ds_list_i.clear();
        for (int i = 0; i < 20; ++i) ds_list_i.push_back(rand() % TIMES_FIND);

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = next(ds_list_i.begin(), 10);
            GET_DURATION({ ds_list_i.insert(it, rand() % TIMES_FIND); }, time_list);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = next(ds_list_i.begin(), 10);
            GET_DURATION({ ds_list_i.erase(it); }, time_list);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());


        /* insert back-10 + erase back-10 */
        ds_list_i.clear();
        for (int i = 0; i < 20; ++i) ds_list_i.push_back(rand() % TIMES_FIND);

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = prev(ds_list_i.end(), 10);
            GET_DURATION({ ds_list_i.insert(it, rand() % TIMES_FIND); }, time_list);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = prev(ds_list_i.end(), 10);
            GET_DURATION({ ds_list_i.erase(it); }, time_list);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());


        /* insert mid + erase mid */ /* only TIMES_INSERT / 5 times */
        /* 和 main.c 的 list 块一致：std::list 的 it(n) 同样是 O(n)，所以中点只定位一次、
           循环里复用（list 的 insert 不让别的迭代器失效，erase 返回下一个）。 */
        ds_list_i.clear();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_list_i.push_back(rand() % TIMES_FIND);
        {
            auto mid = next(ds_list_i.begin(), ds_list_i.size() / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { ds_list_i.insert(mid, rand() % TIMES_FIND); }, time_list);
            printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), ds_list_i.size());
        }

        {
            auto mid = next(ds_list_i.begin(), ds_list_i.size() / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { mid = ds_list_i.erase(mid); }, time_list);
            printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), ds_list_i.size());
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_list_i.clear();
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_list = 0;
            GET_DURATION({ ds_list_i.insert(ds_list_i.end(), (size_t)TIMES_INSERT, 9); }, time_list);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_i.size());

            time_list = 0;
            GET_DURATION({ ds_list_i.insert(next(ds_list_i.begin(), ds_list_i.size() / 2), (size_t)MID_N, 9); }, time_list);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_list / 1000), ds_list_i.size());

            time_list = 0;
            {
                size_t s = ds_list_i.size();
                auto   b = next(ds_list_i.begin(), (s - (size_t)MID_N) / 2);
                auto   e = next(ds_list_i.begin(), (s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_list_i.erase(b, e); }, time_list);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_list / 1000), ds_list_i.size());

            time_list = 0;
            {
                size_t curr = ds_list_i.size();
                GET_DURATION({ ds_list_i.remove(9); }, time_list);
                removed = (ds_size_t)(curr - ds_list_i.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), removed, ds_list_i.size());
        }


        /* sort: 升序 —— std::list 的成员 sort（归并、只改链），对应 main.c 的 clist_sort(l, LIST_SORT_ASC) */
        srand(SORT_SEED);
        ds_list_i.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_list_i.push_back((ds_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ ds_list_i.sort(); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000), (int)is_sorted(ds_list_i.begin(), ds_list_i.end()), ds_list_i.size());

        /* sort: 降序 —— 对应 main.c 的 clist_sort(l, LIST_SORT_DESC) */
        srand(SORT_SEED);
        ds_list_i.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_list_i.push_back((ds_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ ds_list_i.sort(greater<ds_data_t>()); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000), (int)is_sorted(ds_list_i.begin(), ds_list_i.end(), greater<ds_data_t>()), ds_list_i.size());
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
        time_pqueue   = 0;

        size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = ds_hashmap_i.size();  ds_hashmap_i.clear();  }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ removed = ds_map_i.size();      ds_map_i.clear();      }, time_map);
#elif TEST_SET
        GET_DURATION({ removed = ds_set_i.size();      ds_set_i.clear();      }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = ds_multimap_i.size(); ds_multimap_i.clear(); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ removed = ds_multiset_i.size(); ds_multiset_i.clear(); }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ removed = ds_list_i.size();     ds_list_i.clear();     }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ removed = ds_vector_i.size();   ds_vector_i.clear();   }, time_vector);
#elif TEST_DEQUE
        GET_DURATION({ removed = ds_deque_i.size();    ds_deque_i.clear();    }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_i.size();
            priority_queue<ds_data_t>().swap(ds_pqueue_i);
        }, time_pqueue);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
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

    g_s_pool = (char*)malloc(S_POOL_LEN);
    if (!g_s_pool) {
        fprintf(stderr, "%s: pool malloc fail\n", __func__);
        exit(1);
    }
    for (int i = 0; i < S_POOL_LEN; ++i)
        g_s_pool[i] = charset[rand() % (int)(sizeof(charset) - 1)];
}

static string& s_pool_str(string& dst)
{
    int len   = S_STR_LEN_MIN + rand() % (S_STR_LEN_MAX - S_STR_LEN_MIN + 1); /* 16..31 */
    int index = rand() % (S_POOL_LEN - S_STR_LEN_MAX + 1);                    /* index+len ≤ S_POOL_LEN */

    dst.assign(g_s_pool + index, (size_t)len);
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
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    unordered_map<string, ds_data_t> ds_hashmap_s;
    ds_hashmap_s.reserve(HASHMAP_CAPACITY_INIT);
    printf("Unordered_map reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map<string, ds_data_t> ds_map_s;
#elif TEST_SET
    set<string> ds_set_s;
#elif TEST_MULTIMAP
    multimap<string, ds_data_t> ds_multimap_s;
#elif TEST_MULTISET
    multiset<string> ds_multiset_s;
#elif TEST_LIST
    list<string> ds_list_s;
#elif TEST_VECTOR
    vector<string> ds_vector_s;
#elif TEST_DEQUE
    deque<string> ds_deque_s;
#elif TEST_PQUEUE
    priority_queue<string> ds_pqueue_s;
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
        time_deque    = 0;
        time_pqueue   = 0;

        string dst;
        dst.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_hashmap_s.emplace(s_pool_str(dst), i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_map_s.emplace(s_pool_str(dst), i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_set_s.emplace(s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_multimap_s.emplace(s_pool_str(dst), i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_multiset_s.emplace(s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_list_s.push_back(s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_vector_s.push_back(s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_deque_s.push_back(s_pool_str(dst));
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_pqueue_s.push(s_pool_str(dst));
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
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
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size = 0;

        string dst;
        dst.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        ds_size = ds_hashmap_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_hashmap_s.find(s_pool_str(dst));
            if (it != ds_hashmap_s.end()) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = ds_map_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_map_s.find(s_pool_str(dst));
            if (it != ds_map_s.end()) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = ds_set_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_set_s.find(s_pool_str(dst));
            if (it != ds_set_s.end()) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = ds_multimap_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multimap_s.find(s_pool_str(dst));
            if (it != ds_multimap_s.end()) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = ds_multiset_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            auto it = ds_multiset_s.find(s_pool_str(dst));
            if (it != ds_multiset_s.end()) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = ds_list_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_list_s.begin(), ds_list_s.end(), s_pool_str(dst));
            if (it != ds_list_s.end()) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = ds_vector_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_vector_s.begin(), ds_vector_s.end(), s_pool_str(dst));
            if (it != ds_vector_s.end()) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = ds_deque_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = find(ds_deque_s.begin(), ds_deque_s.end(), s_pool_str(dst));
            if (it != ds_deque_s.end()) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = ds_pqueue_s.size();
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
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
        time_pqueue   = 0;

        size_t removed = 0;

        string dst;
        dst.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_hashmap_s.erase(s_pool_str(dst));
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_map_s.erase(s_pool_str(dst));
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_set_s.erase(s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multimap_s.erase(s_pool_str(dst));
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += ds_multiset_s.erase(s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_list_s.size();
            ds_list_s.remove(s_pool_str(dst));
            removed += curr - ds_list_s.size();
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_vector_s.size();
            ds_vector_s.erase(remove(ds_vector_s.begin(), ds_vector_s.end(), s_pool_str(dst)),
                              ds_vector_s.end());
            removed += curr - ds_vector_s.size();
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_deque_s.size();
            ds_deque_s.erase(remove(ds_deque_s.begin(), ds_deque_s.end(), s_pool_str(dst)),
                             ds_deque_s.end());
            removed += curr - ds_deque_s.size();
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        string dst;
        string fixbuf;
        dst.reserve(S_STR_LEN_MAX + 1);
        fixbuf.reserve(S_STR_LEN_MAX + 1);

#define DS_ARG()   (s_pool_str(dst))

        /* count */
        {
            long cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_deque_s.begin(), ds_deque_s.end(), DS_ARG()); }, time_deque);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), cnt, ds_deque_s.size());
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ ds_deque_s.erase(ds_deque_s.begin() + 1, ds_deque_s.end() - 1); }, time_deque);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, (TIMES_INSERT - ds_deque_s.size()), (time_deque / 1000));
        GET_DURATION({ ds_deque_s.erase(ds_deque_s.begin(), ds_deque_s.end()); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.insert(ds_deque_s.begin(), DS_ARG());       }, time_deque);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.erase(ds_deque_s.begin());                  }, time_deque);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.push_front(DS_ARG());                       }, time_deque);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.pop_front();                                }, time_deque);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.insert(ds_deque_s.end(), DS_ARG());         }, time_deque);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.erase(ds_deque_s.end() - 1);                }, time_deque);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.push_back(DS_ARG());                        }, time_deque);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.pop_back();                                 }, time_deque);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());


        /* insert front+10 + erase front+10 */
        ds_deque_s.clear();
        for (int i = 0; i < 20; ++i) ds_deque_s.push_back(DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_s.begin() + 10;
            GET_DURATION({ ds_deque_s.insert(it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_s.begin() + 10;
            GET_DURATION({ ds_deque_s.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());


        /* insert back-10 + erase back-10 */
        ds_deque_s.clear();
        for (int i = 0; i < 20; ++i) ds_deque_s.push_back(DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_s.begin() + (ds_deque_s.size() - 10);
            GET_DURATION({ ds_deque_s.insert(it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_deque_s.begin() + (ds_deque_s.size() - 10);
            GET_DURATION({ ds_deque_s.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());


        /* insert mid + erase mid */
        ds_deque_s.clear();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_s.push_back(DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_deque_s.begin() + (ds_deque_s.size() / 2);
            GET_DURATION({ ds_deque_s.insert(it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), ds_deque_s.size());

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_deque_s.begin() + (ds_deque_s.size() / 2);
            GET_DURATION({ ds_deque_s.erase(it); }, time_deque);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), ds_deque_s.size());


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_deque_s.clear();
        s_pool_str(fixbuf);
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_deque = 0;
            GET_DURATION({ ds_deque_s.insert(ds_deque_s.end(), (size_t)TIMES_INSERT, fixbuf); }, time_deque);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), ds_deque_s.size());

            time_deque = 0;
            GET_DURATION({ ds_deque_s.insert(ds_deque_s.begin() + (ds_deque_s.size() / 2), (size_t)MID_N, fixbuf); }, time_deque);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_deque / 1000), ds_deque_s.size());

            time_deque = 0;
            {
                size_t s = ds_deque_s.size();
                auto   b = ds_deque_s.begin() + ((s - (size_t)MID_N) / 2);
                auto   e = ds_deque_s.begin() + ((s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_deque_s.erase(b, e); }, time_deque);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_deque / 1000), ds_deque_s.size());

            time_deque = 0;
            {
                size_t curr = ds_deque_s.size();
                GET_DURATION({ ds_deque_s.erase(remove(ds_deque_s.begin(), ds_deque_s.end(), fixbuf), ds_deque_s.end()); }, time_deque);
                removed = (ds_size_t)(curr - ds_deque_s.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), removed, ds_deque_s.size());
        }


        /* sort */
        srand(SORT_SEED);
        ds_deque_s.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_s.push_back(DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort(ds_deque_s.begin(), ds_deque_s.end()); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000), (int)is_sorted(ds_deque_s.begin(), ds_deque_s.end()), ds_deque_s.size());

        srand(SORT_SEED);
        ds_deque_s.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_deque_s.push_back(DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort(ds_deque_s.begin(), ds_deque_s.end(), greater<string>()); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000), (int)is_sorted(ds_deque_s.begin(), ds_deque_s.end(), greater<string>()), ds_deque_s.size());

#undef DS_ARG
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        string dst;
        string fixbuf;
        dst.reserve(S_STR_LEN_MAX + 1);
        fixbuf.reserve(S_STR_LEN_MAX + 1);

#define DS_ARG()   (s_pool_str(dst))

        /* count */
        {
            long cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_vector_s.begin(), ds_vector_s.end(), DS_ARG()); }, time_vector);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), cnt, ds_vector_s.size());
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ ds_vector_s.erase(ds_vector_s.begin() + 1, ds_vector_s.end() - 1); }, time_vector);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, (TIMES_INSERT - ds_vector_s.size()), (time_vector / 1000));
        GET_DURATION({ ds_vector_s.erase(ds_vector_s.begin(), ds_vector_s.end()); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_INSERT / 500 times */
        time_vector = 0;
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { ds_vector_s.insert(ds_vector_s.begin(), DS_ARG());       }, time_vector);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { ds_vector_s.erase(ds_vector_s.begin());                  }, time_vector);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_s.size());


        /* insert/push back + erase/pop back */
        time_vector = 0;
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_s.insert(ds_vector_s.end(), DS_ARG());         }, time_vector);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_s.erase(ds_vector_s.end() - 1);                }, time_vector);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_s.push_back(DS_ARG());                        }, time_vector);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_s.pop_back();                                 }, time_vector);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());


        /* insert front+10 + erase front+10 */ /* only TIMES_INSERT / 500 times */
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        for (int i = 0; i < 20; ++i) ds_vector_s.push_back(DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            auto it = ds_vector_s.begin() + 10;
            GET_DURATION({ ds_vector_s.insert(it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            auto it = ds_vector_s.begin() + 10;
            GET_DURATION({ ds_vector_s.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), ds_vector_s.size());


        /* insert back-10 + erase back-10 */
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        for (int i = 0; i < 20; ++i) ds_vector_s.push_back(DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_vector_s.begin() + (ds_vector_s.size() - 10);
            GET_DURATION({ ds_vector_s.insert(it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = ds_vector_s.begin() + (ds_vector_s.size() - 10);
            GET_DURATION({ ds_vector_s.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_s.push_back(DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_vector_s.begin() + (ds_vector_s.size() / 2);
            GET_DURATION({ ds_vector_s.insert(it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), ds_vector_s.size());

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            auto it = ds_vector_s.begin() + (ds_vector_s.size() / 2);
            GET_DURATION({ ds_vector_s.erase(it); }, time_vector);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), ds_vector_s.size());


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit();
        s_pool_str(fixbuf);
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_vector = 0;
            GET_DURATION({ ds_vector_s.insert(ds_vector_s.end(), (size_t)TIMES_INSERT, fixbuf); }, time_vector);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), ds_vector_s.size());

            time_vector = 0;
            GET_DURATION({ ds_vector_s.insert(ds_vector_s.begin() + (ds_vector_s.size() / 2), (size_t)MID_N, fixbuf); }, time_vector);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_vector / 1000), ds_vector_s.size());

            time_vector = 0;
            {
                size_t s = ds_vector_s.size();
                auto   b = ds_vector_s.begin() + ((s - (size_t)MID_N) / 2);
                auto   e = ds_vector_s.begin() + ((s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_vector_s.erase(b, e); }, time_vector);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_vector / 1000), ds_vector_s.size());

            time_vector = 0;
            {
                size_t curr = ds_vector_s.size();
                GET_DURATION({ ds_vector_s.erase(remove(ds_vector_s.begin(), ds_vector_s.end(), fixbuf), ds_vector_s.end()); }, time_vector);
                removed = (ds_size_t)(curr - ds_vector_s.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), removed, ds_vector_s.size());
        }


        /* sort */
        srand(SORT_SEED);
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit(); for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_s.push_back(DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort(ds_vector_s.begin(), ds_vector_s.end()); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000), (int)is_sorted(ds_vector_s.begin(), ds_vector_s.end()), ds_vector_s.size());

        srand(SORT_SEED);
        ds_vector_s.clear(); ds_vector_s.shrink_to_fit(); for (int i = 0; i < TIMES_INSERT; ++i) ds_vector_s.push_back(DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort(ds_vector_s.begin(), ds_vector_s.end(), greater<string>()); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000), (int)is_sorted(ds_vector_s.begin(), ds_vector_s.end(), greater<string>()), ds_vector_s.size());

#undef DS_ARG
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        string dst;
        string fixbuf;
        dst.reserve(S_STR_LEN_MAX + 1);
        fixbuf.reserve(S_STR_LEN_MAX + 1);

#define DS_ARG()   (s_pool_str(dst))

        /* count */
        {
            long cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += count(ds_list_s.begin(), ds_list_s.end(), DS_ARG()); }, time_list);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_list / 1000), cnt, ds_list_s.size());
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ ds_list_s.erase(next(ds_list_s.begin()), prev(ds_list_s.end())); }, time_list);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - ds_list_s.size(), (time_list / 1000));
        GET_DURATION({ ds_list_s.erase(ds_list_s.begin(), ds_list_s.end()); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.insert(ds_list_s.begin(), DS_ARG());       }, time_list);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.erase(ds_list_s.begin());                  }, time_list);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.push_front(DS_ARG());                       }, time_list);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.pop_front();                                }, time_list);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.insert(ds_list_s.end(), DS_ARG());         }, time_list);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.erase(prev(ds_list_s.end()));              }, time_list);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.push_back(DS_ARG());                        }, time_list);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.pop_back();                                 }, time_list);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());


        /* insert front+10 + erase front+10 */
        ds_list_s.clear();
        for (int i = 0; i < 20; ++i) ds_list_s.push_back(DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = next(ds_list_s.begin(), 10);
            GET_DURATION({ ds_list_s.insert(it, DS_ARG()); }, time_list);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = next(ds_list_s.begin(), 10);
            GET_DURATION({ ds_list_s.erase(it); }, time_list);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());


        /* insert back-10 + erase back-10 */
        ds_list_s.clear();
        for (int i = 0; i < 20; ++i) ds_list_s.push_back(DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = prev(ds_list_s.end(), 10);
            GET_DURATION({ ds_list_s.insert(it, DS_ARG()); }, time_list);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            auto it = prev(ds_list_s.end(), 10);
            GET_DURATION({ ds_list_s.erase(it); }, time_list);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());


        /* insert mid + erase mid */ /* only TIMES_INSERT / 5 times */
        /* 同 main.c 的 list 块：中点只定位一次，循环里复用。 */
        ds_list_s.clear();
        for (int i = 0; i < TIMES_INSERT; ++i) ds_list_s.push_back(DS_ARG());
        {
            auto mid = next(ds_list_s.begin(), ds_list_s.size() / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { ds_list_s.insert(mid, DS_ARG()); }, time_list);
            printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), ds_list_s.size());
        }

        {
            auto mid = next(ds_list_s.begin(), ds_list_s.size() / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { mid = ds_list_s.erase(mid); }, time_list);
            printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), ds_list_s.size());
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        ds_list_s.clear();
        s_pool_str(fixbuf);
        {
            const long MID_N = TIMES_INSERT / 5;
            ds_size_t  removed;

            time_list = 0;
            GET_DURATION({ ds_list_s.insert(ds_list_s.end(), (size_t)TIMES_INSERT, fixbuf); }, time_list);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), ds_list_s.size());

            time_list = 0;
            GET_DURATION({ ds_list_s.insert(next(ds_list_s.begin(), ds_list_s.size() / 2), (size_t)MID_N, fixbuf); }, time_list);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_list / 1000), ds_list_s.size());

            time_list = 0;
            {
                size_t s = ds_list_s.size();
                auto   b = next(ds_list_s.begin(), (s - (size_t)MID_N) / 2);
                auto   e = next(ds_list_s.begin(), (s + (size_t)MID_N) / 2);
                GET_DURATION({ ds_list_s.erase(b, e); }, time_list);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, MID_N, (time_list / 1000), ds_list_s.size());

            time_list = 0;
            {
                size_t curr = ds_list_s.size();
                GET_DURATION({ ds_list_s.remove(fixbuf); }, time_list);
                removed = (ds_size_t)(curr - ds_list_s.size());
            }
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), removed, ds_list_s.size());
        }


        /* sort: 升序 —— std::list 的成员 sort（归并、只改链），对应 main.c 的 clist_sort(l, list_sso_lt) */
        srand(SORT_SEED);
        ds_list_s.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_list_s.push_back(DS_ARG());

        time_list = 0;
        GET_DURATION({ ds_list_s.sort(); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000), (int)is_sorted(ds_list_s.begin(), ds_list_s.end()), ds_list_s.size());

        /* sort: 降序 —— 对应 main.c 的 clist_sort(l, list_sso_gt) */
        srand(SORT_SEED);
        ds_list_s.clear(); for (int i = 0; i < TIMES_INSERT; ++i) ds_list_s.push_back(DS_ARG());

        time_list = 0;
        GET_DURATION({ ds_list_s.sort(greater<string>()); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000), (int)is_sorted(ds_list_s.begin(), ds_list_s.end(), greater<string>()), ds_list_s.size());

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
        time_deque    = 0;
        time_pqueue   = 0;

        size_t removed = 0;

#ifdef TEST_HASHMAP
        GET_DURATION({ removed = ds_hashmap_s.size();  ds_hashmap_s.clear();  }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ removed = ds_map_s.size();      ds_map_s.clear();      }, time_map);
#elif TEST_SET
        GET_DURATION({ removed = ds_set_s.size();      ds_set_s.clear();      }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = ds_multimap_s.size(); ds_multimap_s.clear(); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ removed = ds_multiset_s.size(); ds_multiset_s.clear(); }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ removed = ds_list_s.size();     ds_list_s.clear();     }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ removed = ds_vector_s.size();   ds_vector_s.clear();   }, time_vector);
#elif TEST_DEQUE
        GET_DURATION({ removed = ds_deque_s.size();    ds_deque_s.clear();    }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_s.size();
            priority_queue<string>().swap(ds_pqueue_s);
        }, time_pqueue);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zu\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
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
