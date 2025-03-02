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
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
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

#define TEST_HASHMAP        1
#define TEST_MAP            1
#define TEST_SET            1
#define TEST_MULTIMAP       1
#define TEST_MULTISET       1
#define TEST_LIST           1
#define TEST_VECTOR         1
#define TEST_PQUEUE         1

#define TIMES_INSERT   10000000
#define TIMES_FIND     100000000
#define TIMES_FIND_V_L 100

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
    hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, 2 * TIMES_INSERT, 0, 0.0);
    // hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, 0, 0, 0.0);
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

        printf("Insert  [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { chashmap->find(&ds_hashmap_i, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmap->find(&ds_map_i, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cset->find(&ds_set_i, i);           }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultimap->find(&ds_multimap_i, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultiset->find(&ds_multiset_i, i); }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { clist->find(&ds_list_i, i);     }, time_list);     /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cvector->find(&ds_vector_i, i); }, time_vector);   /* only 10^2 */
        GET_DURATION(for (int i = 0; i < 1; ++i) { cvector->sort(&ds_vector_i, NULL);  }, time_vector_s);
#elif TEST_PQUEUE
        /* unsupport */
#endif

        printf("Find    [ 10^8 times    ] [ hashmap | map | set | multimap | multiset | list(10^2) | vector(10^2) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000);
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

#ifdef TEST_HASHMAP
        GET_DURATION({ HASHMAP_DEINIT(&ds_hashmap_i);       }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ MAP_DEINIT(&ds_map_i);               }, time_map);
#elif TEST_SET
        GET_DURATION({ SET_DEINIT(&ds_set_i);               }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ MULTIMAP_DEINIT(&ds_multimap_i);     }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ MULTISET_DEINIT(&ds_multiset_i);     }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ LIST_DEINIT(&ds_list_i);             }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ VECTOR_DEINIT(&ds_vector_i);         }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION({ PRIORITY_QUEUE_DEINIT(&ds_pqueue_i); }, time_pqueue);
#endif

        printf("Deinit  [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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
    hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, 2 * TIMES_INSERT, 0, 0.0);
    // hashmap_t        ds_hashmap_i  = HASHMAP_INIT_3(&ds_hashmap_i, 0, 0, 0.0);
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
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { chashmap->insert(&ds_hashmap_i, rand() % TIMES_FIND, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmap->insert(&ds_map_i, rand() % TIMES_FIND, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cset->insert(&ds_set_i, rand() % TIMES_FIND);              }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmultimap->insert(&ds_multimap_i, rand() % TIMES_FIND, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cmultiset->insert(&ds_multiset_i, rand() % TIMES_FIND);    }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { clist->push_back(&ds_list_i, rand() % TIMES_FIND);         }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cvector->push_back(&ds_vector_i, rand() % TIMES_FIND);     }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { cpriority_queue->push(&ds_pqueue_i, rand() % TIMES_FIND);  }, time_pqueue);
#endif

        printf("Insert  [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { chashmap->find(&ds_hashmap_i, rand() % TIMES_FIND);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmap->find(&ds_map_i, rand() % TIMES_FIND);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cset->find(&ds_set_i, rand() % TIMES_FIND);           }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultimap->find(&ds_multimap_i, rand() % TIMES_FIND); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultiset->find(&ds_multiset_i, rand() % TIMES_FIND); }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { clist->find(&ds_list_i, rand() % TIMES_FIND);     }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cvector->find(&ds_vector_i, rand() % TIMES_FIND); }, time_vector); /* only 10^2 */
        GET_DURATION({ cvector->sort(&ds_vector_i, NULL); }, time_vector_s);
#elif TEST_PQUEUE
            /* unsupport */
#endif

        printf("Find    [ 10^8 times    ] [ hashmap | map | set | multimap | multiset | list(10^2) | vector(10^2) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000);
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

#ifdef TEST_HASHMAP
        GET_DURATION({ HASHMAP_DEINIT(&ds_hashmap_i);       }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ MAP_DEINIT(&ds_map_i);               }, time_map);
#elif TEST_SET
        GET_DURATION({ SET_DEINIT(&ds_set_i);               }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ MULTIMAP_DEINIT(&ds_multimap_i);     }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ MULTISET_DEINIT(&ds_multiset_i);     }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ LIST_DEINIT(&ds_list_i);             }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ VECTOR_DEINIT(&ds_vector_i);         }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION({ PRIORITY_QUEUE_DEINIT(&ds_pqueue_i); }, time_pqueue);
#endif

        printf("Deinit  [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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
    hashmap_t  ds_hashmap_s      = HASHMAP_INIT_OPS_3(&ds_hashmap_s, &tops_hashmap, 2 * TIMES_INSERT, 0, 0.0);
    // hashmap_t  ds_hashmap_s      = HASHMAP_INIT_OPS_3(&ds_hashmap_s, &tops_hashmap, 0, 0, 0.0);
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

#define NUM_BUF 1000000
#define NUM_KEY_LEN 8
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
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { chashmap->insert(&ds_hashmap_s, (hashmap_key_t)(buffer[i]), i);          }, time_hashmap);
#elif TEST_MAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cmap->insert(&ds_map_s, (map_key_t)(buffer[i]), i);                      }, time_map);
#elif TEST_SET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cset->insert(&ds_set_s, (set_value_t)(buffer[i]));                       }, time_set);
#elif TEST_MULTIMAP
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cmultimap->insert(&ds_multimap_s, (multimap_key_t)(buffer[i]), i);       }, time_multimap);
#elif TEST_MULTISET
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cmultiset->insert(&ds_multiset_s, (multiset_value_t)(buffer[i]));        }, time_multiset);
#elif TEST_LIST
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { clist->push_back(&ds_list_s, (list_data_t)(buffer[i]));                  }, time_list);
#elif TEST_VECTOR
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cvector->push_back(&ds_vector_s, (vector_data_t)(buffer[i]));            }, time_vector);
#elif TEST_PQUEUE
            GET_DURATION(for (int i = 0; i < NUM_BUF; ++i) { cpriority_queue->push(&ds_pqueue_s, (priority_queue_data_t)(buffer[i])); }, time_pqueue);
#endif
        }

        printf("SInsert [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector(push_back) | pqueue ] \t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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

        {
            int trand;
            int len1;

            for (int i = 0; i < NUM_BUF; ++i) {
                trand = rand(); /* between 0 and RAND_MAX */
                len1 = trand % (sizeof(buffer[0]) + 1);
                if (len1 <= 1) len1 = 2;
                string_rand(buffer[i], len1);
            }
        }

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { chashmap->find(&ds_hashmap_s, (hashmap_key_t)(buffer[rand() % NUM_BUF]));      }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmap->find(&ds_map_s, (map_key_t)(buffer[rand() % NUM_BUF]));                  }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cset->find(&ds_set_s, (set_value_t)(buffer[rand() % NUM_BUF]));                }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultimap->find(&ds_multimap_s, (multimap_key_t)(buffer[rand() % NUM_BUF]));   }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) { cmultiset->find(&ds_multiset_s, (multiset_value_t)(buffer[rand() % NUM_BUF])); }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { clist->find(&ds_list_s, (list_data_t)(buffer[rand() % NUM_BUF]));          }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cvector->find(&ds_vector_s, (vector_data_t)(buffer[rand() % NUM_BUF]));    }, time_vector); /* only 10^2 */
        GET_DURATION({ cvector->sort(&ds_vector_s, __ds_ops_lt_default_string); }, time_vector_s);
#endif

        printf("SFind   [ 10^8 times    ] [ hashmap | map | set | multimap | multiset | list(10^2) | vector(10^2) | vector_sort | pqueue ] \t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld | unsupport ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_vector_s / 1000);
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

#ifdef TEST_HASHMAP
        GET_DURATION({ HASHMAP_DEINIT(&ds_hashmap_s);       }, time_hashmap);
#elif TEST_MAP
        GET_DURATION({ MAP_DEINIT(&ds_map_s);               }, time_map);
#elif TEST_SET
        GET_DURATION({ SET_DEINIT(&ds_set_s);               }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION({ MULTIMAP_DEINIT(&ds_multimap_s);     }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION({ MULTISET_DEINIT(&ds_multiset_s);     }, time_multiset);
#elif TEST_LIST
        GET_DURATION({ LIST_DEINIT(&ds_list_s);             }, time_list);
#elif TEST_VECTOR
        GET_DURATION({ VECTOR_DEINIT(&ds_vector_s);         }, time_vector);
#elif TEST_PQUEUE
        GET_DURATION({ PRIORITY_QUEUE_DEINIT(&ds_pqueue_s); }, time_pqueue);
#endif

        printf("SDeinit [ 10^7 elements ] [ hashmap | map | set | multimap | multiset | list | vector | pqueue ] \t\t\t\t= [ %ld | %ld | %ld | %ld | %ld | %ld | %ld | %ld ] ms\n", 
                time_hashmap  / 1000,
                time_map      / 1000,
                time_set      / 1000,
                time_multimap / 1000,
                time_multiset / 1000,
                time_list     / 1000,
                time_vector   / 1000,
                time_pqueue   / 1000);
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
