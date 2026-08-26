/*
  Multiset Interfaces
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

#ifndef __J_MULTISET_H
#define __J_MULTISET_H

#include <multiset/multiset_ops.h>
#include <iterator/iterator.h>

typedef struct multiset_iterator {
    union {
        multiset_data_t* d;
        char** sd;
    };
} multiset_iterator_t;

typedef struct multiset_reverse_iterator {
    union {
        multiset_data_t* d;
        char** sd;
    };
} multiset_reverse_iterator_t;
typedef multiset_reverse_iterator_t multiset_r_iterator_t;

typedef struct multiset multiset_t;

/* Method 1 */
multiset_size_t        cmultiset_size(const multiset_t* _this);
multiset_count_t       cmultiset_count(const multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_end(const multiset_t* _this);
multiset_iterator_t    cmultiset_begin(const multiset_t* _this);
multiset_iterator_t    cmultiset_next(const multiset_t* _this, const multiset_iterator_t iterator);
multiset_iterator_t    cmultiset_prev(const multiset_t* _this, const multiset_iterator_t iterator);
multiset_r_iterator_t  cmultiset_rend(const multiset_t* _this);
multiset_r_iterator_t  cmultiset_rbegin(const multiset_t* _this);
multiset_r_iterator_t  cmultiset_rnext(const multiset_t* _this, const multiset_r_iterator_t r_iterator);
multiset_r_iterator_t  cmultiset_rprev(const multiset_t* _this, const multiset_r_iterator_t r_iterator);
multiset_iterator_t    cmultiset_find(const multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_lower_bound(const multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_upper_bound(const multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_insert(multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_erase(multiset_t* _this, multiset_iterator_t iterator);
multiset_size_t        cmultiset_remove(multiset_t* _this, multiset_key_t key);
multiset_size_t        cmultiset_remove_if(multiset_t* _this, remove_if_condition_k cond);
multiset_size_t        cmultiset_clear(multiset_t* _this);

/* Method 2 */
typedef struct class_multiset {
    multiset_size_t (*size)(const multiset_t* _this);
    multiset_count_t (*count)(const multiset_t* _this, multiset_key_t key);
    multiset_iterator_t (*end)(const multiset_t* _this);
    multiset_iterator_t (*begin)(const multiset_t* _this);
    multiset_iterator_t (*next)(const multiset_t* _this, const multiset_iterator_t iterator);
    multiset_iterator_t (*prev)(const multiset_t* _this, const multiset_iterator_t iterator);
    multiset_r_iterator_t (*rend)(const multiset_t* _this);
    multiset_r_iterator_t (*rbegin)(const multiset_t* _this);
    multiset_r_iterator_t (*rnext)(const multiset_t* _this, const multiset_r_iterator_t r_iterator);
    multiset_r_iterator_t (*rprev)(const multiset_t* _this, const multiset_r_iterator_t r_iterator);
    multiset_iterator_t (*find)(const multiset_t* _this, multiset_key_t key);
    multiset_iterator_t (*lower_bound)(const multiset_t* _this, multiset_key_t key); /* >= key */
    multiset_iterator_t (*upper_bound)(const multiset_t* _this, multiset_key_t key); /*  > key */
    multiset_iterator_t (*insert)(multiset_t* _this, multiset_key_t key);            /* if input key doesn't match -> insert | if input key match -> return NULL */
    multiset_iterator_t (*erase)(multiset_t* _this, multiset_iterator_t iterator);
    multiset_size_t (*remove)(multiset_t* _this, multiset_key_t key);
    multiset_size_t (*remove_if)(multiset_t* _this, remove_if_condition_k cond);
    multiset_size_t (*clear)(multiset_t* _this);
} class_multiset_t;

multiset_t* __multiset_new(const class_multiset_ops_t* ops);
void        __multiset_delete(multiset_t** _this);
const class_multiset_t* class_multiset_ins(void);
#define g_class_multiset()      class_multiset_ins()
#define cmultiset               g_class_multiset()
#define MULTISET_NEW()          __multiset_new(NULL)
#define MULTISET_NEW_OPS(_ops)  __multiset_new((_ops))
#define MULTISET_NEW_STRING()   __multiset_new(g_class_multiset_ops_string())
#define MULTISET_DELETE(_pptr)  do { __multiset_delete((_pptr)); } while(0)

#endif /* __J_MULTISET_H */
