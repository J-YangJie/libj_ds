/*
  Demo Struct
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

#ifndef __J_DEMO_STRUCT_H
#define __J_DEMO_STRUCT_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <_types.h>
#include <_compiler.h>
#include <operations/ds_ops_string.h>
#include <sequence_container/sequence_container.h>

typedef struct demo_sub_struct {
    int a;
    double b;
    double c;
    double d;
} demo_sub_struct_t;

typedef struct demo_struct {
    int id;
    demo_sub_struct_t* sub_struct;
    ds_sso_t name1;
    char* name2;
    int* p;   /* Points to this struct's own member `c` */
    char a;
    short b;
    int c;
    long d;
} demo_struct_t;

HEADER_STATIC_INLINE
bool demo_struct_valid(ds_data_t data)
{
    const demo_struct_t* s = (const demo_struct_t*)data;
    return s && s->sub_struct && s->name1.p && s->name2 && s->p == &s->c;
}

HEADER_STATIC_INLINE
bool __demo_struct_gt(ds_data_t left, ds_data_t right)
{
    const demo_struct_t* l = (const demo_struct_t*)left;
    const demo_struct_t* r = (const demo_struct_t*)right;
    const int nl = l->id + *l->p + l->sub_struct->a;
    const int nr = r->id + *r->p + r->sub_struct->a;

    if (nl != nr)
        return nl > nr;
    return __ds_ops_gt_default_sso((ds_data_t)&l->name1, (ds_data_t)&r->name1);
}

HEADER_STATIC_INLINE
bool __demo_struct_eq(ds_data_t left, ds_data_t right)
{
    const demo_struct_t* l = (const demo_struct_t*)left;
    const demo_struct_t* r = (const demo_struct_t*)right;
    const int nl = l->id + *l->p + l->sub_struct->a;
    const int nr = r->id + *r->p + r->sub_struct->a;

    return nl == nr && __ds_ops_eq_default_sso((ds_data_t)&l->name1, (ds_data_t)&r->name1);
}

HEADER_STATIC_INLINE
bool demo_struct_node_copy(ds_data_t in, ds_data_t* out)
{
    const demo_struct_t* src = (const demo_struct_t*)in;
    demo_struct_t* dest = (demo_struct_t*)out;

    *dest = *src;

    __ds_ops_fix_sso((ds_data_t)&dest->name1); /* __ds_ops_move_sso(&dest->name1, &src->name1); */
    dest->p = &dest->c;
    return true;
}

HEADER_STATIC_INLINE
bool demo_struct_heap_shallow_copy(ds_data_t in, ds_data_t* out)
{
    const demo_struct_t* src = (const demo_struct_t*)in;
    demo_struct_t* dest = (demo_struct_t*)malloc(sizeof(demo_struct_t));

    if (!dest)
        return false;

    *dest = *src;

    __ds_ops_fix_sso((ds_data_t)&dest->name1); /* __ds_ops_move_sso(&dest->name1, &src->name1); */
    dest->p = &dest->c;
    *out = (ds_data_t)dest; /* The container slot stores only the heap address */
    return true;
}

HEADER_STATIC_INLINE
void demo_struct_node_free(ds_data_t* data)
{
    demo_struct_t* s = (demo_struct_t*)data;

    ds_ops_free_data_default_string((ds_data_t*)&s->name2);
    ds_ops_free_data_default_sso((ds_data_t*)&s->name1);
    free(s->sub_struct);
}

HEADER_STATIC_INLINE
void demo_struct_heap_free(ds_data_t* data)
{
    demo_struct_t** s = (demo_struct_t**)data;

    ds_ops_free_data_default_string((ds_data_t*)&(*s)->name2);
    ds_ops_free_data_default_sso((ds_data_t*)&(*s)->name1);
    free((*s)->sub_struct);
    free(*s);
    *s = NULL;
}

HEADER_STATIC_INLINE
void __demo_struct_node_move(ds_data_t dest, ds_data_t src)
{
    demo_struct_t* d = (demo_struct_t*)dest;
    demo_struct_t* s = (demo_struct_t*)src;

    *d = *s;
    __ds_ops_fix_sso((ds_data_t)&d->name1);
    d->p = &d->c;
}

HEADER_STATIC_INLINE
bool __demo_struct_constructor(demo_struct_t* s, int id, char* name1, char* name2)
{
    memset(s, 0, sizeof(*s));

    s->sub_struct = (demo_sub_struct_t*)malloc(sizeof(*s->sub_struct));
    if (!s->sub_struct)
        return false;

    s->sub_struct->a = id;

    s->id = id;
    if (!ds_ops_copy_data_default_sso((ds_data_t)name1, (ds_data_t*)&s->name1))
        goto free_sub_struct;

    if (!ds_ops_copy_data_default_string((ds_data_t)name2, (ds_data_t*)&s->name2))
        goto free_sso;

    s->p = &s->c;

    s->a = 'a';
    s->b = (short)id;
    s->c = id * 10;
    s->d = id * 100L;
    return true;

free_sso:
    ds_ops_free_data_default_sso((ds_data_t*)&s->name1);
free_sub_struct:
    free(s->sub_struct);
    return false;
}

HEADER_STATIC_INLINE
void demo_struct_destructor(demo_struct_t* s)
{
    ds_ops_free_data_default_string((ds_data_t*)&s->name2);
    ds_ops_free_data_default_sso((ds_data_t*)&s->name1);
    free(s->sub_struct);
    memset(s, 0, sizeof(*s));
}

#if JDSC_ALIGNMEM

typedef struct demo_align_struct {
    JDSC_ALIGN_TYPE_N(512) char a;
    short b;
    int c;
    long d;
} demo_align_struct_t;

HEADER_STATIC_INLINE
bool __demo_align_struct_gt(ds_data_t left, ds_data_t right)
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
bool __demo_align_struct_eq(ds_data_t left, ds_data_t right)
{
    const demo_align_struct_t* l = (const demo_align_struct_t*)left;
    const demo_align_struct_t* r = (const demo_align_struct_t*)right;

    return l->a == r->a && l->b == r->b && l->c == r->c && l->d == r->d;
}

HEADER_STATIC_INLINE
bool demo_align_struct_copy(ds_data_t in, ds_data_t* out)
{
    *(demo_align_struct_t*)out = *(const demo_align_struct_t*)in;
    return true;
}

HEADER_STATIC_INLINE
void demo_align_struct_free(ds_data_t* data)
{
    (void)data;
}

#endif /* JDSC_ALIGNMEM */

#endif /* __J_DEMO_STRUCT_H */
