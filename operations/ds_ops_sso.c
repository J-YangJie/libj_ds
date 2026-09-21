/*
  Default Implementations Of Data Structures Default Operations Of SSO String
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

#include <operations/ds_ops_sso.h>

#include <string.h>
#include <_memory.h>
#include <_compiler_inter.h>

JDSC_STATIC_ASSERT(offsetof(ds_sso_t, p) == 0, "p must stay at offset 0: the container reads the first 8 bytes of the slot as the user's char*");

JDSC_INLINE
bool ds_ops_valid_data_default_sso(ds_data_t data)
{
    const char* s = (char*)data;
    return !is_null(s) && 0 != s[0];
}

JDSC_INLINE
bool __ds_ops_eq_default_sso(ds_data_t left, ds_data_t right)
{
    const char*     l = (const char*)left;
    const ds_sso_t* r = (const ds_sso_t*)right;
    size_t llen = strlen(l);
    return llen == r->len && 0 == memcmp(l, r->p, llen);
}

JDSC_INLINE
bool ds_ops_copy_data_default_sso(ds_data_t in, ds_data_t* out)
{
    const char* i = (const char*)in;
    ds_sso_t*   o = (ds_sso_t*)out;
    size_t len = strlen(i);

    if (len <= DS_SSO_LOCAL_CAP) {
        memcpy(o->buf, i, len + 1);
        d->p = o->buf;
        d->len = len;
        return true;
    } else {
        char* s = (char*)p_malloc(len + 1);
        if (is_null(s))
            return false;
        memcpy(s, i, len + 1);
        o->p = s;
        o->len = len;
        o->cap = len + 1;
        return true;
    }
}

static JDSC_INLINE
bool __ds_ops_is_local_default_sso(const ds_sso_t* s)
{
    return s->p == s->buf;
}

JDSC_INLINE
void ds_ops_free_data_default_sso(ds_data_t* data)
{
    ds_sso_t* s = (ds_sso_t*)data;

    if (!__ds_ops_is_local_default_sso(s))
        p_free(s->p);
}
