/*
  Set Interfaces
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

#ifndef __J_SET_H
#define __J_SET_H

#include <set/set_ops.h>
#include <iterator/iterator.h>

typedef struct set_iterator {
    union {
        set_data_t* d;
        char** sd;
    };
} set_iterator_t;

typedef struct set_reverse_iterator {
    union {
        set_data_t* d;
        char** sd;
    };
} set_reverse_iterator_t;
typedef set_reverse_iterator_t set_r_iterator_t;

typedef struct set set_t;

/* Method 1 */
set_size_t        cset_size(const set_t* _this);
set_count_t       cset_count(const set_t* _this, set_key_t key);
set_iterator_t    cset_end(const set_t* _this);
set_iterator_t    cset_begin(const set_t* _this);
set_iterator_t    cset_next(const set_t* _this, const set_iterator_t iterator);
set_iterator_t    cset_prev(const set_t* _this, const set_iterator_t iterator);
set_r_iterator_t  cset_rend(const set_t* _this);
set_r_iterator_t  cset_rbegin(const set_t* _this);
set_r_iterator_t  cset_rnext(const set_t* _this, const set_r_iterator_t r_iterator);
set_r_iterator_t  cset_rprev(const set_t* _this, const set_r_iterator_t r_iterator);
set_iterator_t    cset_find(const set_t* _this, set_key_t key);
set_iterator_t    cset_lower_bound(const set_t* _this, set_key_t key);
set_iterator_t    cset_upper_bound(const set_t* _this, set_key_t key);
set_iterator_t    cset_insert(set_t* _this, set_key_t key);
set_iterator_t    cset_erase(set_t* _this, set_iterator_t iterator);
set_size_t        cset_remove(set_t* _this, set_key_t key);
set_size_t        cset_remove_if(set_t* _this, remove_if_condition_k cond);
set_size_t        cset_clear(set_t* _this);

/* Method 2 */
typedef struct class_set {
    set_size_t (*size)(const set_t* _this);
    set_count_t (*count)(const set_t* _this, set_key_t key);
    set_iterator_t (*end)(const set_t* _this);
    set_iterator_t (*begin)(const set_t* _this);
    set_iterator_t (*next)(const set_t* _this, const set_iterator_t iterator);
    set_iterator_t (*prev)(const set_t* _this, const set_iterator_t iterator);
    set_r_iterator_t (*rend)(const set_t* _this);
    set_r_iterator_t (*rbegin)(const set_t* _this);
    set_r_iterator_t (*rnext)(const set_t* _this, const set_r_iterator_t r_iterator);
    set_r_iterator_t (*rprev)(const set_t* _this, const set_r_iterator_t r_iterator);
    set_iterator_t (*find)(const set_t* _this, set_key_t key);
    set_iterator_t (*lower_bound)(const set_t* _this, set_key_t key); /* >= key */
    set_iterator_t (*upper_bound)(const set_t* _this, set_key_t key); /*  > key */
    set_iterator_t (*insert)(set_t* _this, set_key_t key);            /* if input key doesn't match -> insert | if input key match -> return NULL */
    set_iterator_t (*erase)(set_t* _this, set_iterator_t iterator);
    set_size_t (*remove)(set_t* _this, set_key_t key);
    set_size_t (*remove_if)(set_t* _this, remove_if_condition_k cond);
    set_size_t (*clear)(set_t* _this);
} class_set_t;

set_t* __set_new(const class_set_ops_t* ops);
void   __set_delete(set_t** _this);
const class_set_t* class_set_ins(void);
#define g_class_set()      class_set_ins()
#define cset               g_class_set()
#define SET_NEW()          __set_new(NULL)
#define SET_NEW_OPS(_ops)  __set_new((_ops))
#define SET_NEW_STRING()   __set_new(g_class_set_ops_string())
#define SET_DELETE(_pptr)  do { __set_delete((_pptr)); } while(0)

#endif /* __J_SET_H */
