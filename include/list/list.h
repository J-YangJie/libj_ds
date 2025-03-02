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

#include <linux/_types.h>
#include <list/list_ops.h>

typedef struct list_node {
    list_data_t data;
    struct list_head node;
} list_node_t;

typedef struct list_iterator {
    list_data_t data;
} list_iterator_t;

typedef struct list_reverse_iterator {
    list_data_t data;
} list_reverse_iterator_t;
typedef list_reverse_iterator_t list_r_iterator_t;

typedef struct list {
    const class_list_ops_t* ops;
    struct list_head head;
    list_size_t size;
} list_t;

typedef struct class_list {
    list_size_t (*size)(const list_t* _this);
    list_count_t (*count)(const list_t* _this, list_data_t data);
    list_iterator_t* (*end)(const list_t* _this);
    list_iterator_t* (*begin)(const list_t* _this);
    list_iterator_t* (*next)(const list_t* _this, const list_iterator_t* iterator);
    list_iterator_t* (*prev)(const list_t* _this, const list_iterator_t* iterator);
    list_r_iterator_t* (*rend)(const list_t* _this);
    list_r_iterator_t* (*rbegin)(const list_t* _this);
    list_r_iterator_t* (*rnext)(const list_t* _this, const list_r_iterator_t* r_iterator);
    list_r_iterator_t* (*rprev)(const list_t* _this, const list_r_iterator_t* r_iterator);
    list_data_t (*first)(const list_t* _this, list_data_t default_data);
    list_data_t (*last)(const list_t* _this, list_data_t default_data);
    list_iterator_t* (*find)(const list_t* _this, list_data_t data);
    list_iterator_t* (*push_back)(list_t* _this, list_data_t data);
    list_iterator_t* (*push_front)(list_t* _this, list_data_t data);
    list_iterator_t* (*insert)(list_t* _this, list_iterator_t* iterator, list_data_t data);                         /* insert data before `iterator` */
    list_iterator_t* (*erase)(list_t* _this, list_iterator_t* iterator);
    list_iterator_t* (*erase_range)(list_t* _this, list_iterator_t* iterator_begin, list_iterator_t* iterator_end); /* [iterator_begin, iterator_end) */
    void (*pop_back)(list_t* _this);
    void (*pop_front)(list_t* _this);
    list_size_t (*remove)(list_t* _this, list_data_t data);
    list_size_t (*remove_if)(list_t* _this, remove_if_condition cond);
    list_size_t (*clear)(list_t* _this);
} class_list_t;

void __list_init(list_t* list);
void __list_deinit(list_t* list);
const class_list_t* class_list_ins(void);
#define g_class_list()            class_list_ins()
#define clist                     g_class_list()
#define LIST_INIT(_ptr)           (list_t) { .ops = NULL, .size = 0, }; __list_init((_ptr))
#define LIST_INIT_OPS(_ptr, _ops) (list_t) { .ops = _ops, .size = 0, }; __list_init((_ptr))
#define LIST_INIT_STRING(_ptr)    LIST_INIT_OPS((_ptr), g_class_list_ops_string())
#define LIST_DEINIT(_ptr)         do { __list_deinit((_ptr)); } while(0)

#endif /* __J_LIST_H */
