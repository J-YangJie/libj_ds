/*
  Default Implementations Of Data Structures Default Operations
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

#include <operations/ds_ops_string.h>

#include <string.h>
#include <_log.h>
#include <_memory.h>
#include <_compiler.h>

#define TAG "[ds_ops_string]"

/* BKDR Hash */
static JDSC_INLINE
ds_hash_t _hash_bkdr(const char* str, int len)
{
    ds_hash_t seed = 131;
    ds_hash_t ret = 0;

    for (int i = 0; i < len; ++i)
        ret = ret * seed + (*str++);

    return (ret & 0x7FFFFFFF);
}

/* AP Hash */
static JDSC_INLINE
ds_hash_t _hash_ap(const char* str, int len)
{
    ds_hash_t ret = 0;

    for (int i = 0; i < len; ++i) {
        if (0 == (i & 1))
            ret ^= ((ret << 7) ^ (*str++) ^ (ret >> 3));
        else
            ret ^= (~((ret << 11) ^ (*str++) ^ (ret >> 5)));
    }

    return (ret & 0x7FFFFFFF);
}

JDSC_INLINE
ds_hash_t __ds_ops_hash_default_string(ds_key_t key)
{
    char* k = (char*)key;
    size_t len = strlen(k);
    return _hash_bkdr(k, len);
}

static JDSC_INLINE
bool ds_ops_valid_key_default_string_max_n(ds_key_t key, size_t max) /* the return value type of strlen is size_t */
{
    char* k = (char*)key;
    size_t tlen;
    if (!is_null(k) && (tlen = strlen(k)) > 0 && tlen <= max)
        return true;
    pr_notice("Invalid key [ %s ], max [ %zu ]!", is_null(k) ? "null" : k, max);
    return false;
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_2(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 2);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_4(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 4);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_8(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 8);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_16(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 16);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_32(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 32);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_64(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 64);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_128(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 128);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_256(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 256);
}

JDSC_INLINE
bool ds_ops_valid_key_default_string_max_512(ds_key_t key)
{
    return ds_ops_valid_key_default_string_max_n(key, 512);
}

JDSC_INLINE
bool ds_ops_valid_data_default_string(ds_data_t data)
{
    char* s = (char*)data;
    if (!is_null(s) && 0 != s[0])
        return true;
    // pr_notice("Invalid data [ %s ]!", is_null(s) ? "null" : s);
    return false;
}

static JDSC_INLINE
int smart_strcmp(const char *s1, size_t len1, const char *s2, size_t len2) {
    size_t mlen = len1 < len2 ? len1 : len2;
    int ret = memcmp(s1, s2, mlen);

    if (0 != ret) {
        return ret;
    } else {
        if (len1 == len2)
            return 0;
        return len1 < len2 ? -1 : 1;
    }
}

JDSC_INLINE
bool __ds_ops_lt_default_string(ds_data_t left, ds_data_t right)
{
    char* l = (char*)left;
    char* r = (char*)right;
    return strcmp(l, r) < 0; /* TODO: security */
    // return smart_strcmp(l, strlen(l), r, strlen(r)) < 0;
}

JDSC_INLINE
bool __ds_ops_gt_default_string(ds_data_t left, ds_data_t right)
{
    char* l = (char*)left;
    char* r = (char*)right;
    return strcmp(l, r) > 0; /* TODO: security */
    // return smart_strcmp(l, strlen(l), r, strlen(r)) > 0;
}

JDSC_INLINE
bool __ds_ops_eq_default_string(ds_data_t left, ds_data_t right)
{
    char* l = (char*)left;
    char* r = (char*)right;
    // size_t llen = strlen(l); (void)llen;
    // size_t rlen = strlen(r); (void)rlen;
    // return llen != rlen ? false : 0 == memcmp(l, r, llen);
    return 0 == strcmp(l, r); /* TODO: security */
}

JDSC_INLINE
bool ds_ops_copy_data_default_string(ds_data_t in, ds_data_t* out)
{
    char* i = (char*)in;
    char** o = (char**)out;
    size_t len = 0;

    // if (is_null(i) || is_null(o))
    //     return false;

    len = strlen(i);
    *o = (char*)p_malloc(len + 1);
    if (is_null(*o))
        return false;

    memcpy(*o, i, len + 1);
    return true;
}

JDSC_INLINE
void ds_ops_free_data_default_string(ds_data_t* data)
{
    char** s = (char**)data;
    p_free(*s);
}

JDSC_INLINE
bool __ds_ops_gt_default(ds_data_t left, ds_data_t right)
{
    return left > right;
}
