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

#include <stdlib.h>
#include <_types.h>
#include <operations/ds_ops_string.h>

#define p_malloc(X) malloc(X)
#define p_free(X)   free(X)
#define is_null(X)  (!(X))

typedef struct {
    char* name;
    int id;
    int others;
} test_data_t;

static inline ds_hash_t __demo_hash(ds_key_t key)
{
    test_data_t* p = (test_data_t*)key;
    return __ds_ops_hash_default_string((ds_key_t)(p->name));
}

static inline bool demo_valid_key(ds_key_t key)
{
    test_data_t* p = (test_data_t*)key;
    return !is_null(p) && !is_null(p->name) && strlen(p->name) > 0;
}

static inline bool __demo_cmp(ds_data_t left, ds_data_t right)
{
    test_data_t* l = (test_data_t*)left;
    test_data_t* r = (test_data_t*)right;
    return __ds_ops_gt_default_string((ds_data_t)(l->name), (ds_data_t)(r->name));
}

static inline bool __demo_cmp_lt(ds_data_t left, ds_data_t right)
{
    test_data_t* l = (test_data_t*)left;
    test_data_t* r = (test_data_t*)right;
    return __ds_ops_lt_default_string((ds_data_t)(l->name), (ds_data_t)(r->name));
}

static inline bool demo_copy_key(ds_key_t in, ds_key_t* out)
{
    test_data_t* i  = (test_data_t*)in;
    test_data_t** o = (test_data_t**)out;

    if (is_null(i) || is_null(o) || is_null(i->name) || strlen(i->name) <= 0)
        return false;

    *o = (test_data_t*)p_malloc(sizeof(test_data_t));
    if (is_null(*o))
        return false;

    (*o)->name = (char*)p_malloc(strlen(i->name) + 1);
    if (is_null((*o)->name))
        goto err;

    strcpy((*o)->name, i->name);
    return true;

err:
    p_free(*o);
    return false;
}

static inline void demo_free_key(ds_key_t* key)
{
    test_data_t** p = (test_data_t**)key;

    if (is_null(p) || is_null(*p))
        return ;

    p_free((*p)->name);
    p_free(*p);
}

static inline bool test_remove_if_cond(ds_data_t data)
{
    test_data_t* p = (test_data_t*)data;
    char target[5] = { "yj" };
    return NULL != strstr(p->name, target);
}
