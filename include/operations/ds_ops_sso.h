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

#define DS_SSO_LOCAL_CAP 15
typedef struct ds_sso {
    char*  p;
    size_t len;
    union {
        char   buf[DS_SSO_LOCAL_CAP + 1];
        size_t cap;
    };
} ds_sso_t;

bool ds_ops_valid_data_default_sso(ds_data_t data);              /* SSO type: judge the validity of the `data`(whether the pointer is null and whether the length is greater than 0),
                                                                              without limiting the length */
bool __ds_ops_eq_default_sso(ds_data_t left, ds_data_t right);   /* SSO type: return true if [ `left` == `right` ] */
bool ds_ops_copy_data_default_sso(ds_data_t in, ds_data_t* out); /* SSO type: deep copy `in` and use `out` to receive the copied memory */
void ds_ops_free_data_default_sso(ds_data_t* data);              /* SSO type: release the `data` and set `data` to `NULL` */


static inline void __ds_ops_fix_sso(ds_data_t data)
{
    ds_sso_t* s = (ds_sso_t*)data;
    if (s->len <= DS_SSO_LOCAL_CAP)
        s->p = s->buf;
}

static inline int ds_sso_cmp(const ds_sso_t* a, const ds_sso_t* b)
{
    size_t m = a->len < b->len ? a->len : b->len;
    int    r = memcmp(a->p, b->p, m);

    if (0 != r)
        return r;
    return a->len < b->len ? -1 : (a->len > b->len ? 1 : 0);
}

static inline void ds_sso_move(ds_sso_t* dst, const ds_sso_t* src)
{
    bool local = (src->p == src->buf);

    *dst = *src;
    if (local)
        dst->p = dst->buf;
}

#endif /* __J_DS_OPS_SSO_H */
