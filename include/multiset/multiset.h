/*
  Multiset Interfaces
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

#ifndef __J_MULTISET_H
#define __J_MULTISET_H

#include <iterator/iterator.h>
#include <associative_container/associative_container.h>

typedef class_associative_container_key_ops_t class_multiset_ops_t;

typedef struct multiset_iterator {
    union {
        multiset_data_t* d;
        char** sd;
    };
    void* private_data;
} multiset_iterator_t;

typedef struct multiset_reverse_iterator {
    union {
        multiset_data_t* d;
        char** sd;
    };
    void* private_data;
} multiset_reverse_iterator_t;
typedef multiset_reverse_iterator_t multiset_r_iterator_t;

typedef struct multiset multiset_t;

/* Method 1 */
multiset_size_t        cmultiset_size(const multiset_t* _this);
multiset_count_t       cmultiset_count(const multiset_t* _this, multiset_key_t key);
multiset_iterator_t    cmultiset_end(const multiset_t* _this);
multiset_iterator_t    cmultiset_begin(const multiset_t* _this);
multiset_iterator_t    cmultiset_next(const multiset_iterator_t iterator);
multiset_iterator_t    cmultiset_prev(const multiset_iterator_t iterator);
multiset_r_iterator_t  cmultiset_rend(const multiset_t* _this);
multiset_r_iterator_t  cmultiset_rbegin(const multiset_t* _this);
multiset_r_iterator_t  cmultiset_rnext(const multiset_r_iterator_t r_iterator);
multiset_r_iterator_t  cmultiset_rprev(const multiset_r_iterator_t r_iterator);
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
    multiset_iterator_t (*next)(const multiset_iterator_t iterator);
    multiset_iterator_t (*prev)(const multiset_iterator_t iterator);
    multiset_r_iterator_t (*rend)(const multiset_t* _this);
    multiset_r_iterator_t (*rbegin)(const multiset_t* _this);
    multiset_r_iterator_t (*rnext)(const multiset_r_iterator_t r_iterator);
    multiset_r_iterator_t (*rprev)(const multiset_r_iterator_t r_iterator);
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
#define MULTISET_DELETE(_pptr)  do { __multiset_delete((_pptr)); } while(0)

/*
    Keys are ordered ascending by default.

    _type includes:
    1. uint8_t/uint16_t/uint32_t/uint64_t
    2. int8_t/int16_t/int32_t/int64_t
    3. uintptr_t/intptr_t/size_t/ssize_t
    4. float/double/string

    MULTISET_NEW_T(_type) like std::multiset<_type>
    Such as: multiset_t* demo = MULTISET_NEW_T(string);
    Such as: multiset_t* demo = MULTISET_NEW_T(int16_t);
    Such as: multiset_t* demo = MULTISET_NEW_T(float);
 */
#define MULTISET_NEW_T(_type)     MULTISET_NEW_OPS(g_class_associative_container_key_ops_##_type())

/*
    Keys are ordered descending.

    _type includes:
    1. uint8_t/uint16_t/uint32_t/uint64_t
    2. int8_t/int16_t/int32_t/int64_t
    3. uintptr_t/intptr_t/size_t/ssize_t
    4. float/double/string

    MULTISET_NEW_T_GT(_type) like std::multiset<_type, std::greater<_type>>
    Such as: multiset_t* demo = MULTISET_NEW_T_GT(string);
    Such as: multiset_t* demo = MULTISET_NEW_T_GT(int16_t);
    Such as: multiset_t* demo = MULTISET_NEW_T_GT(float);
 */
#define MULTISET_NEW_T_GT(_type)  MULTISET_NEW_OPS(g_class_associative_container_key_ops_##_type##_greater())

#endif /* __J_MULTISET_H */
