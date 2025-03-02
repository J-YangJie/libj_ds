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
#include <vector/vector.h>
#include <list/list.h>
#include <priority_queue/priority_queue.h>

#include "demo_ops.h"
#include "test_vector.h"
#include "test_list.h"

#define TAG "[stress_slow]"

void* _p_malloc(size_t size)                { int num = rand(); if (123 == num % 4321) { pr_test("Memory [ malloc  ] [ %d ] failed!", num); return NULL; } return malloc(size); }
void* _p_calloc(size_t nmemb, size_t size)  { int num = rand(); if (123 == num % 4321) { pr_test("Memory [ calloc  ] [ %d ] failed!", num); return NULL; } return calloc(nmemb, size); }
void* _p_realloc(void* ptr, size_t size)    { int num = rand(); if (0 == num % 5)      { pr_test("Memory [ realloc ] [ %d ] failed!", num); return NULL; } return realloc(ptr, size); }
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
    const class_stress_slow_interface_t* interface;
} stress_t;

typedef struct {
    ds_data_t data;
} stress_iterator_d_t;

#define NUM_DS 3

static bool test_insert(stress_t* stress, ds_data_t data)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            stress_iterator_d_t* it = NULL;
            switch (data % 20)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                it = interface->insert(this, interface->end(this), data);
                break;
            case 9:
                it = interface->insert(this, interface->begin(this), data);
                break;
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                it = interface->push_back(this, data);
                break;
            case 19:
                it = interface->push_front(this, data);
                break;
            default:
                break;
            }
            if (!it)
                goto err;

            // ds_count_t tcount = interface->count(this, data);
            // if (tcount > 1) pr_test("Multi insert, count [ %zd ]!", tcount);
        }
    }
    return true;

err:
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
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
        const class_stress_slow_interface_t* interface = stress[i].interface;
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
    ds_size_t num_element = 0;
    ds_data_t data_first = 0;
    ds_data_t data_last  = 0;

    ds_size_t total = 0;
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            total += interface->size(this);
        }
    }

    if (0 == total)
        return true;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            if (0 == num_element) {
                num_element = interface->size(this);
            } else {
                if (num_element ^ interface->size(this))
                    while(true); /* 排查问题 */
            }
        }

        {
            if (0 == data_first) {
                data_first = interface->first(this, 0);
            } else {
                if (__demo_cmp(data_first, interface->first(this, 0)) 
                    || __demo_cmp(interface->first(this, 0), data_first))
                    while(true); /* 排查问题 */
            }
        }

        {
            if (0 == data_last) {
                data_last = interface->last(this, 0);
            } else {
                if (__demo_cmp(data_last, interface->last(this, 0)) 
                    || __demo_cmp(interface->last(this, 0), data_last))
                    while(true); /* 排查问题 */
            }
        }

        {
            its[i] = interface->begin(this);
        }
    }

    while (num_element > 0) {
        num_element--;

        ds_data_t tdata = 0;
        for (int i = 0; stress[i].this; ++i) {
            void* this = stress[i].this;
            const class_stress_slow_interface_t* interface = stress[i].interface;
            if (!interface)
                continue;

            if (0 == tdata) {
                tdata = its[i]->data;
                its[i] = interface->next(this, its[i]);
                continue;
            }

            if (__demo_cmp(tdata, its[i]->data) || __demo_cmp(its[i]->data, tdata))
                while(true); /* 排查问题 */

            its[i] = interface->next(this, its[i]);
        }
    }

    pr_test("Is same [ true ]!");
    return true;
}

static bool test_find_and_erase(stress_t* stress, ds_data_t data)
{
    stress_iterator_d_t* its[NUM_DS] = { 0 };
    ds_data_t cache = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
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
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (__demo_cmp(cache, its[i]->data) || __demo_cmp(its[i]->data, cache))
            while(true); /* 排查问题 */

        {
            its[i] = interface->erase(this, its[i]);
            pr_test("Found element and erase [ %s ]!", its[i] ? "successfully" : "failed");

            its[i] = interface->erase_range(this, its[i], interface->next(this, interface->next(this, its[i])));
            pr_test("After erase, erase_range [ %s ]!", its[i] ? "successfully" : "failed");
        }
    }
    return true;
}

static bool test_pop_back(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            interface->pop_back(this);
        }
    }
    return true;
}

static bool test_pop_front(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            interface->pop_front(this);
        }
    }
    return true;
}

static bool test_remove(stress_t* stress, ds_data_t data)
{
    ds_count_t count = -1;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
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

static bool test_remove_if(stress_t* stress, remove_if_condition cond)
{
    ds_size_t removed = -1;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_size_t tremoved = interface->remove_if(this, cond);
            if (-1 == removed) {
                removed = tremoved;
            } else {
                if (removed != tremoved)
                    while(true); /* 排查问题 */
            }

            if (removed > 0)
                pr_test("Found element and remove_if [ %zd ]!", removed);
        }
    }
    return true;
}

static inline bool test_sort_ascending(ds_data_t left, ds_data_t right)
{
    return __demo_cmp_lt(left, right);
}

static inline bool test_sort_descending(ds_data_t left, ds_data_t right)
{
    return __demo_cmp(left, right);
}

static bool test_sort(stress_t* stress, bool ascending)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->sort) {
            __comp rule = ascending ? test_sort_ascending : test_sort_descending;
            interface->sort(this, rule);

            stress_iterator_d_t* it = interface->begin(this);
            if (interface->end(this) == it)
                continue;

            ds_data_t data = it->data;
            for (it = interface->next(this, it); interface->end(this) != it; it = interface->next(this, it)) {
                if (rule(it->data, data))
                    while(true); /* 排查问题 */
            }
            pr_test("Sort successfully [ %s ]!", ascending ? "ascending" : "descending");
        }
    }
    return true;
}

static bool test_clear(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
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
        const class_stress_slow_interface_t* interface = stress[i].interface;
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
    class_vector_ops_t vector_ops = {
        .valid_data = demo_valid_key,
        .__lt       = __demo_cmp,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    vector_t vector = VECTOR_INIT_OPS(&vector, &vector_ops);

    class_list_ops_t list_ops = {
        .valid_data = demo_valid_key,
        .__lt       = __demo_cmp,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    list_t list = LIST_INIT_OPS(&list, &list_ops);

    stress_t stress[NUM_DS] = { (stress_t){ .this = &vector,  .interface = cs_vector },
                                (stress_t){ .this = &list,    .interface = cs_list },
                                (stress_t){ .this = NULL,     .interface = NULL },
                                };

    srand(time(0));
    char buffer[128];
    long times = 0;
    long limit = argc > 1 ? atoi(argv[1]) : 1000000;

    for (; times < limit; ++times) {
        if (1)
        {
            ds_size_t total = test_total_size(stress);
            if (0 == times % 10000) {
                pr_test("================================ Times [ %ld ], total elements [ %zd ]", times, total);
            }

            if (total > 3000000) {
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

#define SWITCH_MAX 1000000
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
        case 10:
        case 11:
        case 12:
        case 13:
            test_remove_if(stress, test_remove_if_cond);
            break;
        default:
        {
            if (curr_ops < 100) {
                test_sort(stress, curr_ops % 2);
                test_clear(stress);
            } else if (curr_ops >= 100 && curr_ops < 1000)
                test_pop_front(stress);
            else if (curr_ops >= 1000 && curr_ops < 10000)
                test_remove(stress, (ds_data_t)(&tmp));
            else if (curr_ops >= 10000 && curr_ops < 20000)
                test_pop_back(stress);
            else if (curr_ops >= 20000 && curr_ops < 100000)
                test_find_and_erase(stress, (ds_data_t)(&tmp));
            else
                test_insert(stress, (ds_data_t)(&tmp));
        }
            break;
        }
    }

    VECTOR_DEINIT(&vector);
    LIST_DEINIT(&list);

    return 0;
}
