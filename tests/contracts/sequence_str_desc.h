/*
  libj_ds permanent tests owned-string descriptor
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

#ifndef __J_SEQUENCE_STR_DESC_H
#define __J_SEQUENCE_STR_DESC_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*test_seq_str_visit_fn)(void* ctx, const char* value);

/*
  The descriptor owns string storage through the container. Callers pass
  NUL-terminated strings; empty strings are rejected by the library and are
  not used. `get` copies into the caller buffer and fails if it is too small.
*/
typedef struct test_seq_str_desc {
    const char* name;

    void*  (*create)(void);
    void   (*destroy)(void* self);
    size_t (*size)(const void* self);
    bool   (*get)(const void* self, size_t index, char* buf, size_t cap);
    bool   (*foreach)(const void* self, bool reverse,
                      test_seq_str_visit_fn fn, void* ctx);

    bool   (*push_back)(void* self, const char* value);
    bool   (*push_front)(void* self, const char* value);
    bool   (*insert)(void* self, size_t index, const char* value,
                     size_t* out_index);
    bool   (*erase)(void* self, size_t index, size_t* out_next);

    size_t (*clear)(void* self);
    void   (*swap)(void* self, void* other);
} test_seq_str_desc_t;

#ifdef __cplusplus
}
#endif

#endif /* __J_SEQUENCE_STR_DESC_H */
