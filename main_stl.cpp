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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

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
        ds_deque_i.push_back(TIMES_INSERT + 1);
        ds_deque_i.push_back(TIMES_INSERT + 2);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.insert(--ds_deque_i.end(), i);      }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_pqueue_i.push(i);          }, time_pqueue);
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
        GET_DURATION({ sort(ds_vector_i.begin(), ds_vector_i.end()); }, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
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

        printf("RESULT %s remove %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        ds_size_t n;

        time_deque = 0;
        n = ds_deque_i.size();
        GET_DURATION({ ds_deque_i.erase(ds_deque_i.begin(), ds_deque_i.end()); }, time_deque);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_back(i);  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_front(); }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_front(i); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_back(); }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        ds_size_t n;

        time_list = 0;
        n = ds_list_i.size();
        GET_DURATION({ ds_list_i.erase(ds_list_i.begin(), ds_list_i.end()); }, time_list);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_back(i);  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_front(); }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_front(i); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_back(); }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        ds_size_t n;

        time_vector = 0;
        n = ds_vector_i.size();
        GET_DURATION({ ds_vector_i.erase(ds_vector_i.begin(), ds_vector_i.end()); }, time_vector);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.push_back(i);  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_i.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.pop_back(); }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_i.size());
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

        printf("RESULT %s deinit %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

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
        ds_deque_i.push_back(TIMES_FIND + 1);
        ds_deque_i.push_back(TIMES_FIND + 2);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_deque_i.insert(--ds_deque_i.end(), rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            ds_pqueue_i.push(rand() % TIMES_FIND);
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
        GET_DURATION(sort(ds_vector_i.begin(), ds_vector_i.end());, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
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

        printf("RESULT %s remove %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        ds_size_t n;

        time_deque = 0;
        n = ds_deque_i.size();
        GET_DURATION({ ds_deque_i.erase(ds_deque_i.begin(), ds_deque_i.end()); }, time_deque);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_back(rand() % TIMES_FIND);  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_front(); }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.push_front(rand() % TIMES_FIND); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_i.pop_back(); }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_i.size());
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        ds_size_t n;

        time_list = 0;
        n = ds_list_i.size();
        GET_DURATION({ ds_list_i.erase(ds_list_i.begin(), ds_list_i.end()); }, time_list);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_back(rand() % TIMES_FIND);  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_front(); }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.push_front(rand() % TIMES_FIND); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_i.pop_back(); }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_i.size());
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        ds_size_t n;

        time_vector = 0;
        n = ds_vector_i.size();
        GET_DURATION({ ds_vector_i.erase(ds_vector_i.begin(), ds_vector_i.end()); }, time_vector);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.push_back(rand() % TIMES_FIND);  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_i.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_i.pop_back(); }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_i.size());
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

        printf("RESULT %s deinit %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
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

    g_s_pool = (char*)malloc(S_POOL_LEN);
    if (!g_s_pool) {
        fprintf(stderr, "%s: pool malloc fail\n", __func__);
        exit(1);
    }
    for (int i = 0; i < S_POOL_LEN; ++i)
        g_s_pool[i] = charset[rand() % (int)(sizeof(charset) - 1)];
}

static void pool_assign(string& key)
{
    int len   = S_STR_LEN_MIN + rand() % (S_STR_LEN_MAX - S_STR_LEN_MIN + 1); /* 16..31 */
    int index = rand() % (S_POOL_LEN - S_STR_LEN_MAX + 1);                    /* index+len ≤ S_POOL_LEN */

    key.assign(g_s_pool + index, (size_t)len);
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
        time_vector_s = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        string key;
        key.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_hashmap_s.emplace(key, i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_map_s.emplace(key, i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_set_s.emplace(key);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_multimap_s.emplace(key, i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_multiset_s.emplace(key);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_list_s.push_back(key);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_vector_s.push_back(key);
        }, time_vector);
#elif TEST_DEQUE
        pool_assign(key); ds_deque_s.push_back(key);
        pool_assign(key); ds_deque_s.push_back(key);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_deque_s.insert(--ds_deque_s.end(), key);
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            pool_assign(key);
            ds_pqueue_s.push(key);
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
        ds_size_t ds_size = 0;

        string key;
        key.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        ds_size = ds_hashmap_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            pool_assign(key);
            auto it = ds_hashmap_s.find(key);
            if (it != ds_hashmap_s.end()) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = ds_map_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            pool_assign(key);
            auto it = ds_map_s.find(key);
            if (it != ds_map_s.end()) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = ds_set_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            pool_assign(key);
            auto it = ds_set_s.find(key);
            if (it != ds_set_s.end()) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = ds_multimap_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            pool_assign(key);
            auto it = ds_multimap_s.find(key);
            if (it != ds_multimap_s.end()) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = ds_multiset_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            pool_assign(key);
            auto it = ds_multiset_s.find(key);
            if (it != ds_multiset_s.end()) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = ds_list_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            pool_assign(key);
            auto it = find(ds_list_s.begin(), ds_list_s.end(), key);
            if (it != ds_list_s.end()) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = ds_vector_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            pool_assign(key);
            auto it = find(ds_vector_s.begin(), ds_vector_s.end(), key);
            if (it != ds_vector_s.end()) times_succ++;
        }, time_vector);
        GET_DURATION({ sort(ds_vector_s.begin(), ds_vector_s.end()); }, time_vector_s);
        printf("RESULT %s sort %s %ld %ld ms\n", __func__, DS_NAME, (long)TIMES_FIND_V_L, (long)(time_vector_s / 1000));
#elif TEST_DEQUE
        ds_size = ds_deque_s.size();
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            pool_assign(key);
            auto it = find(ds_deque_s.begin(), ds_deque_s.end(), key);
            if (it != ds_deque_s.end()) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = ds_pqueue_s.size();
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

        size_t removed = 0;

        string key;
        key.reserve(S_STR_LEN_MAX + 1);

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            pool_assign(key);
            removed += ds_hashmap_s.erase(key);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            pool_assign(key);
            removed += ds_map_s.erase(key);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            pool_assign(key);
            removed += ds_set_s.erase(key);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            pool_assign(key);
            removed += ds_multimap_s.erase(key);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            pool_assign(key);
            removed += ds_multiset_s.erase(key);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_list_s.size();
            pool_assign(key);
            ds_list_s.remove(key);
            removed += curr - ds_list_s.size();
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_vector_s.size();
            pool_assign(key);
            ds_vector_s.erase(remove(ds_vector_s.begin(), ds_vector_s.end(), key),
                              ds_vector_s.end());
            removed += curr - ds_vector_s.size();
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            auto curr = ds_deque_s.size();
            pool_assign(key);
            ds_deque_s.erase(remove(ds_deque_s.begin(), ds_deque_s.end(), key),
                             ds_deque_s.end());
            removed += curr - ds_deque_s.size();
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)REMOVE_OPS, (long)(TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        ds_size_t n;
        string key;
        key.reserve(S_STR_LEN_MAX + 1);

        time_deque = 0;
        n = ds_deque_s.size();
        GET_DURATION({ ds_deque_s.erase(ds_deque_s.begin(), ds_deque_s.end()); }, time_deque);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_deque / 1000));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { pool_assign(key); ds_deque_s.push_back(key);  }, time_deque);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.pop_front(); }, time_deque);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { pool_assign(key); ds_deque_s.push_front(key); }, time_deque);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_s.size());

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_deque_s.pop_back(); }, time_deque);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_deque / 1000), (long)ds_deque_s.size());
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        ds_size_t n;
        string key;
        key.reserve(S_STR_LEN_MAX + 1);

        time_list = 0;
        n = ds_list_s.size();
        GET_DURATION({ ds_list_s.erase(ds_list_s.begin(), ds_list_s.end()); }, time_list);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_list / 1000));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { pool_assign(key); ds_list_s.push_back(key);  }, time_list);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.pop_front(); }, time_list);
        printf("RESULT %s pop_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { pool_assign(key); ds_list_s.push_front(key); }, time_list);
        printf("RESULT %s push_front %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_s.size());

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_list_s.pop_back(); }, time_list);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_list / 1000), (long)ds_list_s.size());
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        ds_size_t n;
        string key;
        key.reserve(S_STR_LEN_MAX + 1);

        time_vector = 0;
        n = ds_vector_s.size();
        GET_DURATION({ ds_vector_s.erase(ds_vector_s.begin(), ds_vector_s.end()); }, time_vector);
        printf("RESULT %s erase %s %ld %ld ms\n", __func__, DS_NAME, (long)n, (long)(time_vector / 1000));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { pool_assign(key); ds_vector_s.push_back(key);  }, time_vector);
        printf("RESULT %s push_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_s.size());

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_vector_s.pop_back(); }, time_vector);
        printf("RESULT %s pop_back %s %ld %ld ms size=%ld\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(time_vector / 1000), (long)ds_vector_s.size());
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

        printf("RESULT %s deinit %s %ld %ld ms removed=%zu\n", __func__, DS_NAME, (long)TIMES_INSERT, (long)(TIME_DS / 1000), removed);
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
