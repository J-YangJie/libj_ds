/*
  List Interfaces
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

#ifndef __J_LIST_H
#define __J_LIST_H

#include <list/list_ops.h>
#include <iterator/iterator.h>

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

typedef struct list list_t;

/* Method 1 */
list_size_t        clist_size(const list_t* _this);
list_count_t       clist_count(const list_t* _this, list_data_t data);
list_iterator_t    clist_end(const list_t* _this);
list_iterator_t    clist_begin(const list_t* _this);
list_iterator_t    clist_next(const list_t* _this, const list_iterator_t iterator);
list_iterator_t    clist_prev(const list_t* _this, const list_iterator_t iterator);
list_r_iterator_t  clist_rend(const list_t* _this);
list_r_iterator_t  clist_rbegin(const list_t* _this);
list_r_iterator_t  clist_rnext(const list_t* _this, const list_r_iterator_t r_iterator);
list_r_iterator_t  clist_rprev(const list_t* _this, const list_r_iterator_t r_iterator);
list_data_t        clist_first(const list_t* _this, list_data_t default_data);
list_data_t        clist_last(const list_t* _this, list_data_t default_data);
list_iterator_t    clist_find(const list_t* _this, list_data_t data);
list_iterator_t    clist_push_back(list_t* _this, list_data_t data);
list_iterator_t    clist_push_front(list_t* _this, list_data_t data);
list_iterator_t    clist_insert(list_t* _this, list_iterator_t iterator, list_data_t data);
list_iterator_t    clist_erase(list_t* _this, list_iterator_t iterator);
list_iterator_t    clist_erase_range(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end);
void               clist_pop_back(list_t* _this);
void               clist_pop_front(list_t* _this);
list_size_t        clist_remove(list_t* _this, list_data_t data);
list_size_t        clist_remove_if(list_t* _this, remove_if_condition cond);
list_size_t        clist_clear(list_t* _this);

/* Method 2 */
typedef struct class_list {
    list_size_t (*size)(const list_t* _this);
    list_count_t (*count)(const list_t* _this, list_data_t data);
    list_iterator_t (*end)(const list_t* _this);
    list_iterator_t (*begin)(const list_t* _this);
    list_iterator_t (*next)(const list_t* _this, const list_iterator_t iterator);
    list_iterator_t (*prev)(const list_t* _this, const list_iterator_t iterator);
    list_r_iterator_t (*rend)(const list_t* _this);
    list_r_iterator_t (*rbegin)(const list_t* _this);
    list_r_iterator_t (*rnext)(const list_t* _this, const list_r_iterator_t r_iterator);
    list_r_iterator_t (*rprev)(const list_t* _this, const list_r_iterator_t r_iterator);
    list_data_t (*first)(const list_t* _this, list_data_t default_data);
    list_data_t (*last)(const list_t* _this, list_data_t default_data);
    list_iterator_t (*find)(const list_t* _this, list_data_t data);
    list_iterator_t (*push_back)(list_t* _this, list_data_t data);
    list_iterator_t (*push_front)(list_t* _this, list_data_t data);
    list_iterator_t (*insert)(list_t* _this, list_iterator_t iterator, list_data_t data);                         /* insert data before `iterator` */
    list_iterator_t (*erase)(list_t* _this, list_iterator_t iterator);
    list_iterator_t (*erase_range)(list_t* _this, list_iterator_t iterator_begin, list_iterator_t iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(list_t* _this);
    void (*pop_front)(list_t* _this);
    list_size_t (*remove)(list_t* _this, list_data_t data);
    list_size_t (*remove_if)(list_t* _this, remove_if_condition cond);
    list_size_t (*clear)(list_t* _this);
} class_list_t;

list_t* __list_new(const class_list_ops_t* ops);
void    __list_delete(list_t** _this);
const class_list_t* class_list_ins(void);
#define g_class_list()      class_list_ins()
#define clist               g_class_list()
#define LIST_NEW()          __list_new(NULL)
#define LIST_NEW_OPS(_ops)  __list_new((_ops))
#define LIST_NEW_STRING()   __list_new(g_class_list_ops_string())
#define LIST_DELETE(_pptr)  do { __list_delete((_pptr)); } while(0)

#endif /* __J_LIST_H */
