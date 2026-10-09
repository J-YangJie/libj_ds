/*
  List Interfaces
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

#ifndef __J_LIST_H
#define __J_LIST_H

#include <list/list_ops.h>
#include <iterator/iterator.h>
#include <operations/ds_ops_sso.h>

typedef struct list_value {
    union {
        list_data_t* d;
        char** sd;
        ds_sso_t* ssd;
        uint8_t*  u8;
        uint16_t* u16;
        uint32_t* u32;
        int8_t*   s8;
        int16_t*  s16;
        int32_t*  s32;
    };
} list_value_t;

/* 迭代器里的 .d / .cur 指向节点里存的值（node + offsetof(list_node_t, value)），
   不是节点本身 —— 这样 it_data(it) / it_sdata(it) 的老用法照旧可用。
   end() 与 rend() 同为哨兵 &head。 */
typedef struct list_iterator {
    union {
        list_data_t* d;
        char** sd;
        uint8_t* cur;
    };
} list_iterator_t;

typedef struct list_reverse_iterator {
    union {
        list_data_t* d;
        char** sd;
        uint8_t* cur;
    };
} list_reverse_iterator_t;
typedef list_reverse_iterator_t list_r_iterator_t;

#include <list/i_list.h>

typedef i_list_t list_t;

/* Method 1 */
static inline list_size_t        clist_size(const list_t* _this)                                          { return i_list_size(_this); }
static inline bool               __clist_empty(const list_t* _this)                                      { return __i_list_empty(_this); }
static inline list_count_t       clist_count(const list_t* _this, list_data_t data)                      { return i_list_count(_this, data); }
static inline list_iterator_t    clist_end(const list_t* _this)                                          { return i_list_end(_this); }
static inline list_iterator_t    clist_begin(const list_t* _this)                                        { return i_list_begin(_this); }
static inline list_iterator_t    clist_next(const list_t* _this, list_iterator_t iterator)               { return i_list_next(_this, iterator); }
static inline list_iterator_t    clist_prev(const list_t* _this, list_iterator_t iterator)               { return i_list_prev(_this, iterator); }
static inline list_r_iterator_t  clist_rend(const list_t* _this)                                         { return i_list_rend(_this); }
static inline list_r_iterator_t  clist_rbegin(const list_t* _this)                                       { return i_list_rbegin(_this); }
static inline list_r_iterator_t  clist_rnext(const list_t* _this, list_r_iterator_t r_iterator)          { return i_list_rnext(_this, r_iterator); }
static inline list_r_iterator_t  clist_rprev(const list_t* _this, list_r_iterator_t r_iterator)          { return i_list_rprev(_this, r_iterator); }
static inline list_iterator_t    __clist_it(const list_t* _this, list_size_t n)                          { return __i_list_it(_this, n); }
static inline list_iterator_t    clist_it(const list_t* _this, list_size_t n)                            { return i_list_it(_this, n); }
static inline list_value_t       __clist_at(const list_t* _this, list_size_t n)                          { return __i_list_at(_this, n); }
static inline list_value_t       clist_at(const list_t* _this, list_size_t n)                            { return i_list_at(_this, n); }
static inline list_value_t       clist_front(const list_t* _this)                                        { return i_list_front(_this); }
static inline list_value_t       clist_back(const list_t* _this)                                         { return i_list_back(_this); }
static inline list_data_t        clist_first(const list_t* _this, list_data_t default_data)               { return is_null(_this) ? default_data : __i_list_first(_this); }
static inline list_data_t        clist_last(const list_t* _this, list_data_t default_data)                { return is_null(_this) ? default_data : __i_list_last(_this); }
static inline list_iterator_t    clist_find(const list_t* _this, list_data_t data)                       { return i_list_find(_this, data); }
static inline list_iterator_t    clist_push_back(list_t* _this, list_data_t data)                        { return i_list_push_back(_this, data); }
static inline list_iterator_t    clist_push_front(list_t* _this, list_data_t data)                       { return i_list_push_front(_this, data); }
static inline list_iterator_t    clist_insert(list_t* _this, list_iterator_t iterator, list_data_t data) { return i_list_insert(_this, iterator, data); }
static inline list_iterator_t    clist_insert_n(list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data) { return i_list_insert_n(_this, iterator, n, data); }
static inline list_iterator_t    clist_erase(list_t* _this, list_iterator_t iterator)                    { return i_list_erase(_this, iterator); }
static inline list_iterator_t    clist_erase_range(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end) { return i_list_erase_range(_this, iterator_begin, iterator_end); }
static inline void               clist_pop_back(list_t* _this)                                           { i_list_pop_back(_this); }
static inline void               clist_pop_front(list_t* _this)                                          { i_list_pop_front(_this); }
static inline list_size_t        clist_remove(list_t* _this, list_data_t data)                           { return i_list_remove(_this, data); }
static inline list_size_t        clist_remove_if(list_t* _this, remove_if_condition cond)                { return i_list_remove_if(_this, cond); }
static inline list_size_t        clist_unique(list_t* _this)                                             { return i_list_unique(_this); }
static inline void               clist_reverse(list_t* _this)                                            { i_list_reverse(_this); }
static inline void               clist_sort(list_t* _this, list_cmp_t cmp)                               { i_list_sort(_this, cmp); }
static inline void               clist_merge(list_t* _this, list_t* other)                               { i_list_merge(_this, other); }
static inline void               clist_splice(list_t* _this, list_iterator_t iterator, list_t* other)    { i_list_splice(_this, iterator, other); }
static inline void               clist_splice_range(list_t* _this, list_iterator_t iterator, list_t* other, list_iterator_t iterator_begin, list_iterator_t iterator_end) { i_list_splice_range(_this, iterator, other, iterator_begin, iterator_end); }
static inline bool               clist_resize(list_t* _this, list_size_t n, list_data_t default_data)    { return i_list_resize(_this, n, default_data); }
static inline void               clist_swap(list_t* _this, list_t* other)                                { i_list_swap(_this, other); }
static inline list_size_t        clist_assign(list_t* _this, list_size_t n, list_data_t data)            { return i_list_assign(_this, n, data); }
static inline list_size_t        clist_clear(list_t* _this)                                              { return i_list_clear(_this); }

/* Method 2 */
typedef struct class_list {
    list_size_t (*size)(const list_t* _this);
    bool (*__empty)(const list_t* _this);
    list_count_t (*count)(const list_t* _this, list_data_t data);
    list_iterator_t (*end)(const list_t* _this);
    list_iterator_t (*begin)(const list_t* _this);
    list_iterator_t (*next)(const list_t* _this, list_iterator_t iterator);
    list_iterator_t (*prev)(const list_t* _this, list_iterator_t iterator);
    list_r_iterator_t (*rend)(const list_t* _this);
    list_r_iterator_t (*rbegin)(const list_t* _this);
    list_r_iterator_t (*rnext)(const list_t* _this, list_r_iterator_t r_iterator);
    list_r_iterator_t (*rprev)(const list_t* _this, list_r_iterator_t r_iterator);
    list_iterator_t (*__it)(const list_t* _this, list_size_t n);                                                   /* return iterator by n(index), but without checking parameters */
    list_iterator_t (*it)(const list_t* _this, list_size_t n);                                                     /* return iterator by n(index) */
    list_value_t (*__at)(const list_t* _this, list_size_t n);                                                      /* STL: ds[index], but without checking parameters */
    list_value_t (*at)(const list_t* _this, list_size_t n);                                                        /* STL: ds->at(index) */
    list_value_t (*front)(const list_t* _this);
    list_value_t (*back)(const list_t* _this);
    list_data_t (*first)(const list_t* _this, list_data_t default_data);
    list_data_t (*last)(const list_t* _this, list_data_t default_data);
    list_iterator_t (*find)(const list_t* _this, list_data_t data);
    list_iterator_t (*push_back)(list_t* _this, list_data_t data);
    list_iterator_t (*push_front)(list_t* _this, list_data_t data);
    list_iterator_t (*insert)(list_t* _this, list_iterator_t iterator, list_data_t data);                           /* insert data before `iterator` */
    list_iterator_t (*insert_n)(list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data);
    list_iterator_t (*erase)(list_t* _this, list_iterator_t iterator);
    list_iterator_t (*erase_range)(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end);   /* [iterator_begin, iterator_end) */
    void (*pop_back)(list_t* _this);
    void (*pop_front)(list_t* _this);
    list_size_t (*remove)(list_t* _this, list_data_t data);
    list_size_t (*remove_if)(list_t* _this, remove_if_condition cond);
    list_size_t (*unique)(list_t* _this);                                                                          /* STL: ds->unique(), drops consecutive duplicates */
    void (*reverse)(list_t* _this);                                                                                /* STL: ds->reverse() */
    void (*sort)(list_t* _this, list_cmp_t cmp);                                                                   /* STL: ds->sort(), 64-way merge sort. `cmp` decides the direction: NULL or (list_cmp_t)1 = plain numeric ascending, (list_cmp_t)2 = plain numeric descending, anything else is called as the comparator */
    void (*merge)(list_t* _this, list_t* other);                                                                   /* STL: ds->merge(other), both sides must be sorted */
    void (*splice)(list_t* _this, list_iterator_t iterator, list_t* other);                                        /* STL: ds->splice(pos, other) */
    void (*splice_range)(list_t* _this, list_iterator_t iterator, list_t* other,
                         list_iterator_t iterator_begin, list_iterator_t iterator_end);                            /* STL: ds->splice(pos, other, first, last) */
    bool (*resize)(list_t* _this, list_size_t n, list_data_t default_data);
    void (*swap)(list_t* _this, list_t* other);
    list_size_t (*assign)(list_t* _this, list_size_t n, list_data_t data);                                         /* STL: ds->assign(n, data) */
    list_size_t (*clear)(list_t* _this);
} class_list_t;

list_t* __list_new(const class_list_ops_t* ops, list_step_t step);
void    __list_delete(list_t** _this);
const class_list_t* class_list_ins(void);
#define g_class_list()                class_list_ins()
#define clist                         g_class_list()
#define LIST_NEW()                    __list_new(NULL, sizeof(list_data_t))
#define LIST_NEW_OPS(_ops)            __list_new((_ops), sizeof(list_data_t))
#define LIST_NEW_T(_type)             __list_new(NULL, sizeof(_type))
#define LIST_NEW_OPS_T(_ops, _type)   __list_new((_ops), sizeof(_type))
#define LIST_NEW_CHAR()               __list_new(g_class_list_ops_string(), sizeof(list_data_t))
#define LIST_NEW_SSO()                __list_new(g_class_list_ops_sso(), sizeof(ds_sso_t))
#define LIST_NEW_STRING()             LIST_NEW_SSO()
#define LIST_DELETE(_pptr)            do { __list_delete((_pptr)); } while(0)

/* clist_sort 的 cmp 参数用的两个魔术数字（内核 lib/sort.c 那套约定的简化版）：
   NULL / LIST_SORT_ASC 是同一件事 —— 对普通数字升序。要用别的次序，传自己的 list_cmp_t。 */
#define LIST_SORT_ASC                 ((list_cmp_t)(uintptr_t)1)
#define LIST_SORT_DESC                ((list_cmp_t)(uintptr_t)2)

#endif /* __J_LIST_H */
