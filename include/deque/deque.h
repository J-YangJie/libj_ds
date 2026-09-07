/*
  Deque Interfaces
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

#ifndef __J_DEQUE_H
#define __J_DEQUE_H

#include <_types.h>

typedef struct deque_iterator {
    union {
        deque_data_t* d;
        char** sd;
        deque_data_t* cur;
    };
    deque_data_t*  begin;
    deque_data_t*  end;
    deque_data_t** bkt;
} deque_iterator_t;

typedef struct deque_reverse_iterator {
    union {
        deque_data_t* d;
        char** sd;
        deque_data_t* cur;
    };
    deque_data_t*  begin;
    deque_data_t*  end;
    deque_data_t** bkt;
    deque_data_t*  begin_cur;
} deque_reverse_iterator_t;
typedef deque_reverse_iterator_t deque_r_iterator_t;

#include <deque/i_deque.h>

typedef i_deque_t deque_t;

static inline deque_size_t       cdeque_size(const deque_t* _this)                     { return i_deque_size(_this); }
static inline deque_count_t      cdeque_count(const deque_t* _this, deque_data_t data) { return i_deque_count(_this, data); }
static inline deque_iterator_t   cdeque_end(const deque_t* _this)                      { return i_deque_end(_this); }
static inline deque_iterator_t   cdeque_begin(const deque_t* _this)                    { return i_deque_begin(_this); }
static inline deque_iterator_t   cdeque_next(deque_iterator_t iterator)                { return i_deque_next(iterator); }
static inline deque_iterator_t   cdeque_prev(deque_iterator_t iterator)                { return i_deque_prev(iterator); }
static inline deque_r_iterator_t cdeque_rend(const deque_t* _this)                     { return i_deque_rend(_this); }
static inline deque_r_iterator_t cdeque_rbegin(const deque_t* _this)                   { return i_deque_rbegin(_this); }
static inline deque_r_iterator_t cdeque_rnext(deque_r_iterator_t r_iterator)           { return i_deque_rnext(r_iterator); }
static inline deque_r_iterator_t cdeque_rprev(deque_r_iterator_t r_iterator)           { return i_deque_rprev(r_iterator); }
//static inline deque_iterator_t   cdeque_first(const deque_t* _this)                    { return i_deque_first(_this); }
//static inline deque_iterator_t   cdeque_last(const deque_t* _this)                     { return i_deque_last(_this); }
static inline deque_iterator_t   cdeque_find(const deque_t* _this, deque_data_t data)  { return i_deque_find(_this, data); }
static inline bool               cdeque_push_back(deque_t* _this, deque_data_t data)   { return i_deque_push_back(_this, data); }
static inline bool               cdeque_push_front(deque_t* _this, deque_data_t data)  { return i_deque_push_front(_this, data); }
static inline deque_iterator_t   cdeque_insert(deque_t* _this, deque_iterator_t pos, deque_data_t data) { return i_deque_insert(_this, pos, data); }
static inline deque_iterator_t   cdeque_erase(deque_t* _this, deque_iterator_t pos)    { return i_deque_erase(_this, pos); }
static inline deque_iterator_t   cdeque_erase_range(deque_t* _this, deque_iterator_t iterator_begin, deque_iterator_t iterator_end) { return i_deque_erase_range(_this, iterator_begin, iterator_end); }
static inline void               cdeque_pop_back(deque_t* _this)                       { i_deque_pop_back(_this); }
static inline void               cdeque_pop_front(deque_t* _this)                      { i_deque_pop_front(_this); }
static inline deque_size_t       cdeque_remove(deque_t* _this, deque_data_t data)      { return i_deque_remove(_this, data); }
static inline deque_size_t       cdeque_remove_if(deque_t* _this, remove_if_condition cond) { return i_deque_remove_if(_this, cond); }
static inline deque_size_t       cdeque_clear(deque_t* _this)                          { return i_deque_clear(_this); }


deque_t* __deque_new(const class_deque_ops_t* ops);
void     __deque_delete(deque_t** _this);
// const class_deque_t* class_deque_ins(void);
// #define g_class_deque()      class_deque_ins()
// #define cdeque               g_class_deque()
#define DEQUE_NEW()          __deque_new(NULL)
#define DEQUE_NEW_OPS(_ops)  __deque_new((_ops))
#define DEQUE_NEW_STRING()   __deque_new(g_class_deque_ops_string())
#define DEQUE_DELETE(_pptr)  do { __deque_delete((_pptr)); } while(0)

#endif /* __J_DEQUE_H */
