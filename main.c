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
#include <iterator/iterator.h>
#include <hashmap/hashmap.h>
#include <list/list.h>
#include <vector/vector.h>
#include <priority_queue/priority_queue.h>
#include <map/map.h>
#include <multimap/multimap.h>
#include <set/set.h>
#include <multiset/multiset.h>
#include <operations/ds_ops_string.h>

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
    hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t            ds_map_i      = MAP_INIT(&ds_map_i);
#elif TEST_SET
    set_t            ds_set_i      = SET_INIT(&ds_set_i);
#elif TEST_MULTIMAP
    multimap_t       ds_multimap_i = MULTIMAP_INIT(&ds_multimap_i);
#elif TEST_MULTISET
    multiset_t       ds_multiset_i = MULTISET_INIT(&ds_multiset_i);
#elif TEST_LIST
    list_t           ds_list_i     = LIST_INIT(&ds_list_i);
#elif TEST_VECTOR
    vector_t         ds_vector_i   = VECTOR_INIT(&ds_vector_i);
#elif TEST_PQUEUE
    priority_queue_t ds_pqueue_i   = PRIORITY_QUEUE_INIT(&ds_pqueue_i);
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
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { chashmap->insert(&ds_hashmap_i, i, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmap->insert(&ds_map_i, i, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cset->insert(&ds_set_i, i);              }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmultimap->insert(&ds_multimap_i, i, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmultiset->insert(&ds_multiset_i, i);    }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { clist->push_back(&ds_list_i, i);         }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cvector->push_back(&ds_vector_i, i);     }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cpriority_queue->push(&ds_pqueue_i, i);  }, time_pqueue);
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
        void* it;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = chashmap->size(&ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = chashmap->find(&ds_hashmap_i, i);   
            if (it && iterator_end() != it) times_succ++; 
        }, time_hashmap);
#elif TEST_MAP
        ds_size = cmap->size(&ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmap->find(&ds_map_i, i);           
            if (it && iterator_end() != it) times_succ++; 
        }, time_map);
#elif TEST_SET
        ds_size = cset->size(&ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cset->find(&ds_set_i, i);           
            if (it && iterator_end() != it) times_succ++; 
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = cmultimap->size(&ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmultimap->find(&ds_multimap_i, i); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = cmultiset->size(&ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmultiset->find(&ds_multiset_i, i); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_multiset);
#elif TEST_LIST
        ds_size = clist->size(&ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
            it = clist->find(&ds_list_i, i);     
            if (it && iterator_end() != it) times_succ++; 
        }, time_list);
#elif TEST_VECTOR
        ds_size = cvector->size(&ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
            it = cvector->find(&ds_vector_i, i); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_vector);
        GET_DURATION({ cvector->sort(&ds_vector_i, NULL); }, time_vector_s);
#elif TEST_PQUEUE
        ds_size = cpriority_queue->size(&ds_pqueue_i);
        (void)it;
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

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += chashmap->remove(&ds_hashmap_i, rand() % TIMES_FIND);   
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmap->remove(&ds_map_i, rand() % TIMES_FIND);           
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cset->remove(&ds_set_i, rand() % TIMES_FIND);           
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmultimap->remove(&ds_multimap_i, rand() % TIMES_FIND); 
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmultiset->remove(&ds_multiset_i, rand() % TIMES_FIND); 
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
            removed += clist->remove(&ds_list_i, rand() % TIMES_FIND);     
        }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
            removed += cvector->remove(&ds_vector_i, rand() % TIMES_FIND); 
        }, time_vector); /* only 10^2 */
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("Remove  [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zd ]\n", 
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

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = chashmap->clear(&ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DEINIT(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = cmap->clear(&ds_map_i);               }, time_map);
        MAP_DEINIT(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = cset->clear(&ds_set_i);               }, time_set);
        SET_DEINIT(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = cmultimap->clear(&ds_multimap_i);     }, time_multimap);
        MULTIMAP_DEINIT(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = cmultiset->clear(&ds_multiset_i);     }, time_multiset);
        MULTISET_DEINIT(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = clist->clear(&ds_list_i);             }, time_list);
        LIST_DEINIT(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = cvector->clear(&ds_vector_i);         }, time_vector);
        VECTOR_DEINIT(&ds_vector_i);
#elif TEST_PQUEUE
        GET_DURATION({ removed = cpriority_queue->clear(&ds_pqueue_i); }, time_pqueue);
        PRIORITY_QUEUE_DEINIT(&ds_pqueue_i);
#endif

        printf("Deinit  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zd ]\n", 
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
    hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t            ds_map_i      = MAP_INIT(&ds_map_i);
#elif TEST_SET
    set_t            ds_set_i      = SET_INIT(&ds_set_i);
#elif TEST_MULTIMAP
    multimap_t       ds_multimap_i = MULTIMAP_INIT(&ds_multimap_i);
#elif TEST_MULTISET
    multiset_t       ds_multiset_i = MULTISET_INIT(&ds_multiset_i);
#elif TEST_LIST
    list_t           ds_list_i     = LIST_INIT(&ds_list_i);
#elif TEST_VECTOR
    vector_t         ds_vector_i   = VECTOR_INIT(&ds_vector_i);
#elif TEST_PQUEUE
    priority_queue_t ds_pqueue_i   = PRIORITY_QUEUE_INIT(&ds_pqueue_i);
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
            chashmap->insert(&ds_hashmap_i, rand() % TIMES_FIND, i);   
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cmap->insert(&ds_map_i, rand() % TIMES_FIND, i);           
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cset->insert(&ds_set_i, rand() % TIMES_FIND);              
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cmultimap->insert(&ds_multimap_i, rand() % TIMES_FIND, i); 
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cmultiset->insert(&ds_multiset_i, rand() % TIMES_FIND);    
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            clist->push_back(&ds_list_i, rand() % TIMES_FIND);         
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cvector->push_back(&ds_vector_i, rand() % TIMES_FIND);     
        }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { 
            cpriority_queue->push(&ds_pqueue_i, rand() % TIMES_FIND);  
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
        void* it;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = chashmap->size(&ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = chashmap->find(&ds_hashmap_i, rand() % TIMES_FIND);   
            if (it && iterator_end() != it) times_succ++; 
        }, time_hashmap);
#elif TEST_MAP
        ds_size = cmap->size(&ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmap->find(&ds_map_i, rand() % TIMES_FIND);           
            if (it && iterator_end() != it) times_succ++; 
        }, time_map);
#elif TEST_SET
        ds_size = cset->size(&ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cset->find(&ds_set_i, rand() % TIMES_FIND);           
            if (it && iterator_end() != it) times_succ++; 
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = cmultimap->size(&ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmultimap->find(&ds_multimap_i, rand() % TIMES_FIND); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = cmultiset->size(&ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { 
            it = cmultiset->find(&ds_multiset_i, rand() % TIMES_FIND); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_multiset);
#elif TEST_LIST
        ds_size = clist->size(&ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
            it = clist->find(&ds_list_i, rand() % TIMES_FIND);     
            if (it && iterator_end() != it) times_succ++; 
        }, time_list);
#elif TEST_VECTOR
        ds_size = cvector->size(&ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
            it = cvector->find(&ds_vector_i, rand() % TIMES_FIND); 
            if (it && iterator_end() != it) times_succ++; 
        }, time_vector);
        GET_DURATION({ cvector->sort(&ds_vector_i, NULL); }, time_vector_s);
#elif TEST_PQUEUE
        ds_size = cpriority_queue->size(&ds_pqueue_i);
        (void)it;
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

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += chashmap->remove(&ds_hashmap_i, rand() % TIMES_FIND);   
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmap->remove(&ds_map_i, rand() % TIMES_FIND);           
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cset->remove(&ds_set_i, rand() % TIMES_FIND);           
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmultimap->remove(&ds_multimap_i, rand() % TIMES_FIND); 
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) { 
            removed += cmultiset->remove(&ds_multiset_i, rand() % TIMES_FIND); 
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
            removed += clist->remove(&ds_list_i, rand() % TIMES_FIND);     
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
            removed += cvector->remove(&ds_vector_i, rand() % TIMES_FIND); 
        }, time_vector);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("Remove  [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zd ]\n", 
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

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = chashmap->clear(&ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DEINIT(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = cmap->clear(&ds_map_i);               }, time_map);
        MAP_DEINIT(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = cset->clear(&ds_set_i);               }, time_set);
        SET_DEINIT(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = cmultimap->clear(&ds_multimap_i);     }, time_multimap);
        MULTIMAP_DEINIT(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = cmultiset->clear(&ds_multiset_i);     }, time_multiset);
        MULTISET_DEINIT(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = clist->clear(&ds_list_i);             }, time_list);
        LIST_DEINIT(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = cvector->clear(&ds_vector_i);         }, time_vector);
        VECTOR_DEINIT(&ds_vector_i);
#elif TEST_PQUEUE
        GET_DURATION({ removed = cpriority_queue->clear(&ds_pqueue_i); }, time_pqueue);
        PRIORITY_QUEUE_DEINIT(&ds_pqueue_i);
#endif

        printf("Deinit  [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zd ]\n", 
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
    class_hashmap_ops_t tops_hashmap = {
        .__hash      = __ds_ops_hash_default_string,
        .valid_key   = ds_ops_valid_key_default_string_max_128,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MAP
    class_map_ops_t tops_map = {
        .valid_key   = ds_ops_valid_key_default_string_max_128,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_SET
    class_set_ops_t tops_set = {
        .valid_value = ds_ops_valid_key_default_string_max_128,
        .__lt_value  = __ds_ops_lt_default_string,
        .copy_value  = ds_ops_copy_data_default_string,
        .free_value  = ds_ops_free_data_default_string,
    };
#elif TEST_MULTIMAP
    class_multimap_ops_t tops_multimap = {
        .valid_key   = ds_ops_valid_key_default_string_max_128,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MULTISET
    class_multiset_ops_t tops_multiset = {
        .valid_value = ds_ops_valid_data_default_string,
        .__lt_value  = __ds_ops_lt_default_string,
        .copy_value  = ds_ops_copy_data_default_string,
        .free_value  = ds_ops_free_data_default_string,
    };
#endif

#ifdef TEST_HASHMAP
    hashmap_t  ds_hashmap_s      = HASHMAP_INIT_OPS_3(&ds_hashmap_s, &tops_hashmap, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t      ds_map_s          = MAP_INIT_OPS(&ds_map_s, &tops_map);
#elif TEST_SET
    set_t      ds_set_s          = SET_INIT_OPS(&ds_set_s, &tops_set);
#elif TEST_MULTIMAP
    multimap_t ds_multimap_s     = MULTIMAP_INIT_OPS(&ds_multimap_s, &tops_multimap);
#elif TEST_MULTISET
    multiset_t ds_multiset_s     = MULTISET_INIT_OPS(&ds_multiset_s, &tops_multiset);
#elif TEST_LIST
    list_t     ds_list_s         = LIST_INIT_STRING(&ds_list_s);
#elif TEST_VECTOR
    vector_t   ds_vector_s       = VECTOR_INIT_STRING(&ds_vector_s);
#elif TEST_PQUEUE
    priority_queue_t ds_pqueue_s = PRIORITY_QUEUE_INIT_STRING(&ds_pqueue_s);
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
                trand = rand(); /* between 0 and RAND_MAX */
                len1 = trand % (sizeof(buffer[0]) + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                chashmap->insert(&ds_hashmap_s, (hashmap_key_t)(buffer[i]), i);          
            }, time_hashmap);
#elif TEST_MAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cmap->insert(&ds_map_s, (map_key_t)(buffer[i]), i);                      
            }, time_map);
#elif TEST_SET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cset->insert(&ds_set_s, (set_value_t)(buffer[i]));                       
            }, time_set);
#elif TEST_MULTIMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cmultimap->insert(&ds_multimap_s, (multimap_key_t)(buffer[i]), i);       
            }, time_multimap);
#elif TEST_MULTISET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cmultiset->insert(&ds_multiset_s, (multiset_value_t)(buffer[i]));        
            }, time_multiset);
#elif TEST_LIST
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                clist->push_back(&ds_list_s, (list_data_t)(buffer[i]));                  
            }, time_list);
#elif TEST_VECTOR
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cvector->push_back(&ds_vector_s, (vector_data_t)(buffer[i]));            
            }, time_vector);
#elif TEST_PQUEUE
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                cpriority_queue->push(&ds_pqueue_s, (priority_queue_data_t)(buffer[i])); 
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
        void* it;
        ds_size_t ds_size;

#if defined(TEST_LIST) || defined(TEST_VECTOR)
        {
#else
        for (int s = 0; s < TIMES_FIND / NUM_BUF; ++s) {
#endif /* defined(TEST_LIST) || defined(TEST_VECTOR) */
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand(); /* between 0 and RAND_MAX */
                len1 = trand % (sizeof(buffer[0]) + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            ds_size = chashmap->size(&ds_hashmap_s);
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                it = chashmap->find(&ds_hashmap_s, (hashmap_key_t)(buffer[rand() % NUM_BUF]));      
                if (it && iterator_end() != it) times_succ++; 
            }, time_hashmap);
#elif TEST_MAP
            ds_size = cmap->size(&ds_map_s);
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                it = cmap->find(&ds_map_s, (map_key_t)(buffer[rand() % NUM_BUF]));                  
                if (it && iterator_end() != it) times_succ++; 
            }, time_map);
#elif TEST_SET
            ds_size = cset->size(&ds_set_s);
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                it = cset->find(&ds_set_s, (set_value_t)(buffer[rand() % NUM_BUF]));                
                if (it && iterator_end() != it) times_succ++; 
            }, time_set);
#elif TEST_MULTIMAP
            ds_size = cmultimap->size(&ds_multimap_s);
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                it = cmultimap->find(&ds_multimap_s, (multimap_key_t)(buffer[rand() % NUM_BUF]));   
                if (it && iterator_end() != it) times_succ++; 
            }, time_multimap);
#elif TEST_MULTISET
            ds_size = cmultiset->size(&ds_multiset_s);
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                it = cmultiset->find(&ds_multiset_s, (multiset_value_t)(buffer[rand() % NUM_BUF])); 
                if (it && iterator_end() != it) times_succ++; 
            }, time_multiset);
#elif TEST_LIST
            ds_size = clist->size(&ds_list_s);
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
                it = clist->find(&ds_list_s, (list_data_t)(buffer[rand() % NUM_BUF]));          
                if (it && iterator_end() != it) times_succ++; 
            }, time_list);
#elif TEST_VECTOR
            ds_size = cvector->size(&ds_vector_s);
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { 
                it = cvector->find(&ds_vector_s, (vector_data_t)(buffer[rand() % NUM_BUF]));    
                if (it && iterator_end() != it) times_succ++; 
            }, time_vector);
            GET_DURATION({ cvector->sort(&ds_vector_s, __ds_ops_lt_default_string); }, time_vector_s);
#elif TEST_PQUEUE
            ds_size = cpriority_queue->size(&ds_pqueue_s);
            (void)it;
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

        ds_size_t removed = 0;

#if defined(TEST_LIST) || defined(TEST_VECTOR)
        {
#else
        for (int s = 0; s < TIMES_REMOVE / NUM_BUF; ++s) {
#endif /* defined(TEST_LIST) || defined(TEST_VECTOR) */
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand(); /* between 0 and RAND_MAX */
                len1 = trand % (sizeof(buffer[0]) + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }

#ifdef TEST_HASHMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += chashmap->remove(&ds_hashmap_s, (hashmap_key_t)(buffer[rand() % NUM_BUF]));        
            }, time_hashmap);
#elif TEST_MAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += cmap->remove(&ds_map_s, (map_key_t)(buffer[rand() % NUM_BUF]));                    
            }, time_map);
#elif TEST_SET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += cset->remove(&ds_set_s, (set_value_t)(buffer[rand() % NUM_BUF]));                  
            }, time_set);
#elif TEST_MULTIMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += cmultimap->remove(&ds_multimap_s, (multimap_key_t)(buffer[rand() % NUM_BUF]));     
            }, time_multimap);
#elif TEST_MULTISET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { 
                removed += cmultiset->remove(&ds_multiset_s, (multiset_value_t)(buffer[rand() % NUM_BUF]));   
            }, time_multiset);
#elif TEST_LIST
            GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
                removed += clist->remove(&ds_list_s, (list_data_t)(buffer[rand() % NUM_BUF]));       
            }, time_list);
#elif TEST_VECTOR
            GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) { 
                removed += cvector->remove(&ds_vector_s, (vector_data_t)(buffer[rand() % NUM_BUF])); 
            }, time_vector);
#elif TEST_PQUEUE
            removed = 0;
            /* unsupport */
#endif
        }

        printf("SRemove [ %.0f*10^%d times    ] [ hashmap | map | set | multimap | multiset | list(%.0f*10^%d) | vector(%.0f*10^%d) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n\tremoved [ %zd ]\n", 
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

        ds_size_t removed = 0;

#ifdef TEST_HASHMAP
        GET_DURATION({ removed = chashmap->clear(&ds_hashmap_s);       }, time_hashmap);
        HASHMAP_DEINIT(&ds_hashmap_s);
#elif TEST_MAP
        GET_DURATION({ removed = cmap->clear(&ds_map_s);               }, time_map);
        MAP_DEINIT(&ds_map_s);
#elif TEST_SET
        GET_DURATION({ removed = cset->clear(&ds_set_s);               }, time_set);
        SET_DEINIT(&ds_set_s);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = cmultimap->clear(&ds_multimap_s);     }, time_multimap);
        MULTIMAP_DEINIT(&ds_multimap_s);
#elif TEST_MULTISET
        GET_DURATION({ removed = cmultiset->clear(&ds_multiset_s);     }, time_multiset);
        MULTISET_DEINIT(&ds_multiset_s);
#elif TEST_LIST
        GET_DURATION({ removed = clist->clear(&ds_list_s);             }, time_list);
        LIST_DEINIT(&ds_list_s);
#elif TEST_VECTOR
        GET_DURATION({ removed = cvector->clear(&ds_vector_s);         }, time_vector);
        VECTOR_DEINIT(&ds_vector_s);
#elif TEST_PQUEUE
        GET_DURATION({ removed = cpriority_queue->clear(&ds_pqueue_s); }, time_pqueue);
        PRIORITY_QUEUE_DEINIT(&ds_pqueue_s);
#endif

        printf("SDeinit [ %.0f*10^%d elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n\tremoved [ %zd ]\n", 
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
