/*
  Default Operation Interface Of The Data Structures For SSO
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

#ifndef __J_DS_OPS_SSO_H
#define __J_DS_OPS_SSO_H

#include <string.h>
#include <_types.h>
#include <_compiler.h>

#define __DS_SSO_LOCAL_CAP 15

typedef struct ds_sso {
    char* p;
    size_t len;
    union {
        char buf[__DS_SSO_LOCAL_CAP + 1];
        size_t cap;
    };
} ds_sso_t;


/*
 *  Build a read-only SSO view over a C string.
 *
 *  Only `p` and `len` are meaningful; `buf`/`cap` are intentionally left untouched.
 *  The view must not be stored, moved or freed, and `s` must outlive it.
 *  Validating the input is the caller's job (usually via `valid_data`).
 */
HEADER_STATIC_INLINE
bool __ds_ops_sso_constructor(ds_sso_t* _this, const char* s)
{
    _this->p   = (char*)s;
    _this->len = strlen(s);
    return true; /* Never return false */
}

HEADER_STATIC_INLINE
void __ds_ops_sso_destructor(ds_sso_t* _this)
{
    /* Nothing */
}





/*
 * Same
 *
 * `l/left` and `r/right` have the same meaning
 */
HEADER_STATIC_INLINE
int __ds_ops_cmp_same_sso(const ds_sso_t* l, const ds_sso_t* r)
{
    size_t m = l->len < r->len ? l->len : r->len;
    int c = memcmp(l->p, r->p, m);
    if (0 != c) return c;
    return l->len < r->len ? -1 : l->len > r->len ? 1 : 0;
}

HEADER_STATIC_INLINE
bool __ds_ops_eq_same_sso(ds_data_t left, ds_data_t right)
{
    const ds_sso_t* l = (const ds_sso_t*)left;
    const ds_sso_t* r = (const ds_sso_t*)right;

    /* Length first: only equal lengths need the memcmp. */
    return l->len == r->len && 0 == memcmp(l->p, r->p, l->len);
}

HEADER_STATIC_INLINE
bool __ds_ops_lt_same_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_cmp_same_sso((const ds_sso_t*)left, (const ds_sso_t*)right) < 0;
}

HEADER_STATIC_INLINE
bool __ds_ops_gt_same_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_cmp_same_sso((const ds_sso_t*)left, (const ds_sso_t*)right) > 0;
}

bool ds_ops_copy_data_same_sso(ds_data_t in, ds_data_t* out);

HEADER_STATIC_INLINE
bool __ds_ops_is_local_sso(const ds_sso_t* s)
{
    return s->p == s->buf;
}

HEADER_STATIC_INLINE
void __ds_ops_move_same_sso(ds_sso_t* dest, const ds_sso_t* src)
{
    *dest = *src;
    if (__ds_ops_is_local_sso(src))
        dest->p = dest->buf;
}





/*
 * Default interfaces
 *
 * Exposed as the public SSO callbacks: each one forwards to its same-side
 * implementation, so callers never deal with `char*` vs. `ds_sso_t`.
 */
HEADER_STATIC_INLINE
bool ds_ops_valid_data_default_sso(ds_data_t data)
{
    const char* s = (char*)data;
    return s && 0 != s[0];
}

HEADER_STATIC_INLINE
bool __ds_ops_eq_default_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_eq_same_sso(left, right);
}

HEADER_STATIC_INLINE
bool __ds_ops_lt_default_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_lt_same_sso(left, right);
}

HEADER_STATIC_INLINE
bool __ds_ops_gt_default_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_gt_same_sso(left, right);
}

HEADER_STATIC_INLINE
bool ds_ops_copy_data_default_sso(ds_data_t in, ds_data_t* out)
{
    return ds_ops_copy_data_same_sso(in, out);
}

void ds_ops_free_data_default_sso(ds_data_t* data);

HEADER_STATIC_INLINE
void __ds_ops_move_default_sso(ds_sso_t* dest, const ds_sso_t* src)
{
    return __ds_ops_move_same_sso(dest, src);
}





/*
 * Internal helpers
 *
 * Used by the containers themselves; not part of the public callback set.
 */
HEADER_STATIC_INLINE
void __ds_ops_fix_sso(ds_data_t data)
{
    ds_sso_t* s = (ds_sso_t*)data;
    if (s->len <= __DS_SSO_LOCAL_CAP)
        s->p = s->buf;
}

HEADER_STATIC_INLINE
ds_data_t __ds_ops_char_addr_sso(ds_data_t data)
{
    ds_sso_t* s = (ds_sso_t*)data;
    return (ds_data_t)s->p;
}


#endif /* __J_DS_OPS_SSO_H */
