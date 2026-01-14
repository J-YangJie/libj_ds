/*
  Sorting Algorithms Implementations
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

#include <time.h>
#include <string.h>

#include <_types.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include <hashmap/hashmap.h>
#include <map/map.h>
#include <set/set.h>
#include <multimap/multimap.h>
#include <multiset/multiset.h>
#include <priority_queue/priority_queue.h>

#include "demo_ops.h"
#include "test_hashmap.h"
#include "test_map.h"
#include "test_set.h"
#include "test_multimap.h"
#include "test_multiset.h"

#define TAG "[stress_test]"

void* _p_malloc(size_t size)                { int num = rand(); if (123 == num % 654321) { pr_test("Memory [ malloc  ] [ %d ] failed!", num); return NULL; } return malloc(size); }
void* _p_calloc(size_t nmemb, size_t size)  { int num = rand(); if (123 == num % 654321) { pr_test("Memory [ calloc  ] [ %d ] failed!", num); return NULL; } return calloc(nmemb, size); }
void* _p_realloc(void* ptr, size_t size)    { int num = rand(); if (0 == num % 5)        { pr_test("Memory [ realloc ] [ %d ] failed!", num); return NULL; } return realloc(ptr, size); }
void  _p_free(void** ptr)                   { if (NULL != ptr && NULL != *ptr) { free(*ptr); *ptr = NULL; } }

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

typedef struct {
    void* this;
    const class_stress_interface_t* interface;
} stress_t;

typedef struct {
    ds_key_t key;
    ds_value_t value;
} stress_iterator_kv_t;

typedef struct {
    ds_data_t data;
} stress_iterator_d_t;

#define NUM_DS 6

static bool test_insert(stress_t* stress, ds_data_t data)
{
    ds_data_t value_replace = 99;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            stress_iterator_d_t* it = interface->insert(this, data, 666);
            if (!it)
                goto err;

            ds_count_t tcount = interface->count(this, data);
            if (tcount > 1) pr_test("Multi insert, count [ %zd ]!", tcount);
        }

        if (interface->insert_replace) {
            stress_iterator_kv_t* it = interface->insert_replace(this, data, value_replace);
            if (!it)
                goto err;

            if (value_replace != it->value)
                while(true); /* 排查问题 */
        }
    }
    return true;

err:
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            interface->remove(this, data);
        }
    }
    return false;
}

static bool test_iterator(stress_t* stress)
{
    stress_iterator_d_t* its[NUM_DS];
    ds_size_t ts[4];

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            for (ts[0] = 0, its[i] = interface->begin(this);  interface->end(this) != its[i];  its[i] = interface->next(this, its[i]))  ts[0]++;
            for (ts[1] = 0, its[i] = interface->end(this);    interface->end(this) != its[i];  its[i] = interface->prev(this, its[i]))  ts[1]++;
            for (ts[2] = 0, its[i] = interface->rbegin(this); interface->rend(this) != its[i]; its[i] = interface->rnext(this, its[i])) ts[2]++;
            for (ts[3] = 0, its[i] = interface->rend(this);   interface->rend(this) != its[i]; its[i] = interface->rprev(this, its[i])) ts[3]++;
            if (ts[0] ^ ts[1] ^ ts[2] ^ ts[3])
                while(true); /* 排查问题 */
        }
    }
    return true;
}

static bool test_is_same(stress_t* stress)
{
    stress_iterator_d_t* its[NUM_DS];
    class_priority_queue_ops_t ds_ops = {
        .valid_data = demo_valid_key,
        .__lt       = __demo_cmp,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    priority_queue_t ds = PRIORITY_QUEUE_INIT_OPS(&ds, &ds_ops);

    /* TODO: 首个数据结构不能是multi类型 */
    {
        void* this = stress[0].this;
        const class_stress_interface_t* interface = stress[0].interface;

        for (its[0] = interface->begin(this); interface->end(this) != its[0]; its[0] = interface->next(this, its[0]))
            cpriority_queue->push(&ds, its[0]->data);
    }

    while (cpriority_queue->size(&ds) > 0) {
        ds_data_t tdata = cpriority_queue->pop(&ds, 0);
        for (int i = 0; stress[i].this; ++i) {
            void* this = stress[i].this;
            const class_stress_interface_t* interface = stress[i].interface;
            if (!interface)
                continue;

            if (interface->end(this) == interface->find(this, tdata))
                while(true); /* 排查问题 */
        }
        demo_free_key(&tdata);
    }

    pr_test("Is same [ true ]!");
    PRIORITY_QUEUE_DEINIT(&ds);
    return true;
}

static bool test_find_and_erase(stress_t* stress, ds_data_t data)
{
    stress_iterator_d_t* its[NUM_DS] = { 0 };
    ds_data_t cache = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            its[i] = interface->find(this, data);
            ds_count_t count = interface->count(this, data);
            if (count < 0)
                while(true); /* 排查问题 */

            if ((0 == count && interface->end(this) != its[i]) || (count > 0 && interface->end(this) == its[i]))
                while(true); /* 排查问题 */

            if (count > 0)
                cache = its[i]->data;
        }
    }

    if (0 == cache)
        return true;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (__demo_cmp(cache, its[i]->data) || __demo_cmp(its[i]->data, cache))
            while(true); /* 排查问题 */

        {
            its[i] = interface->erase(this, its[i]);
            pr_test("Found element and erase [ %s ]!", its[i] ? "successfully" : "failed");
        }
    }
    return true;
}

static bool test_lower_and_upper(stress_t* stress, ds_data_t data)
{
    bool first = true;
    ds_data_t data_first;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->lower_bound) {
            stress_iterator_d_t* tit = interface->lower_bound(this, data);
            ds_data_t tdata = interface->end(this) == tit ? 0 : tit->data;

            if (first) {
                data_first = tdata;
                first = false;
                continue;
            }
            
            if (0 == data_first) {
                if (0 != tdata)
                    while(true); /* 排查问题 */
            } else {
                if (__demo_cmp(data_first, tdata) || __demo_cmp(tdata, data_first))
                    while(true); /* 排查问题 */
            }
        }
    }

    first = true;
	for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->upper_bound) {
            stress_iterator_d_t* tit = interface->upper_bound(this, data);
            ds_data_t tdata = interface->end(this) == tit ? 0 : tit->data;

            if (first) {
                data_first = tdata;
                first = false;
                continue;
            }
            
            if (0 == data_first) {
                if (0 != tdata)
                    while(true); /* 排查问题 */
            } else {
                if (__demo_cmp(data_first, tdata) || __demo_cmp(tdata, data_first))
                    while(true); /* 排查问题 */
            }
        }
    }
    return true;
}

static bool test_remove(stress_t* stress, ds_data_t data)
{
    ds_count_t count = -1;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_count_t tcount = interface->count(this, data);
            if (tcount < 0)
                while(true); /* 排查问题 */

            if (-1 == count) {
                count = !!tcount;
            } else {
                if (count != (!!tcount))
                    while(true); /* 排查问题 */
            }

            if (0 == tcount)
                continue;

            ds_size_t tremoved = interface->remove(this, data);
            if (tcount != tremoved)
                while(true); /* 排查问题 */
            pr_test("Found element and remove [ %zd ]!", tremoved);
        }
    }
    return true;
}

static bool test_clear(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_size_t num = interface->clear(this);
            if (num < 0)
                while(true); /* 排查问题 */
            pr_test("Clear [ %zd ] elements!", num);
        }
    }
    return true;
}

static ds_size_t test_total_size(stress_t* stress)
{
    ds_size_t count = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_size_t size = interface->size(this);
            if (size < 0)
                while(true); /* 排查问题 */

            count += size;
        }
    }
    return count;
}

int main(int argc, char** argv)
{
    class_hashmap_ops_t hashmap_ops = {
        .__hash      = __demo_hash,
        .valid_key   = demo_valid_key,
        .__lt        = __demo_cmp,
        .copy_key    = demo_copy_key,
        .free_key    = demo_free_key,
        .valid_value = NULL,
        .copy_value  = NULL,
        .free_value  = NULL,
    };
    hashmap_t hashmap = HASHMAP_INIT_OPS_3(&hashmap, &hashmap_ops, 0, 0, 0.0);

    class_map_ops_t map_ops = {
        .valid_key   = demo_valid_key,
        .__lt        = __demo_cmp,
        .copy_key    = demo_copy_key,
        .free_key    = demo_free_key,
        .valid_value = NULL,
        .copy_value  = NULL,
        .free_value  = NULL,
    };
    map_t map = MAP_INIT_OPS(&map, &map_ops);

    class_set_ops_t set_ops = {
        .valid_value = demo_valid_key,
        .__lt_value  = __demo_cmp,
        .copy_value  = demo_copy_key,
        .free_value  = demo_free_key,
    };
    set_t set = SET_INIT_OPS(&set, &set_ops);

    class_multimap_ops_t multimap_ops = {
        .valid_key   = demo_valid_key,
        .__lt        = __demo_cmp,
        .copy_key    = demo_copy_key,
        .free_key    = demo_free_key,
        .valid_value = NULL,
        .copy_value  = NULL,
        .free_value  = NULL,
    };
    multimap_t multimap = MULTIMAP_INIT_OPS(&multimap, &multimap_ops);

    class_multiset_ops_t multiset_ops = {
        .valid_value = demo_valid_key,
        .__lt_value  = __demo_cmp,
        .copy_value  = demo_copy_key,
        .free_value  = demo_free_key,
    };
    multiset_t multiset = MULTISET_INIT_OPS(&multiset, &multiset_ops);

    stress_t stress[NUM_DS] = { (stress_t){ .this = &hashmap,  .interface = cs_hashmap },
                                (stress_t){ .this = &map,      .interface = cs_map },
                                (stress_t){ .this = &set,      .interface = cs_set },
                                (stress_t){ .this = &multimap, .interface = cs_multimap },
                                (stress_t){ .this = &multiset, .interface = cs_multiset },
                                (stress_t){ .this = NULL,      .interface = NULL },
                                };

    srand(time(0));
    char buffer[128];
    long times = 0;
    long limit = argc > 1 ? atoi(argv[1]) : 1000000;

    for (; times < limit; ++times) {
        if (1)
        {
            ds_size_t total = test_total_size(stress);
            if (0 == times % 100000) {
                pr_test("================================ Times [ %ld ], total elements [ %zd ]", times, total);
            }

            if (total > 13000000) {
                pr_test("Memory warning, clear!");
                test_clear(stress);
            }
        }

        {
            int len1 = rand() % (sizeof(buffer) + 1);
            if (len1 < 5) len1 = 5;
            string_rand(buffer, len1);
        }
        test_data_t tmp = { .name = buffer, .id = rand(), };

#define SWITCH_MAX 10000000
        int curr_ops = rand() % (SWITCH_MAX + 1);
        switch (curr_ops) {
        case 0:
        case 1:
            test_clear(stress);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            test_is_same(stress);
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            test_iterator(stress);
            break;
        default:
        {
            if (curr_ops < 10000)
                test_remove(stress, (ds_data_t)(&tmp));
            else if (curr_ops >= 10000 && curr_ops < 20000)
                test_lower_and_upper(stress, (ds_data_t)(&tmp));
            else if (curr_ops >= 20000 && curr_ops < 100000)
                test_find_and_erase(stress, (ds_data_t)(&tmp));
            else
                test_insert(stress, (ds_data_t)(&tmp));
        }
            break;
        }
    }

    HASHMAP_DEINIT(&hashmap);
    MAP_DEINIT(&map);
    SET_DEINIT(&set);
    MULTIMAP_DEINIT(&multimap);
    MULTISET_DEINIT(&multiset);

    return 0;
}
