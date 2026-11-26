/*
  libj_ds permanent tests list descriptor
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

#ifndef __J_LIST_DESC_H
#define __J_LIST_DESC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../contracts/sequence_desc.h"
#include "../../contracts/sequence_str_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  List-specific operations used by the differential tests. Indexes follow
  the public iterator helpers; `index == size` denotes `end`.
*/
typedef struct test_list_ext_desc {
    void   (*sort)(void* self, bool ascending);
    bool   (*sorted)(const void* self, bool ascending);
    void   (*reverse)(void* self);
    size_t (*unique)(void* self);
    bool   (*replace)(void* self, size_t index, int32_t value);
    void   (*splice)(void* self, size_t index, void* source);
    void   (*splice_range)(void* self, size_t index, void* source,
                           size_t first, size_t last);
    void   (*merge)(void* self, void* source, bool ascending);
} test_list_ext_desc_t;

typedef struct test_list_str_ext_desc {
    void   (*sort)(void* self, bool ascending);
    bool   (*sorted)(const void* self, bool ascending);
    void   (*reverse)(void* self);
    size_t (*unique)(void* self);
    bool   (*replace)(void* self, size_t index, const char* value);
    void   (*splice)(void* self, size_t index, void* source);
    void   (*splice_range)(void* self, size_t index, void* source,
                           size_t first, size_t last);
    void   (*merge)(void* self, void* source, bool ascending);
} test_list_str_ext_desc_t;

const test_seq_desc_t*          list_desc_int32(void);
const test_seq_str_desc_t*      list_desc_string(void);
const test_list_ext_desc_t*     list_desc_ext_int32(void);
const test_list_str_ext_desc_t* list_desc_ext_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __J_LIST_DESC_H */
