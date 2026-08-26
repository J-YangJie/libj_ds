/*
  Multimap Interfaces
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

#ifndef __J_MULTIMAP_H
#define __J_MULTIMAP_H

#include <multimap/multimap_ops.h>
#include <iterator/iterator.h>

typedef struct multimap_iterator_kv {
    union {
        multimap_key_t key;
        char* skey;
    };
    union {
        multimap_value_t value;
        char* svalue;
    };
} multimap_iterator_kv_t;

typedef struct multimap_iterator {
    union {
        multimap_iterator_kv_t* d;
    };
} multimap_iterator_t;

typedef struct multimap_reverse_iterator_kv {
    union {
        multimap_key_t key;
        char* skey;
    };
    union {
        multimap_value_t value;
        char* svalue;
    };
} multimap_reverse_iterator_kv_t;
typedef multimap_reverse_iterator_kv_t multimap_r_iterator_kv_t;

typedef struct multimap_reverse_iterator {
    union {
        multimap_reverse_iterator_kv_t* d;
    };
} multimap_reverse_iterator_t;
typedef multimap_reverse_iterator_t multimap_r_iterator_t;

typedef struct multimap multimap_t;

/* Method 1 */
multimap_size_t        cmultimap_size(const multimap_t* _this);
multimap_count_t       cmultimap_count(const multimap_t* _this, multimap_key_t key);
multimap_iterator_t    cmultimap_end(const multimap_t* _this);
multimap_iterator_t    cmultimap_begin(const multimap_t* _this);
multimap_iterator_t    cmultimap_next(const multimap_t* _this, const multimap_iterator_t iterator);
multimap_iterator_t    cmultimap_prev(const multimap_t* _this, const multimap_iterator_t iterator);
multimap_r_iterator_t  cmultimap_rend(const multimap_t* _this);
multimap_r_iterator_t  cmultimap_rbegin(const multimap_t* _this);
multimap_r_iterator_t  cmultimap_rnext(const multimap_t* _this, const multimap_r_iterator_t r_iterator);
multimap_r_iterator_t  cmultimap_rprev(const multimap_t* _this, const multimap_r_iterator_t r_iterator);
multimap_iterator_t    cmultimap_find(const multimap_t* _this, multimap_key_t key);
multimap_iterator_t    cmultimap_lower_bound(const multimap_t* _this, multimap_key_t key);
multimap_iterator_t    cmultimap_upper_bound(const multimap_t* _this, multimap_key_t key);
multimap_iterator_t    cmultimap_insert(multimap_t* _this, multimap_key_t key, multimap_value_t value);
multimap_iterator_t    cmultimap_erase(multimap_t* _this, multimap_iterator_t iterator);
multimap_size_t        cmultimap_remove(multimap_t* _this, multimap_key_t key);
multimap_size_t        cmultimap_remove_if(multimap_t* _this, remove_if_condition_kv cond);
multimap_size_t        cmultimap_clear(multimap_t* _this);

/* Method 2 */
typedef struct class_multimap {
    multimap_size_t (*size)(const multimap_t* _this);
    multimap_count_t (*count)(const multimap_t* _this, multimap_key_t key);
    multimap_iterator_t (*end)(const multimap_t* _this);
    multimap_iterator_t (*begin)(const multimap_t* _this);
    multimap_iterator_t (*next)(const multimap_t* _this, const multimap_iterator_t iterator);
    multimap_iterator_t (*prev)(const multimap_t* _this, const multimap_iterator_t iterator);
    multimap_r_iterator_t (*rend)(const multimap_t* _this);
    multimap_r_iterator_t (*rbegin)(const multimap_t* _this);
    multimap_r_iterator_t (*rnext)(const multimap_t* _this, const multimap_r_iterator_t r_iterator);
    multimap_r_iterator_t (*rprev)(const multimap_t* _this, const multimap_r_iterator_t r_iterator);
    multimap_iterator_t (*find)(const multimap_t* _this, multimap_key_t key);
    multimap_iterator_t (*lower_bound)(const multimap_t* _this, multimap_key_t key);              /* >= key */
    multimap_iterator_t (*upper_bound)(const multimap_t* _this, multimap_key_t key);              /*  > key */
    multimap_iterator_t (*insert)(multimap_t* _this, multimap_key_t key, multimap_value_t value); /* if input key doesn't match -> insert | if input key match -> return NULL */
    multimap_iterator_t (*erase)(multimap_t* _this, multimap_iterator_t iterator);
    multimap_size_t (*remove)(multimap_t* _this, multimap_key_t key);
    multimap_size_t (*remove_if)(multimap_t* _this, remove_if_condition_kv cond);
    multimap_size_t (*clear)(multimap_t* _this);
} class_multimap_t;

multimap_t* __multimap_new(const class_multimap_ops_t* ops);
void        __multimap_delete(multimap_t** _this);
const class_multimap_t* class_multimap_ins(void);
#define g_class_multimap()      class_multimap_ins()
#define cmultimap               g_class_multimap()
#define MULTIMAP_NEW()          __multimap_new(NULL)
#define MULTIMAP_NEW_OPS(_ops)  __multimap_new((_ops))
#define MULTIMAP_DELETE(_pptr)  do { __multimap_delete((_pptr)); } while(0)

#endif /* __J_MULTIMAP_H */
