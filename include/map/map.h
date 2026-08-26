/*
  Map Interfaces
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

#ifndef __J_MAP_H
#define __J_MAP_H

#include <map/map_ops.h>
#include <iterator/iterator.h>

typedef struct map_iterator_kv {
    union {
        map_key_t key;
        char* skey;
    };
    union {
        map_value_t value;
        char* svalue;
    };
} map_iterator_kv_t;

typedef struct map_iterator {
    union {
        map_iterator_kv_t* d;
    };
} map_iterator_t;

typedef struct map_reverse_iterator_kv {
    union {
        map_key_t key;
        char* skey;
    };
    union {
        map_value_t value;
        char* svalue;
    };
} map_reverse_iterator_kv_t;
typedef map_reverse_iterator_kv_t map_r_iterator_kv_t;

typedef struct map_reverse_iterator {
    union {
        map_reverse_iterator_kv_t* d;
    };
} map_reverse_iterator_t;
typedef map_reverse_iterator_t map_r_iterator_t;

typedef struct map map_t;

/* Method 1 */
map_size_t        cmap_size(const map_t* _this);
map_count_t       cmap_count(const map_t* _this, map_key_t key);
map_iterator_t    cmap_end(const map_t* _this);
map_iterator_t    cmap_begin(const map_t* _this);
map_iterator_t    cmap_next(const map_t* _this, const map_iterator_t iterator);
map_iterator_t    cmap_prev(const map_t* _this, const map_iterator_t iterator);
map_r_iterator_t  cmap_rend(const map_t* _this);
map_r_iterator_t  cmap_rbegin(const map_t* _this);
map_r_iterator_t  cmap_rnext(const map_t* _this, const map_r_iterator_t r_iterator);
map_r_iterator_t  cmap_rprev(const map_t* _this, const map_r_iterator_t r_iterator);
map_iterator_t    cmap_find(const map_t* _this, map_key_t key);
map_iterator_t    cmap_lower_bound(const map_t* _this, map_key_t key);
map_iterator_t    cmap_upper_bound(const map_t* _this, map_key_t key);
map_iterator_t    cmap_insert(map_t* _this, map_key_t key, map_value_t value);
map_iterator_t    cmap_insert_replace(map_t* _this, map_key_t key, map_value_t value);
map_iterator_t    cmap_erase(map_t* _this, map_iterator_t iterator);
map_size_t        cmap_remove(map_t* _this, map_key_t key);
map_size_t        cmap_remove_if(map_t* _this, remove_if_condition_kv cond);
map_size_t        cmap_clear(map_t* _this);

/* Method 2 */
typedef struct class_map {
    map_size_t (*size)(const map_t* _this);
    map_count_t (*count)(const map_t* _this, map_key_t key);
    map_iterator_t (*end)(const map_t* _this);
    map_iterator_t (*begin)(const map_t* _this);
    map_iterator_t (*next)(const map_t* _this, const map_iterator_t iterator);
    map_iterator_t (*prev)(const map_t* _this, const map_iterator_t iterator);
    map_r_iterator_t (*rend)(const map_t* _this);
    map_r_iterator_t (*rbegin)(const map_t* _this);
    map_r_iterator_t (*rnext)(const map_t* _this, const map_r_iterator_t r_iterator);
    map_r_iterator_t (*rprev)(const map_t* _this, const map_r_iterator_t r_iterator);
    map_iterator_t (*find)(const map_t* _this, map_key_t key);
    map_iterator_t (*lower_bound)(const map_t* _this, map_key_t key);                 /* >= key */
    map_iterator_t (*upper_bound)(const map_t* _this, map_key_t key);                 /*  > key */
    map_iterator_t (*insert)(map_t* _this, map_key_t key, map_value_t value);         /* if input key doesn't match -> insert | if input key match -> return NULL */
    map_iterator_t (*insert_replace)(map_t* _this, map_key_t key, map_value_t value); /* if input key doesn't match -> insert | if input key match -> replace value (Refer to C++11 a[key] = value) */
    map_iterator_t (*erase)(map_t* _this, map_iterator_t iterator);
    map_size_t (*remove)(map_t* _this, map_key_t key);
    map_size_t (*remove_if)(map_t* _this, remove_if_condition_kv cond);
    map_size_t (*clear)(map_t* _this);
} class_map_t;

map_t* __map_new(const class_map_ops_t* ops);
void   __map_delete(map_t** _this);
const class_map_t* class_map_ins(void);
#define g_class_map()      class_map_ins()
#define cmap               g_class_map()
#define MAP_NEW()          __map_new(NULL)
#define MAP_NEW_OPS(_ops)  __map_new((_ops))
#define MAP_DELETE(_pptr)  do { __map_delete((_pptr)); } while(0)

#endif /* __J_MAP_H */
