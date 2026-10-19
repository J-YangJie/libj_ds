/*
  Data Structures Default Operations Of SSO String
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

#include <_types.h>
#include <string.h>

#define __DS_SSO_LOCAL_CAP 15
typedef struct ds_sso {
    char*  p;
    size_t len;
    union {
        char   buf[__DS_SSO_LOCAL_CAP + 1];
        size_t cap;
    };
} ds_sso_t;

static inline
bool ds_ops_valid_data_default_sso(ds_data_t data)
{
    const char* s = (char*)data;
    return s && 0 != s[0];
}

static inline
int __ds_ops_cmp_sso(const ds_sso_t* l, const ds_sso_t* r)
{
    size_t m = l->len < r->len ? l->len : r->len;
    int c = memcmp(l->p, r->p, m);
    if (0 != c) return c;
    return l->len < r->len ? -1 : l->len > r->len ? 1 : 0;
}

static inline
bool __ds_ops_eq_default_sso(ds_data_t left, ds_data_t right)
{
    return 0 == __ds_ops_cmp_sso((const ds_sso_t*)left, (const ds_sso_t*)right);
}

static inline
bool __ds_ops_lt_default_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_cmp_sso((const ds_sso_t*)left, (const ds_sso_t*)right) < 0;
}

static inline
bool __ds_ops_gt_default_sso(ds_data_t left, ds_data_t right)
{
    return __ds_ops_cmp_sso((const ds_sso_t*)left, (const ds_sso_t*)right) > 0;
}

bool ds_ops_copy_data_default_sso(ds_data_t in, ds_data_t* out);
void ds_ops_free_data_default_sso(ds_data_t* data);





static inline
bool __ds_ops_is_local_sso(const ds_sso_t* s)
{
    return s->p == s->buf;
}

static inline
void __ds_ops_move_sso(ds_sso_t* dst, const ds_sso_t* src)
{
    *dst = *src;
    if (__ds_ops_is_local_sso(src))
        dst->p = dst->buf;
}

static inline
bool __ds_ops_eq_sso(ds_data_t left, ds_data_t right)
{
    const char*     l = (const char*)left;
    const ds_sso_t* r = (const ds_sso_t*)right;
    size_t llen = strlen(l);
    return llen == r->len && 0 == memcmp(l, r->p, llen);
}

static inline
void __ds_ops_fix_sso(ds_data_t data)
{
    ds_sso_t* s = (ds_sso_t*)data;
    if (s->len <= __DS_SSO_LOCAL_CAP)
        s->p = s->buf;
}

#endif /* __J_DS_OPS_SSO_H */
