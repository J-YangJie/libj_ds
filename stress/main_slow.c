/*
  Sorting Algorithms Implementations
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <_types.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include <vector/vector.h>
#include <list/list.h>
#include <deque/deque.h>

#include "demo_ops.h"
#include "test_vector.h"
#include "test_list.h"
#include "test_deque.h"

#define TAG "[stress_slow]"

#define STRESS_NEW(_ptr, _expr) \
    do { while (NULL == ((_ptr) = (_expr))) ; } while (0)

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

#define NUM_DS 4

static long g_stress_times = 0;
static int g_stress_op = 0;

static void stress_fail(int line)
{
    pr_test("Stress check failed at line [ %d ], times [ %ld ], op [ %d ]!", line, g_stress_times, g_stress_op);
    abort();
}

static void stress_clear_all(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        interface->clear(this);
    }
}

static ds_size_t test_total_size(stress_t* stress)
{
    ds_size_t count = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        count += interface->size(this);
    }
    return count;
}

static ds_size_t stress_first_size(stress_t* stress)
{
    for (int i = 0; stress[i].this; ++i) {
        if (stress[i].interface)
            return stress[i].interface->size(stress[i].this);
    }
    return 0;
}

static bool stress_sizes_same(stress_t* stress, ds_size_t expected)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->size(this) != expected)
            return false;
    }
    return true;
}

static bool stress_data_all_equal(stress_t* stress, ds_data_t data)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        for (stress_iterator_t it = interface->begin(this); stress_it_ne(interface->end(this), it); it = interface->next(this, it)) {
            if (!__demo_cmp_eq(stress_it_data(it), data))
                return false;
        }
    }
    return true;
}

static ds_data_t* stress_collect(stress_t* stress, ds_size_t* num)
{
    ds_size_t size = stress_first_size(stress);
    ds_data_t* seq = NULL;

    *num = size;
    if (0 == size)
        return NULL;

    seq = (ds_data_t*)malloc(sizeof(ds_data_t) * (size_t)size);
    if (NULL == seq)
        return NULL;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_size_t index = 0;
        for (stress_iterator_t it = interface->begin(this); stress_it_ne(interface->end(this), it); it = interface->next(this, it))
            seq[index++] = stress_it_data(it);
        break;
    }
    return seq;
}

static bool stress_seq_eq(stress_t* stress, ds_data_t* seq, ds_size_t num)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->size(this) != num) {
            pr_test("seq_eq c[%d] size [%zd] != [%zd]", i, interface->size(this), num);
            return false;
        }

        ds_size_t index = 0;
        for (stress_iterator_t it = interface->begin(this); stress_it_ne(interface->end(this), it); it = interface->next(this, it)) {
            if (!__demo_cmp_eq(stress_it_data(it), seq[index])) {
                pr_test("seq_eq c[%d] index [%zd] data mismatch", i, index);
                return false;
            }
            index++;
        }
    }
    return true;
}

static bool fill_aux(stress_t* aux, int count)
{
    stress_clear_all(aux);

    for (int k = 0; k < count; ++k) {
        char buffer[128];
        int len = rand() % (sizeof(buffer) + 1);
        if (len < 5) len = 5;
        string_rand(buffer, len);
        test_data_t tmp = { .name = buffer, .id = rand(), };

        for (int i = 0; aux[i].this; ++i) {
            void* this = aux[i].this;
            const class_stress_slow_interface_t* interface = aux[i].interface;
            if (!interface)
                continue;

            if (stress_it_null(interface->push_back(this, (ds_data_t)(&tmp)))) {
                stress_clear_all(aux);
                return false;
            }
        }
    }
    return true;
}

static stress_iterator_t stress_push_front(void* this, const class_stress_slow_interface_t* interface, ds_data_t data)
{
    if (interface->push_front)
        return interface->push_front(this, data);
    return interface->insert(this, interface->begin(this), data);
}

static void stress_pop_front(void* this, const class_stress_slow_interface_t* interface)
{
    if (interface->pop_front)
        interface->pop_front(this);
    else
        interface->erase(this, interface->begin(this));
}

static bool test_insert(stress_t* stress, ds_data_t data)
{
    stress_iterator_t inserted[NUM_DS] = { 0 };
    bool success[NUM_DS] = { false };
    ds_size_t size = stress_first_size(stress);
    ds_index_t index = size > 0 ? (ds_index_t)(rand() % size) : 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        stress_iterator_t it = { 0 };
        switch ((ds_uintptr_t)data % 100)
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
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
            it = interface->push_back(this, data);
            break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            it = interface->insert(this, interface->end(this), data);
            break;
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
            it = interface->insert(this, interface->begin(this), data);
            break;
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
            it = interface->insert(this, interface->it(this, index), data);
            break;
        default:
            it = stress_push_front(this, interface, data);
            break;
        }

        if (stress_it_null(it))
            goto err;

        inserted[i] = it;
        success[i] = true;
    }
    return true;

err:
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface || !success[i])
            continue;

        interface->erase(this, inserted[i]);
    }
    return false;
}

static bool test_insert_n(stress_t* stress, ds_data_t data)
{
    stress_iterator_t inserted[NUM_DS] = { 0 };
    bool success[NUM_DS] = { false };
    ds_size_t n = 1 + (ds_size_t)(rand() % 4);
    ds_size_t size = stress_first_size(stress);
    ds_index_t index = size > 0 ? (ds_index_t)(rand() % size) : 0;
    int mode = rand() % 3;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        stress_iterator_t it = { 0 };
        switch (mode)
        {
        case 0:
            it = interface->insert_n(this, interface->begin(this), n, data);
            break;
        case 1:
            it = interface->insert_n(this, interface->end(this), n, data);
            break;
        default:
            it = interface->insert_n(this, interface->it(this, index), n, data);
            break;
        }

        if (stress_it_null(it))
            goto err;

        inserted[i] = it;
        success[i] = true;
    }
    return true;

err:
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface || !success[i])
            continue;

        stress_iterator_t it = inserted[i];
        for (ds_size_t k = 0; k < n; ++k)
            it = interface->erase(this, it);
    }
    return false;
}

static bool test_iterator(stress_t* stress)
{
    stress_iterator_t its[NUM_DS];
    ds_size_t ts[4];

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            for (ts[0] = 0, its[i] = interface->begin(this);  stress_it_ne(interface->end(this), its[i]);  its[i] = interface->next(this, its[i]))        ts[0]++;
            for (ts[1] = 0, its[i] = interface->end(this);    stress_it_ne(interface->begin(this), its[i]); )  { its[i] = interface->prev(this, its[i]);  ts[1]++; }
            for (ts[2] = 0, its[i] = interface->rbegin(this); stress_it_ne(interface->rend(this), its[i]); its[i] = interface->rnext(this, its[i]))       ts[2]++;
            for (ts[3] = 0, its[i] = interface->rend(this);   stress_it_ne(interface->rbegin(this), its[i]); ) { its[i] = interface->rprev(this, its[i]); ts[3]++; }
            if (ts[0] ^ ts[1] ^ ts[2] ^ ts[3])
                stress_fail(__LINE__);
        }
    }

    if (stress_first_size(stress) > 0) {
        ds_size_t size = stress_first_size(stress);
        ds_index_t index = (ds_index_t)(rand() % size);
        ds_data_t cache = 0;

        for (int i = 0; stress[i].this; ++i) {
            void* this = stress[i].this;
            const class_stress_slow_interface_t* interface = stress[i].interface;
            if (!interface)
                continue;

            ds_data_t data_at = interface->at(this, index, 0);
            ds_data_t data_it = stress_it_data(interface->it(this, index));
            if (!__demo_cmp_eq(data_at, data_it))
                stress_fail(__LINE__);

            if (0 == cache)
                cache = data_at;
            else if (!__demo_cmp_eq(cache, data_at))
                stress_fail(__LINE__);

            if (!__demo_cmp_eq(interface->front(this, 0), stress_it_data(interface->begin(this))))
                stress_fail(__LINE__);

            stress_iterator_t last = interface->prev(this, interface->end(this));
            if (!__demo_cmp_eq(interface->back(this, 0), stress_it_data(last)))
                stress_fail(__LINE__);
        }
    }
    return true;
}

static bool test_is_same(stress_t* stress)
{
    stress_iterator_t its[NUM_DS];
    ds_size_t num_element = 0;
    ds_data_t data_first = 0;
    ds_data_t data_last  = 0;
    ds_size_t total = test_total_size(stress);

    if (0 == total)
        return true;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_size_t size = interface->size(this);
            if (size <= 0 || interface->max_size(this) < size || interface->empty(this))
                stress_fail(__LINE__);

            if (0 == num_element) {
                num_element = size;
            } else {
                if (num_element ^ size)
                    stress_fail(__LINE__);
            }
        }

        {
            ds_data_t first = interface->front(this, 0);
            ds_data_t last = interface->back(this, 0);
            if (0 == first || 0 == last)
                stress_fail(__LINE__);

            if (0 == data_first) {
                data_first = first;
                data_last = last;
            } else {
                if (!__demo_cmp_eq(data_first, first) || !__demo_cmp_eq(data_last, last))
                    stress_fail(__LINE__);
            }
        }

        its[i] = interface->begin(this);
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
                tdata = stress_it_data(its[i]);
                its[i] = interface->next(this, its[i]);
                continue;
            }

            if (!__demo_cmp_eq(tdata, stress_it_data(its[i])))
                stress_fail(__LINE__);

            its[i] = interface->next(this, its[i]);
        }
    }

    pr_test("Is same [ true ]!");
    return true;
}

static bool test_find_and_erase(stress_t* stress, ds_data_t data)
{
    stress_iterator_t its[NUM_DS] = { 0 };
    ds_data_t cache = 0;
    ds_count_t cache_count = -1;
    ds_size_t size = stress_first_size(stress);
    char name_buffer[256];
    test_data_t stable = { .name = name_buffer, .id = 0, .others = 0, };

    if (0 == size)
        return true;

    if (0 == data) {
        ds_index_t index = (ds_index_t)(rand() % size);
        test_data_t* src = (test_data_t*)stress[0].interface->at(stress[0].this, index, 0);
        if (NULL == src || strlen(src->name) >= sizeof(name_buffer))
            stress_fail(__LINE__);

        strcpy(name_buffer, src->name);
        data = (ds_data_t)(&stable);
    }

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            its[i] = interface->find(this, data);
            ds_count_t count = interface->count(this, data);
            if (count < 0)
                stress_fail(__LINE__);

            if (-1 == cache_count) {
                cache_count = count;
            } else {
                if (cache_count != count)
                    stress_fail(__LINE__);
            }

            if ((0 == count && stress_it_ne(interface->end(this), its[i])) || (count > 0 && stress_it_eq(interface->end(this), its[i])))
                stress_fail(__LINE__);

            if (count > 0)
                cache = stress_it_data(its[i]);
        }
    }

    if (0 == cache_count)
        return true;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!__demo_cmp_eq(cache, stress_it_data(its[i])))
            stress_fail(__LINE__);

        {
            stress_iterator_t next = interface->next(this, its[i]);
            ds_data_t next_data = stress_it_ne(interface->end(this), next) ? stress_it_data(next) : 0;
            stress_iterator_t erased = interface->erase(this, its[i]);

            if (0 == next_data) {
                if (stress_it_ne(interface->end(this), erased))
                    stress_fail(__LINE__);
            } else {
                if (stress_it_eq(interface->end(this), erased) || !__demo_cmp_eq(next_data, stress_it_data(erased)))
                    stress_fail(__LINE__);
            }

            its[i] = erased;
            if (stress_it_ne(interface->end(this), its[i])) {
                stress_iterator_t nx = interface->next(this, its[i]);
                if (stress_it_ne(interface->end(this), nx))
                    interface->erase_range(this, its[i], interface->next(this, nx));
            }
        }
    }
    return true;
}

static bool test_remove(stress_t* stress, ds_data_t data)
{
    ds_count_t count = -1;
    ds_size_t size = stress_first_size(stress);
    char name_buffer[256];
    test_data_t stable = { .name = name_buffer, .id = 0, .others = 0, };

    if (0 == size)
        return true;

    if (0 == data) {
        ds_index_t index = (ds_index_t)(rand() % size);
        test_data_t* src = (test_data_t*)stress[0].interface->at(stress[0].this, index, 0);
        if (NULL == src || strlen(src->name) >= sizeof(name_buffer))
            stress_fail(__LINE__);

        strcpy(name_buffer, src->name);
        data = (ds_data_t)(&stable);
    }

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        {
            ds_count_t tcount = interface->count(this, data);
            if (tcount < 0)
                stress_fail(__LINE__);

            if (-1 == count) {
                count = tcount;
            } else {
                if (count != tcount)
                    stress_fail(__LINE__);
            }

            if (0 == tcount)
                continue;

            ds_size_t size = interface->size(this);
            ds_size_t tremoved = interface->remove(this, data);
            if (tcount != tremoved || interface->size(this) != size - tremoved)
                stress_fail(__LINE__);
            if (interface->count(this, data) != 0 || stress_it_ne(interface->end(this), interface->find(this, data)))
                stress_fail(__LINE__);
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
            ds_size_t size = interface->size(this);
            ds_size_t tremoved = interface->remove_if(this, cond);
            if (interface->size(this) != size - tremoved)
                stress_fail(__LINE__);

            if (-1 == removed) {
                removed = tremoved;
            } else {
                if (removed != tremoved)
                    stress_fail(__LINE__);
            }

            for (stress_iterator_t it = interface->begin(this); stress_it_ne(interface->end(this), it); it = interface->next(this, it)) {
                if (cond(stress_it_data(it)))
                    stress_fail(__LINE__);
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
    return __demo_cmp_gt(left, right);
}

static bool test_remove_if_half(ds_data_t data)
{
    test_data_t* p = (test_data_t*)data;
    return 0 == (strlen(p->name) & 1);
}

static bool test_unique_if_always(ds_data_t left, ds_data_t right)
{
    (void)left;
    (void)right;
    return true;
}

static bool test_sort(stress_t* stress, bool ascending)
{
    ds_size_t total = test_total_size(stress);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (interface->sort) {
            __cmp rule = ascending ? test_sort_ascending : test_sort_descending;
            ds_size_t size = interface->size(this);

            interface->sort(this, ascending);
            if (interface->size(this) != size)
                stress_fail(__LINE__);

            stress_iterator_t it = interface->begin(this);
            if (stress_it_eq(interface->end(this), it))
                continue;

            ds_data_t data = stress_it_data(it);
            for (it = interface->next(this, it); stress_it_ne(interface->end(this), it); it = interface->next(this, it)) {
                if (rule(stress_it_data(it), data))
                    stress_fail(__LINE__);
                data = stress_it_data(it);
            }
            pr_test("Sort successfully [ %s ]!", ascending ? "ascending" : "descending");
        }
    }

    if (total > 0)
        test_is_same(stress);
    return true;
}

static bool test_pop_back(stress_t* stress)
{
    ds_size_t size = stress_first_size(stress);

    if (0 == size)
        return true;
    if (!stress_sizes_same(stress, size))
        stress_fail(__LINE__);

    ds_data_t cache = 0;
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_data_t data = interface->back(this, 0);
        if (0 == data)
            stress_fail(__LINE__);

        if (0 == cache)
            cache = data;
        else if (!__demo_cmp_eq(cache, data))
            stress_fail(__LINE__);
    }

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        interface->pop_back(this);
        if (interface->size(this) != size - 1)
            stress_fail(__LINE__);
    }

    if (size > 1) {
        ds_data_t first = 0;
        for (int i = 0; stress[i].this; ++i) {
            void* this = stress[i].this;
            const class_stress_slow_interface_t* interface = stress[i].interface;
            if (!interface)
                continue;

            ds_data_t data = interface->back(this, 0);
            if (0 == first)
                first = data;
            else if (!__demo_cmp_eq(first, data))
                stress_fail(__LINE__);
        }
    }
    return true;
}

static bool test_pop_front(stress_t* stress)
{
    ds_size_t size = stress_first_size(stress);

    if (0 == size)
        return true;
    if (!stress_sizes_same(stress, size))
        stress_fail(__LINE__);

    ds_data_t cache = 0;
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_data_t data = interface->front(this, 0);
        if (0 == data)
            stress_fail(__LINE__);

        if (0 == cache)
            cache = data;
        else if (!__demo_cmp_eq(cache, data))
            stress_fail(__LINE__);
    }

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        stress_pop_front(this, interface);
        if (interface->size(this) != size - 1)
            stress_fail(__LINE__);
    }

    if (size > 1) {
        ds_data_t first = 0;
        for (int i = 0; stress[i].this; ++i) {
            void* this = stress[i].this;
            const class_stress_slow_interface_t* interface = stress[i].interface;
            if (!interface)
                continue;

            ds_data_t data = interface->front(this, 0);
            if (0 == first)
                first = data;
            else if (!__demo_cmp_eq(first, data))
                stress_fail(__LINE__);
        }
    }
    return true;
}

static bool test_replace(stress_t* stress, ds_data_t data)
{
    ds_size_t size = stress_first_size(stress);

    if (0 == size)
        return true;
    if (!stress_sizes_same(stress, size))
        stress_fail(__LINE__);

    ds_index_t index = (ds_index_t)(rand() % size);
    ds_data_t cache = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_data_t data_at = interface->at(this, index, 0);
        ds_data_t data_it = stress_it_data(interface->it(this, index));
        if (0 == data_at || !__demo_cmp_eq(data_at, data_it))
            stress_fail(__LINE__);

        if (0 == cache)
            cache = data_at;
        else if (!__demo_cmp_eq(cache, data_at))
            stress_fail(__LINE__);
    }

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!interface->replace_it(this, interface->it(this, index), data))
            goto err;
        if (!__demo_cmp_eq(interface->at(this, index, 0), data))
            stress_fail(__LINE__);

        if (!interface->replace_index(this, index, data))
            goto err;
        if (!__demo_cmp_eq(interface->at(this, index, 0), data))
            stress_fail(__LINE__);
    }

    test_is_same(stress);
    return true;

err:
    stress_clear_all(stress);
    return false;
}

static bool test_assign_resize(stress_t* stress, ds_data_t data)
{
    ds_size_t n = (ds_size_t)(rand() % 16);
    ds_size_t target = n + (ds_size_t)(rand() % 8);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!interface->assign(this, n, data))
            goto err;
    }
    if (!stress_sizes_same(stress, n) || !stress_data_all_equal(stress, data))
        stress_fail(__LINE__);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!interface->resize(this, target, data))
            goto err;
    }
    if (!stress_sizes_same(stress, target) || !stress_data_all_equal(stress, data))
        stress_fail(__LINE__);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!interface->resize(this, n, data))
            goto err;
    }
    if (!stress_sizes_same(stress, n) || !stress_data_all_equal(stress, data))
        stress_fail(__LINE__);

    stress_clear_all(stress);
    pr_test("Assign and resize [ %zd -> %zd -> %zd ]!", n, target, n);
    return true;

err:
    stress_clear_all(stress);
    return false;
}

static bool test_range(stress_t* stress, stress_t* aux)
{
    if (!fill_aux(aux, 1 + rand() % 8))
        return false;

    test_is_same(aux);
    ds_size_t count = stress_first_size(aux);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        stress_iterator_t it = interface->insert_range(this, interface->end(this),
                                                       interface->begin(aux[i].this), interface->end(aux[i].this));
        if (stress_it_null(it))
            goto err;
    }
    test_is_same(stress);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        for (ds_size_t k = 0; k < count; ++k)
            interface->pop_back(this);
    }
    test_is_same(stress);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        if (!interface->assign_range(this, interface->begin(aux[i].this), interface->end(aux[i].this)))
            goto err;
    }
    test_is_same(stress);

    stress_clear_all(aux);
    return true;

err:
    stress_clear_all(stress);
    stress_clear_all(aux);
    return false;
}

static bool test_swap(stress_t* stress, stress_t* aux)
{
    if (test_total_size(stress) > 1024)
        return true;
    if (!fill_aux(aux, 1 + rand() % 8))
        return false;

    test_is_same(aux);
    ds_size_t active_num = 0;
    ds_size_t aux_num = 0;
    ds_data_t* active_seq = stress_collect(stress, &active_num);
    ds_data_t* aux_seq = stress_collect(aux, &aux_num);

    if ((active_num > 0 && NULL == active_seq) || (aux_num > 0 && NULL == aux_seq))
        goto out;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;
        interface->swap(this, aux[i].this);
    }

    if (!stress_seq_eq(stress, aux_seq, aux_num) || !stress_seq_eq(aux, active_seq, active_num))
        stress_fail(__LINE__);

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;
        interface->swap(this, aux[i].this);
    }

    if (!stress_seq_eq(stress, active_seq, active_num) || !stress_seq_eq(aux, aux_seq, aux_num))
        stress_fail(__LINE__);

out:
    free(active_seq);
    free(aux_seq);
    stress_clear_all(aux);
    return true;
}

static bool test_unique(stress_t* aux)
{
    for (int i = 0; aux[i].this; ++i) {
        void* this = aux[i].this;
        const class_stress_slow_interface_t* interface = aux[i].interface;
        if (!interface || !interface->unique || !interface->unique_if)
            continue;

        char buffer1[128];
        char buffer2[128];
        do {
            int len1 = rand() % (sizeof(buffer1) + 1);
            int len2 = rand() % (sizeof(buffer2) + 1);
            if (len1 < 5) len1 = 5;
            if (len2 < 5) len2 = 5;
            string_rand(buffer1, len1);
            string_rand(buffer2, len2);
        } while (0 == strcmp(buffer1, buffer2));

        test_data_t tmp1 = { .name = buffer1, .id = rand(), };
        test_data_t tmp2 = { .name = buffer2, .id = rand(), };

        interface->clear(this);
        for (int k = 0; k < 3; ++k) {
            if (stress_it_null(interface->push_back(this, (ds_data_t)(&tmp1)))) {
                interface->clear(this);
                return false;
            }
        }
        for (int k = 0; k < 2; ++k) {
            if (stress_it_null(interface->push_back(this, (ds_data_t)(&tmp2)))) {
                interface->clear(this);
                return false;
            }
        }

        if (interface->size(this) != 5)
            stress_fail(__LINE__);

        ds_size_t removed = interface->unique(this);
        if (removed != 3 || interface->size(this) != 2)
            stress_fail(__LINE__);

        {
            stress_iterator_t it = interface->begin(this);
            if (stress_it_eq(interface->end(this), it))
                stress_fail(__LINE__);

            ds_data_t data = stress_it_data(it);
            for (it = interface->next(this, it); stress_it_ne(interface->end(this), it); it = interface->next(this, it)) {
                if (__demo_cmp_eq(data, stress_it_data(it)))
                    stress_fail(__LINE__);
                data = stress_it_data(it);
            }
        }

        removed = interface->unique(this);
        if (removed != 0 || interface->size(this) != 2)
            stress_fail(__LINE__);

        removed = interface->unique_if(this, test_unique_if_always);
        if (removed != 1 || interface->size(this) != 1)
            stress_fail(__LINE__);

        interface->clear(this);
        pr_test("Unique successfully!");
    }
    return true;
}

static bool test_extra(stress_t* stress, stress_t* aux)
{
    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_size_t size = interface->size(this);
        if (interface->max_size(this) < size)
            stress_fail(__LINE__);

        if (interface->capacity) {
            ds_size_t capacity = interface->capacity(this);
            if (capacity < size)
                stress_fail(__LINE__);

            if (interface->reserve(this, size + 16) && interface->capacity(this) < size + 16)
                stress_fail(__LINE__);
            if (interface->size(this) != size)
                stress_fail(__LINE__);
        }

        if (interface->shrink_to_fit) {
            interface->shrink_to_fit(this);
            if (interface->size(this) != size)
                stress_fail(__LINE__);
        }

        if (interface->reverse && size > 0) {
            ds_data_t first = interface->front(this, 0);
            ds_data_t last = interface->back(this, 0);

            interface->reverse(this);
            if (interface->size(this) != size
                || !__demo_cmp_eq(interface->front(this, 0), last)
                || !__demo_cmp_eq(interface->back(this, 0), first))
                stress_fail(__LINE__);

            interface->reverse(this);
            if (interface->size(this) != size
                || !__demo_cmp_eq(interface->front(this, 0), first)
                || !__demo_cmp_eq(interface->back(this, 0), last))
                stress_fail(__LINE__);
            pr_test("Reverse successfully!");
        }
    }

    return test_unique(aux);
}

static bool test_clear(stress_t* stress)
{
    ds_size_t total = 0;

    for (int i = 0; stress[i].this; ++i) {
        void* this = stress[i].this;
        const class_stress_slow_interface_t* interface = stress[i].interface;
        if (!interface)
            continue;

        ds_size_t size = interface->size(this);
        ds_size_t num = interface->clear(this);
        if (num != size || interface->size(this) != 0 || !interface->empty(this)
            || stress_it_ne(interface->end(this), interface->begin(this))) {
            pr_test("clear c[%d] size[%zd] num[%zd] empty[%d] begin[%p] end[%p]",
                    i, size, num, interface->empty(this), interface->begin(this).d, interface->end(this).d);
            stress_fail(__LINE__);
        }
        total += num;
    }

    pr_test("Clear [ %zd ] elements!", total);
    return true;
}

int main(int argc, char** argv)
{
    class_vector_ops_t vector_ops = {
        .valid_data = demo_valid_key,
        .__eq       = __demo_cmp_eq,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    vector_t* vector = NULL;
    vector_t* vector_aux = NULL;
    STRESS_NEW(vector, VECTOR_NEW_OPS(&vector_ops));
    STRESS_NEW(vector_aux, VECTOR_NEW_OPS(&vector_ops));

    class_list_ops_t list_ops = {
        .valid_data = demo_valid_key,
        .__eq       = __demo_cmp_eq,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    list_t* list = NULL;
    list_t* list_aux = NULL;
    STRESS_NEW(list, LIST_NEW_OPS(&list_ops));
    STRESS_NEW(list_aux, LIST_NEW_OPS(&list_ops));

    class_deque_ops_t deque_ops = {
        .valid_data = demo_valid_key,
        .__eq       = __demo_cmp_eq,
        .copy_data  = demo_copy_key,
        .free_data  = demo_free_key,
    };
    deque_t* deque = NULL;
    deque_t* deque_aux = NULL;
    STRESS_NEW(deque, DEQUE_NEW_OPS(&deque_ops));
    STRESS_NEW(deque_aux, DEQUE_NEW_OPS(&deque_ops));

    stress_t stress[NUM_DS] = { (stress_t){ .this = vector, .interface = cs_vector },
                                (stress_t){ .this = list,   .interface = cs_list },
                                (stress_t){ .this = deque,  .interface = cs_deque },
                                (stress_t){ .this = NULL,   .interface = NULL },
                                };
    stress_t aux[NUM_DS] = { (stress_t){ .this = vector_aux, .interface = cs_vector },
                             (stress_t){ .this = list_aux,   .interface = cs_list },
                             (stress_t){ .this = deque_aux,  .interface = cs_deque },
                             (stress_t){ .this = NULL,       .interface = NULL },
                             };

    srand(time(0));
    char buffer[128];
    long times = 0;
    long limit = argc > 1 ? atoi(argv[1]) : 1000000;

    for (; times < limit; ++times) {
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
        g_stress_times = times;
        g_stress_op = curr_ops;
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
            if (curr_ops < 100) {
                test_sort(stress, curr_ops % 2);
                test_clear(stress);
            } else if (curr_ops < 1000)
                test_pop_front(stress);
            else if (curr_ops < 10000)
                test_pop_back(stress);
            else if (curr_ops < 20000)
                test_remove(stress, (curr_ops % 2) ? 0 : (ds_data_t)(&tmp));
            else if (curr_ops < 30000)
                test_find_and_erase(stress, (curr_ops % 2) ? 0 : (ds_data_t)(&tmp));
            else if (curr_ops < 40000)
                test_replace(stress, (ds_data_t)(&tmp));
            else if (curr_ops < 50000)
                test_assign_resize(stress, (ds_data_t)(&tmp));
            else if (curr_ops < 60000)
                test_range(stress, aux);
            else if (curr_ops < 70000)
                test_swap(stress, aux);
            else if (curr_ops < 80000)
                test_extra(stress, aux);
            else if (curr_ops < 90000)
                test_remove_if(stress, (curr_ops % 2) ? test_remove_if_cond : test_remove_if_half);
            else if (curr_ops < 180000)
                test_insert_n(stress, (ds_data_t)(&tmp));
            else
                test_insert(stress, (ds_data_t)(&tmp));
        }
            break;
        }

        if (0 == times % 1000)
            test_is_same(stress);
    }

    VECTOR_DELETE(&vector);
    VECTOR_DELETE(&vector_aux);
    LIST_DELETE(&list);
    LIST_DELETE(&list_aux);
    DEQUE_DELETE(&deque);
    DEQUE_DELETE(&deque_aux);

    return 0;
}
