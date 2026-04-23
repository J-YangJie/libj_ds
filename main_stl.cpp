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
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <vector>
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
    clock_t time_pqueue   = 0;

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
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { ds_pqueue_i.push(i);          }, time_pqueue);
#endif

        printf("Insert  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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
#elif TEST_PQUEUE
        ds_size = ds_pqueue_i.size();
        /* unsupport */
#endif

        printf("Find    [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tsucc [ %zu ], ds_size [ %zd ]\n", 
                TIMES_FIND / pow(10, (int)log10(TIMES_FIND)),
                (int)log10(TIMES_FIND),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000,
                times_succ, ds_size);
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
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("Remove  [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zu ]\n", 
                TIMES_REMOVE / pow(10, (int)log10(TIMES_REMOVE)),
                (int)log10(TIMES_REMOVE),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                removed);
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
        time_vector_s = 0;
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
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_i.size(); 
            priority_queue<ds_data_t>().swap(ds_pqueue_i); 
        }, time_pqueue);
#endif

        printf("Deinit  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zu ]\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000,
                removed);
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
    clock_t time_pqueue   = 0;

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
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            ds_pqueue_i.push(rand() % TIMES_FIND); 
        }, time_pqueue);
#endif

        printf("Insert  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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
#elif TEST_PQUEUE
        ds_size = ds_pqueue_i.size();
        /* unsupport */
#endif

        printf("Find    [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tsucc [ %zu ], ds_size [ %zd ]\n", 
                TIMES_FIND / pow(10, (int)log10(TIMES_FIND)),
                (int)log10(TIMES_FIND),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000,
                times_succ, ds_size);
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
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("Remove  [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zu ]\n", 
                TIMES_REMOVE / pow(10, (int)log10(TIMES_REMOVE)),
                (int)log10(TIMES_REMOVE),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                removed);
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
        time_vector_s = 0;
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
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_i.size(); 
            priority_queue<ds_data_t>().swap(ds_pqueue_i); 
        }, time_pqueue);
#endif

        printf("Deinit  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zu ]\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000,
                removed);
    }
}

static void string_rand(char* buf, int len)
{
    const char tstr[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \
                         ~!@#$%^&*()_+{}|:<>?~@#￥%……&*（）——+{}|：”《》？杨杰";

    if (len <= 0)
        return ;

    --len;
    for (int i = 0; i < len; ++i)
        buf[i] = tstr[rand() % (int)(sizeof(tstr) - 1)];
    buf[len] = '\0';
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
    clock_t time_pqueue   = 0;

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
#elif TEST_PQUEUE
    priority_queue<string> ds_pqueue_s;
#endif

    srand(time(0));
    printf("%s\n", __func__);

#define NUM_BUF 100000
#define NUM_KEY_LEN 12
    char buffer[NUM_BUF][NUM_KEY_LEN];

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

        for (int s = 0; s < TIMES_INSERT / NUM_BUF; ++s) {
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand();
                len1 = trand % (NUM_KEY_LEN + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_hashmap_s.insert({buffer[i], i}); 
            }, time_hashmap);
#elif TEST_MAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_map_s.insert({buffer[i], i}); 
            }, time_map);
#elif TEST_SET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_set_s.insert(buffer[i]); 
            }, time_set);
#elif TEST_MULTIMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_multimap_s.insert({buffer[i], i}); 
            }, time_multimap);
#elif TEST_MULTISET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_multiset_s.insert(buffer[i]); 
            }, time_multiset);
#elif TEST_LIST
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_list_s.push_back(buffer[i]); 
            }, time_list);
#elif TEST_VECTOR
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_vector_s.push_back(buffer[i]); 
            }, time_vector);
#elif TEST_PQUEUE
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                ds_pqueue_s.push(buffer[i]); 
            }, time_pqueue);
#endif
        }

        printf("SInsert [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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

        size_t times_succ = 0;
        ds_size_t ds_size = 0;

#if defined(TEST_LIST) || defined(TEST_VECTOR)
        {
#else
        for (int s = 0; s < TIMES_FIND / NUM_BUF; ++s) {
#endif /* defined(TEST_LIST) || defined(TEST_VECTOR) */
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand();
                len1 = trand % (NUM_KEY_LEN + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            ds_size = ds_hashmap_s.size();
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                auto it = ds_hashmap_s.find(buffer[rand() % NUM_BUF]); 
                if (it != ds_hashmap_s.end()) times_succ++; 
            }, time_hashmap);
#elif TEST_MAP
            ds_size = ds_map_s.size();
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                auto it = ds_map_s.find(buffer[rand() % NUM_BUF]); 
                if (it != ds_map_s.end()) times_succ++; 
            }, time_map);
#elif TEST_SET
            ds_size = ds_set_s.size();
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                auto it = ds_set_s.find(buffer[rand() % NUM_BUF]); 
                if (it != ds_set_s.end()) times_succ++; 
            }, time_set);
#elif TEST_MULTIMAP
            ds_size = ds_multimap_s.size();
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                auto it = ds_multimap_s.find(buffer[rand() % NUM_BUF]); 
                if (it != ds_multimap_s.end()) times_succ++; 
            }, time_multimap);
#elif TEST_MULTISET
            ds_size = ds_multiset_s.size();
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                auto it = ds_multiset_s.find(buffer[rand() % NUM_BUF]); 
                if (it != ds_multiset_s.end()) times_succ++; 
            }, time_multiset);
#elif TEST_LIST
            ds_size = ds_list_s.size();
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
                auto it = find(ds_list_s.begin(), ds_list_s.end(), buffer[rand() % NUM_BUF]);
                if (it != ds_list_s.end()) times_succ++; 
            }, time_list);
#elif TEST_VECTOR
            ds_size = ds_vector_s.size();
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
                auto it = find(ds_vector_s.begin(), ds_vector_s.end(), buffer[rand() % NUM_BUF]); 
                if (it != ds_vector_s.end()) times_succ++; 
            }, time_vector);
            GET_DURATION({ sort(ds_vector_s.begin(), ds_vector_s.end()); }, time_vector_s);
#elif TEST_PQUEUE
            ds_size = ds_pqueue_s.size();
            /* unsupport */
#endif
        }

        printf("SFind   [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tsucc [ %zu ], ds_size [ %zd ]\n", 
                TIMES_FIND / pow(10, (int)log10(TIMES_FIND)),
                (int)log10(TIMES_FIND),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                TIMES_FIND_V_L / pow(10, (int)log10(TIMES_FIND_V_L)),
                (int)log10(TIMES_FIND_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000,
                times_succ, ds_size);
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

        size_t removed = 0;

#if defined(TEST_LIST) || defined(TEST_VECTOR)
        {
#else
        for (int s = 0; s < TIMES_REMOVE / NUM_BUF; ++s) {
#endif /* defined(TEST_LIST) || defined(TEST_VECTOR) */
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand();
                len1 = trand % (NUM_KEY_LEN + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += ds_hashmap_s.erase(buffer[rand() % NUM_BUF]); 
            }, time_hashmap);
#elif TEST_MAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += ds_map_s.erase(buffer[rand() % NUM_BUF]); 
            }, time_map);
#elif TEST_SET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += ds_set_s.erase(buffer[rand() % NUM_BUF]); 
            }, time_set);
#elif TEST_MULTIMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += ds_multimap_s.erase(buffer[rand() % NUM_BUF]); 
            }, time_multimap);
#elif TEST_MULTISET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += ds_multiset_s.erase(buffer[rand() % NUM_BUF]); 
            }, time_multiset);
#elif TEST_LIST
            GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
                auto curr = ds_list_s.size();
                ds_list_s.remove(buffer[rand() % NUM_BUF]);
                removed += curr - ds_list_s.size();
            }, time_list);
#elif TEST_VECTOR
            GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
                auto curr = ds_vector_s.size();
                ds_vector_s.erase(  remove(ds_vector_s.begin(), 
                                            ds_vector_s.end(), 
                                            buffer[rand() % NUM_BUF]), 
                                    ds_vector_s.end());
                removed += curr - ds_vector_s.size();
            }, time_vector);
#elif TEST_PQUEUE
            removed = 0;
            /* unsupport */
#endif
        }

        printf("SRemove [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zu ]\n", 
                TIMES_REMOVE / pow(10, (int)log10(TIMES_REMOVE)),
                (int)log10(TIMES_REMOVE),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                TIMES_REMOVE_V_L / pow(10, (int)log10(TIMES_REMOVE_V_L)),
                (int)log10(TIMES_REMOVE_V_L),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                removed);
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
        time_vector_s = 0;
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
#elif TEST_PQUEUE
        GET_DURATION({ removed = ds_pqueue_s.size(); 
            priority_queue<string>().swap(ds_pqueue_s); 
        }, time_pqueue);
#endif

        printf("SDeinit [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zu ]\n", 
                TIMES_INSERT / pow(10, (int)log10(TIMES_INSERT)),
                (int)log10(TIMES_INSERT),
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000,
                removed);
    }
}

int main(int argc, char** argv)
{
    test_i_for();
    sleep(1);
    test_i_rand();
    sleep(1);
    test_s_rand();
    return 0;
}
