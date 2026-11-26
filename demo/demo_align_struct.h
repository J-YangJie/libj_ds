/*
  Demo Align Struct
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

#ifndef __J_DEMO_ALIGN_STRUCT_H
#define __J_DEMO_ALIGN_STRUCT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <_types.h>
#include <_compiler.h>
#include <_memory.h>

#if JDSC_ALIGNMEM

typedef struct demo_align_struct {
    JDSC_ALIGN_TYPE_N(512) char a;
    short b;
    int c;
    long d;
} demo_align_struct_t;

/*
 *  Node mode
 *
 *  The element is stored directly inside the container node, and
 *  copy_data writes into the node pointed to by out.
 */
HEADER_STATIC_INLINE
bool __demo_align_struct_node_gt(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    if (l->a != r->a)
        return l->a > r->a;
    if (l->b != r->b)
        return l->b > r->b;
    if (l->c != r->c)
        return l->c > r->c;
    return l->d > r->d;
}

HEADER_STATIC_INLINE
bool __demo_align_struct_node_eq(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    return l->a == r->a && l->b == r->b && l->c == r->c && l->d == r->d;
}

HEADER_STATIC_INLINE
bool demo_align_struct_node_copy(ds_data_t in, ds_data_t* out)
{
    *(demo_align_struct_t*)out = *(const demo_align_struct_t*)in;
    return true;
}

HEADER_STATIC_INLINE
void demo_align_struct_node_free(ds_data_t* data)
{
    (void)data;
}

/*
 *  Heap mode
 *
 *  copy_data allocates an aligned element on the heap, and the slot
 *  stores the pointer.
 */
HEADER_STATIC_INLINE
bool __demo_align_struct_heap_lt(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    if (l->a != r->a)
        return l->a < r->a;
    if (l->b != r->b)
        return l->b < r->b;
    if (l->c != r->c)
        return l->c < r->c;
    return l->d < r->d;
}

HEADER_STATIC_INLINE
bool __demo_align_struct_heap_gt(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    if (l->a != r->a)
        return l->a > r->a;
    if (l->b != r->b)
        return l->b > r->b;
    if (l->c != r->c)
        return l->c > r->c;
    return l->d > r->d;
}

HEADER_STATIC_INLINE
bool __demo_align_struct_heap_eq(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    return l->a == r->a && l->b == r->b && l->c == r->c && l->d == r->d;
}

HEADER_STATIC_INLINE
bool demo_align_struct_heap_copy(ds_data_t in, ds_data_t* out)
{
    const demo_align_struct_t* src = (const demo_align_struct_t*)in;
    demo_align_struct_t* dest = (demo_align_struct_t*)p_aligned_malloc((size_t)JDSC_ALIGNOF(demo_align_struct_t), sizeof(demo_align_struct_t));

    if (!dest)
        return false;

    *dest = *src;
    *out = (ds_data_t)dest;
    return true;
}

HEADER_STATIC_INLINE
void demo_align_struct_heap_free(ds_data_t* data)
{
    demo_align_struct_t** s = (demo_align_struct_t**)data;

    p_aligned_free(*s, (size_t)JDSC_ALIGNOF(demo_align_struct_t));
}

#endif /* JDSC_ALIGNMEM */

#endif /* __J_DEMO_ALIGN_STRUCT_H */
