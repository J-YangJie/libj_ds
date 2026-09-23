/*
  Vector Interfaces
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

#ifndef __J_VECTOR_H
#define __J_VECTOR_H

#include <vector/vector_ops.h>
#include <iterator/iterator.h>
#include <operations/ds_ops_sso.h>

typedef struct vector_value {
    union {
        vector_data_t* d;
        char** sd;
        ds_sso_t* ssd;
        uint8_t*  u8;
        uint16_t* u16;
        uint32_t* u32;
        int8_t*   s8;
        int16_t*  s16;
        int32_t*  s32;
    };
} vector_value_t;

typedef struct vector_iterator {
    union {
        vector_data_t* d;
        char** sd;
        uint8_t* cur;
    };
    vector_step_t step;
} vector_iterator_t;

typedef struct vector_reverse_iterator {
    union {
        vector_data_t* d;
        char** sd;
        uint8_t* cur;
    };
    vector_step_t step;
} vector_reverse_iterator_t;
typedef vector_reverse_iterator_t vector_r_iterator_t;

#include <vector/i_vector.h>

typedef i_vector_t vector_t;

/* Method 1 */
static inline vector_size_t       cvector_size(const vector_t* _this)                                          { return i_vector_size(_this); }
static inline vector_size_t       cvector_capacity(const vector_t* _this)                                      { return i_vector_capacity(_this); }
static inline bool                __cvector_empty(const vector_t* _this)                                       { return __i_vector_empty(_this); }
static inline vector_count_t      cvector_count(const vector_t* _this, vector_data_t data)                     { return i_vector_count(_this, data); }
static inline vector_iterator_t   cvector_end(const vector_t* _this)                                           { return i_vector_end(_this); }
static inline vector_iterator_t   cvector_begin(const vector_t* _this)                                         { return i_vector_begin(_this); }
static inline vector_iterator_t   cvector_next(vector_iterator_t iterator)                                     { return i_vector_next(iterator); }
static inline vector_iterator_t   cvector_prev(vector_iterator_t iterator)                                     { return i_vector_prev(iterator); }
static inline vector_r_iterator_t cvector_rend(const vector_t* _this)                                          { return i_vector_rend(_this); }
static inline vector_r_iterator_t cvector_rbegin(const vector_t* _this)                                        { return i_vector_rbegin(_this); }
static inline vector_r_iterator_t cvector_rnext(vector_r_iterator_t r_iterator)                                { return i_vector_rnext(r_iterator); }
static inline vector_r_iterator_t cvector_rprev(vector_r_iterator_t r_iterator)                                { return i_vector_rprev(r_iterator); }
static inline vector_iterator_t   __cvector_it(const vector_t* _this, vector_size_t n)                         { return __i_vector_iterator(_this, n); }
static inline vector_iterator_t   cvector_it(const vector_t* _this, vector_size_t n)                           { return i_vector_iterator(_this, n); }
static inline vector_value_t      __cvector_at(const vector_t* _this, vector_size_t n)                         { return __i_vector_at(_this, n); }
static inline vector_value_t      cvector_at(const vector_t* _this, vector_size_t n)                           { return i_vector_at(_this, n); }
static inline vector_value_t      cvector_back(const vector_t* _this)                                          { return i_vector_back(_this); }
static inline vector_value_t      cvector_front(const vector_t* _this)                                         { return i_vector_front(_this); }
static inline vector_iterator_t   cvector_find(const vector_t* _this, vector_data_t data)                      { return i_vector_find(_this, data); }
static inline bool                cvector_push_back(vector_t* _this, vector_data_t data)                       { return i_vector_push_back(_this, data); }
static inline vector_iterator_t   cvector_insert(vector_t* _this, vector_iterator_t pos, vector_data_t data)   { return i_vector_insert(_this, pos, data); }
static inline vector_iterator_t   cvector_insert_n(vector_t* _this, vector_iterator_t pos, vector_size_t n, vector_data_t data) { return i_vector_insert_n(_this, pos, n, data); }
static inline vector_iterator_t   cvector_erase(vector_t* _this, vector_iterator_t pos)                        { return i_vector_erase(_this, pos); }
static inline vector_iterator_t   cvector_erase_range(vector_t* _this, vector_iterator_t iterator_begin, vector_iterator_t iterator_end) { return i_vector_erase_range(_this, iterator_begin, iterator_end); }
static inline void                cvector_pop_back(vector_t* _this)                                            { i_vector_pop_back(_this); }
static inline vector_size_t       cvector_remove(vector_t* _this, vector_data_t data)                          { return i_vector_remove(_this, data); }
static inline vector_size_t       cvector_remove_if(vector_t* _this, remove_if_condition cond)                 { return i_vector_remove_if(_this, cond); }
static inline bool                cvector_resize(vector_t* _this, vector_size_t n, vector_data_t default_data) { return i_vector_resize(_this, n, default_data); }
static inline bool                cvector_reserve(vector_t* _this, vector_size_t n)                            { return i_vector_reserve(_this, n); }
static inline void                cvector_shrink_to_fit(vector_t* _this)                                       { i_vector_shrink_to_fit(_this); }
static inline vector_size_t       cvector_clear(vector_t* _this)                                               { return i_vector_clear(_this); }

/* Method 2 */
typedef struct class_vector {
    vector_size_t (*size)(const vector_t* _this);
    vector_size_t (*capacity)(const vector_t* _this);
    bool (*__empty)(const vector_t* _this);
    vector_count_t (*count)(const vector_t* _this, vector_data_t data);
    vector_iterator_t (*end)(const vector_t* _this);
    vector_iterator_t (*begin)(const vector_t* _this);
    vector_iterator_t (*next)(vector_iterator_t iterator);
    vector_iterator_t (*prev)(vector_iterator_t iterator);
    vector_r_iterator_t (*rend)(const vector_t* _this);
    vector_r_iterator_t (*rbegin)(const vector_t* _this);
    vector_r_iterator_t (*rnext)(vector_r_iterator_t r_iterator);
    vector_r_iterator_t (*rprev)(vector_r_iterator_t r_iterator);
    vector_iterator_t (*__it)(const vector_t* _this, vector_size_t n);
    vector_iterator_t (*it)(const vector_t* _this, vector_size_t n);
    vector_value_t (*__at)(const vector_t* _this, vector_size_t n);
    vector_value_t (*at)(const vector_t* _this, vector_size_t n);
    vector_value_t (*back)(const vector_t* _this);
    vector_value_t (*front)(const vector_t* _this);
    vector_iterator_t (*find)(const vector_t* _this, vector_data_t data);
    bool (*push_back)(vector_t* _this, vector_data_t data);
    vector_iterator_t (*insert)(vector_t* _this, vector_iterator_t iterator, vector_data_t data);                        /* insert data before `iterator` */
    vector_iterator_t (*insert_n)(vector_t* _this, vector_iterator_t iterator, vector_size_t n, vector_data_t data);
    vector_iterator_t (*erase)(vector_t* _this, vector_iterator_t iterator);
    vector_iterator_t (*erase_range)(vector_t* _this, vector_iterator_t iterator_begin, vector_iterator_t iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(vector_t* _this);
    vector_size_t (*remove)(vector_t* _this, vector_data_t data);
    vector_size_t (*remove_if)(vector_t* _this, remove_if_condition cond);
    bool (*resize)(vector_t* _this, vector_size_t n, vector_data_t default_data);
    bool (*reserve)(vector_t* _this, vector_size_t n);
    void (*shrink_to_fit)(vector_t* _this);
    vector_size_t (*clear)(vector_t* _this);
} class_vector_t;

vector_t* __vector_new(const class_vector_ops_t* ops, vector_step_t step, vector_size_t capacity_init);
void      __vector_delete(vector_t** _this);
const class_vector_t* class_vector_ins(void);
#define g_class_vector()              class_vector_ins()
#define cvector                       g_class_vector()
#define VECTOR_NEW()                  __vector_new(NULL, sizeof(vector_data_t), 0)
#define VECTOR_NEW_OPS(_ops)          __vector_new((_ops), sizeof(vector_data_t), 0)
#define VECTOR_NEW_T(_type)           __vector_new(NULL, sizeof(_type), 0)
#define VECTOR_NEW_OPS_T(_ops, _type) __vector_new((_ops), sizeof(_type), 0)
#define VECTOR_NEW_CHAR()             __vector_new(g_class_vector_ops_string(), sizeof(vector_data_t), 0)
#define VECTOR_NEW_SSO()              __vector_new(g_class_vector_ops_sso(), sizeof(ds_sso_t), 0)
#define VECTOR_NEW_STRING()           VECTOR_NEW_SSO()
#define VECTOR_DELETE(_pptr)          do { __vector_delete((_pptr)); } while(0)

#endif /* __J_VECTOR_H */
