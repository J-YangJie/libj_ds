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

#include <iterator/iterator.h>
#include <sequence_container/sequence_container.h>

typedef sequence_container_value_t     list_value_t;
typedef class_sequence_container_ops_t class_list_ops_t;

typedef struct list_iterator {
    union {
        list_data_t* d;
        char** sd;
    };
} list_iterator_t;

typedef struct list_reverse_iterator {
    union {
        list_data_t* d;
        char** sd;
    };
} list_reverse_iterator_t;
typedef list_reverse_iterator_t list_r_iterator_t;

#include <list/i_list.h>

typedef i_list_t list_t;

/* Method 1 */
static inline list_size_t        clist_size(const list_t* _this)                                                                { return i_dlist_size(_this); }
static inline bool               __clist_empty(const list_t* _this)                                                             { return __i_dlist_empty(_this); }
static inline list_count_t       clist_count(const list_t* _this, list_data_t data)                                             { return i_dlist_count(_this, data); }
static inline list_iterator_t    clist_end(const list_t* _this)                                                                 { return i_clist_end(_this); }
static inline list_iterator_t    clist_begin(const list_t* _this)                                                               { return i_clist_begin(_this); }
static inline list_iterator_t    clist_next(const list_t* _this, list_iterator_t iterator)                                      { return i_clist_next(_this, iterator); }
static inline list_iterator_t    clist_prev(const list_t* _this, list_iterator_t iterator)                                      { return i_clist_prev(_this, iterator); }
static inline list_r_iterator_t  clist_rend(const list_t* _this)                                                                { return i_clist_rend(_this); }
static inline list_r_iterator_t  clist_rbegin(const list_t* _this)                                                              { return i_clist_rbegin(_this); }
static inline list_r_iterator_t  clist_rnext(const list_t* _this, list_r_iterator_t r_iterator)                                 { return i_clist_rnext(_this, r_iterator); }
static inline list_r_iterator_t  clist_rprev(const list_t* _this, list_r_iterator_t r_iterator)                                 { return i_clist_rprev(_this, r_iterator); }
static inline list_iterator_t    __clist_it(const list_t* _this, list_index_t index)                                            { return __i_dlist_iterator(_this, index); }
static inline list_iterator_t    clist_it(const list_t* _this, list_index_t index)                                              { return i_dlist_iterator(_this, index); }
static inline list_value_t       __clist_at(const list_t* _this, list_index_t index)                                            { return __i_dlist_at(_this, index); }
static inline list_value_t       clist_at(const list_t* _this, list_index_t index)                                              { return i_dlist_at(_this, index); }
static inline list_value_t       clist_back(const list_t* _this)                                                                { return i_dlist_back(_this); }
static inline list_value_t       clist_front(const list_t* _this)                                                               { return i_dlist_front(_this); }
static inline list_iterator_t    clist_find(const list_t* _this, list_data_t data)                                              { return i_clist_find(_this, data); }
static inline bool               clist_push_back(list_t* _this, list_data_t data)                                               { return i_dlist_push_back(_this, data); }
static inline bool               clist_push_front(list_t* _this, list_data_t data)                                              { return i_dlist_push_front(_this, data); }
static inline list_iterator_t    clist_insert(list_t* _this, list_iterator_t iterator, list_data_t data)                        { return i_clist_insert(_this, iterator, data); }
static inline list_iterator_t    clist_insert_n(list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data)       { return i_clist_insert_n(_this, iterator, n, data); }
static inline list_iterator_t    clist_erase(list_t* _this, list_iterator_t iterator)                                           { return i_clist_erase(_this, iterator); }
static inline list_iterator_t    clist_erase_range(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end) { return i_clist_erase_range(_this, iterator_begin, iterator_end); }
static inline void               clist_pop_back(list_t* _this)                                                                  { i_dlist_pop_back(_this); }
static inline void               clist_pop_front(list_t* _this)                                                                 { i_dlist_pop_front(_this); }
static inline list_size_t        clist_remove(list_t* _this, list_data_t data)                                                  { return i_dlist_remove(_this, data); }
static inline list_size_t        clist_remove_if(list_t* _this, remove_if_condition cond)                                       { return i_dlist_remove_if(_this, cond); }
static inline list_size_t        clist_unique(list_t* _this)                                                                    { return i_dlist_unique(_this); }
static inline void               clist_reverse(list_t* _this)                                                                   { i_dlist_reverse(_this); }
static inline void               clist_merge(list_t* _this, list_t* other, list_cmp_t cmp)                                      { i_dlist_merge(_this, other, cmp); }
static inline void               clist_splice(list_t* _this, list_iterator_t iterator, list_t* other)                           { i_dlist_splice(_this, iterator, other); }
static inline void               clist_splice_range(list_t* _this, list_iterator_t iterator, list_t* other, list_iterator_t iterator_begin, list_iterator_t iterator_end) { i_dlist_splice_range(_this, iterator, other, iterator_begin, iterator_end); }
static inline bool               clist_resize(list_t* _this, list_size_t n, list_data_t default_data)                           { return i_dlist_resize(_this, n, default_data); }
static inline void               clist_swap(list_t* _this, list_t* other)                                                       { i_dlist_swap(_this, other); }
static inline list_size_t        clist_assign(list_t* _this, list_size_t n, list_data_t data)                                   { return i_dlist_assign(_this, n, data); }
static inline list_size_t        clist_clear(list_t* _this)                                                                     { return i_dlist_clear(_this); }

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
    list_iterator_t (*__it)(const list_t* _this, list_index_t index);                                                  /* return iterator by n(index), but without checking parameters */
    list_iterator_t (*it)(const list_t* _this, list_index_t index);                                                    /* return iterator by n(index) */
    list_value_t (*__at)(const list_t* _this, list_index_t index);                                                     /* STL: ds[index], but without checking parameters */
    list_value_t (*at)(const list_t* _this, list_index_t index);                                                       /* STL: ds->at(index) */
    list_value_t (*back)(const list_t* _this);
    list_value_t (*front)(const list_t* _this);
    list_iterator_t (*find)(const list_t* _this, list_data_t data);
    bool (*push_back)(list_t* _this, list_data_t data);
    bool (*push_front)(list_t* _this, list_data_t data);
    list_iterator_t (*insert)(list_t* _this, list_iterator_t iterator, list_data_t data);                           /* insert data before `iterator` */
    list_iterator_t (*insert_n)(list_t* _this, list_iterator_t iterator, list_size_t n, list_data_t data);
    list_iterator_t (*erase)(list_t* _this, list_iterator_t iterator);
    list_iterator_t (*erase_range)(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end);   /* [iterator_begin, iterator_end) */
    void (*pop_back)(list_t* _this);
    void (*pop_front)(list_t* _this);
    list_size_t (*remove)(list_t* _this, list_data_t data);
    list_size_t (*remove_if)(list_t* _this, remove_if_condition cond);
    list_size_t (*unique)(list_t* _this);                                                                          /* STL: ds->unique()，删除连续重复元素，返回删除个数 */
    void (*reverse)(list_t* _this);                                                                                /* STL: ds->reverse()，原地翻转节点链接 */
    void (*splice)(list_t* _this, list_iterator_t iterator, list_t* other);                                        /* STL: ds->splice(pos, other)，整条链搬到 pos 之前 */
    void (*splice_range)(list_t* _this, list_iterator_t iterator, list_t* other,
                         list_iterator_t iterator_begin, list_iterator_t iterator_end);                            /* STL: ds->splice(pos, other, first, last)，[first, last) 搬到 pos 之前 */
    void (*merge)(list_t* _this, list_t* other, list_cmp_t cmp);                                                   /* STL: ds->merge(other, cmp)，两条有序链归并；cmp 必须非空 */
    void (*swap)(list_t* _this, list_t* other);                                                                    /* STL: ds->swap(other)，交换两条链的内容（要求 step/ops 一致） */
    bool (*resize)(list_t* _this, list_size_t n, list_data_t default_data);
    list_size_t (*assign)(list_t* _this, list_size_t n, list_data_t data);                                         /* STL: ds->assign(n, data) */
    list_size_t (*clear)(list_t* _this);
} class_list_t;

list_t* __list_new(const class_list_ops_t* ops, list_dtype_t data_type, list_step_t step, list_align_t align);
void    __list_delete(list_t** _this);
const class_list_t* class_list_ins(void);
#define g_class_list()               class_list_ins()
#define clist                        g_class_list()
#define g_class_list_ops_string()    g_class_sequence_container_ops_string()
#define g_class_list_ops_sso()       g_class_sequence_container_ops_sso()
#define LIST_NEW_COMPLATE(_type_id, _type)            __list_new(NULL,   (_type_id), sizeof(_type), __alignof__(_type))
#define LIST_NEW_OPS_COMPLATE(_ops, _type_id, _type)  __list_new((_ops), (_type_id), sizeof(_type), __alignof__(_type))
#define LIST_NEW_T(_type)            LIST_NEW_OPS_COMPLATE((NULL), (__SEQUENCE_CONTAINER_DTYPE_##_type), _type)
#define LIST_NEW_OPS_T(_ops, _type)  LIST_NEW_OPS_COMPLATE((_ops), (__SEQUENCE_CONTAINER_DTYPE_##_type), _type)
#define LIST_NEW()                   LIST_NEW_OPS_T(NULL,   list_data_t)
#define LIST_NEW_OPS(_ops)           LIST_NEW_OPS_T((_ops), list_data_t)
#define LIST_NEW_CHAR()              LIST_NEW_OPS_T(g_class_list_ops_string(), list_data_t)
#define LIST_NEW_SSO()               LIST_NEW_OPS_T(g_class_list_ops_sso(),    ds_sso_t)
#define LIST_NEW_STRING()            LIST_NEW_SSO()
#define LIST_DELETE(_pptr)           do { __list_delete((_pptr)); } while(0)

#endif /* __J_LIST_H */
