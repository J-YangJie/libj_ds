/*
  Associative Containers Operations Defines
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

#ifndef __J_ASSOCIATIVE_CONTAINER_OPS_H
#define __J_ASSOCIATIVE_CONTAINER_OPS_H

#include <_types.h>
#include <_token.h>

typedef struct class_associative_container_key_ops {
    bool (*valid_key)(ds_key_t key);                     /* Return true if `key` is valid */
    bool (*__lt)(ds_key_t left, ds_key_t right);         /* Return true if [ `left` < `right` ] */
    bool (*copy_key)(ds_key_t in, ds_key_t* out);        /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_key` must also be implemented */
    void (*free_key)(ds_key_t* key);                     /* The function pointer can be null and manages memory on its own */
} class_associative_container_key_ops_t;

typedef struct class_associative_container_value_ops {
    bool (*valid_value)(ds_value_t value);               /* Return true if `value` is valid */
    bool (*copy_value)(ds_value_t in, ds_value_t* out);  /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_value` must also be implemented */
    void (*free_value)(ds_value_t* value);               /* The function pointer can be null and manages memory on its own */
} class_associative_container_value_ops_t;

typedef struct class_associative_container_ops {
    bool (*valid_key)(ds_key_t key);                     /* Return true if `key` is valid */
    bool (*__lt)(ds_key_t left, ds_key_t right);         /* Return true if [ `left` < `right` ] */
    bool (*copy_key)(ds_key_t in, ds_key_t* out);        /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_key` must also be implemented */
    void (*free_key)(ds_key_t* key);                     /* The function pointer can be null and manages memory on its own */
    bool (*valid_value)(ds_value_t value);               /* Return true if `value` is valid */
    bool (*copy_value)(ds_value_t in, ds_value_t* out);  /* The function pointer can be null and manages memory on its own. However, if this function is implemented, `free_value` must also be implemented */
    void (*free_value)(ds_value_t* value);               /* The function pointer can be null and manages memory on its own */
} class_associative_container_ops_t;

#define __CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(_t) \
    const class_associative_container_key_ops_t*    class_associative_container_key_ops_##_t##_ins(void);   \
    const class_associative_container_value_ops_t*  class_associative_container_value_ops_##_t##_ins(void);

__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(uint8_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(uint16_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(uint32_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(uint64_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(int8_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(int16_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(int32_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(int64_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(uintptr_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(intptr_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(size_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(ssize_t)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(float)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(double)
__CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL(chars)

#undef __CLASS_ASSOCIATIVE_CONTAINER_KV_OPS_DECL

#define __CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(_t) \
    const class_associative_container_key_ops_t* class_associative_container_key_ops_##_t##_greater_ins(void);

__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(uint8_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(uint16_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(uint32_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(uint64_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(int8_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(int16_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(int32_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(int64_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(uintptr_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(intptr_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(size_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(ssize_t)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(float)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(double)
__CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL(chars)

#undef __CLASS_ASSOCIATIVE_CONTAINER_KEY_OPS_GREATER_DECL

#define g_class_associative_container_key_ops_uint8_t()            class_associative_container_key_ops_uint8_t_ins()
#define g_class_associative_container_key_ops_uint16_t()           class_associative_container_key_ops_uint16_t_ins()
#define g_class_associative_container_key_ops_uint32_t()           class_associative_container_key_ops_uint32_t_ins()
#define g_class_associative_container_key_ops_uint64_t()           class_associative_container_key_ops_uint64_t_ins()
#define g_class_associative_container_key_ops_int8_t()             class_associative_container_key_ops_int8_t_ins()
#define g_class_associative_container_key_ops_int16_t()            class_associative_container_key_ops_int16_t_ins()
#define g_class_associative_container_key_ops_int32_t()            class_associative_container_key_ops_int32_t_ins()
#define g_class_associative_container_key_ops_int64_t()            class_associative_container_key_ops_int64_t_ins()
#define g_class_associative_container_key_ops_uintptr_t()          class_associative_container_key_ops_uintptr_t_ins()
#define g_class_associative_container_key_ops_intptr_t()           class_associative_container_key_ops_intptr_t_ins()
#define g_class_associative_container_key_ops_size_t()             class_associative_container_key_ops_size_t_ins()
#define g_class_associative_container_key_ops_ssize_t()            class_associative_container_key_ops_ssize_t_ins()
#define g_class_associative_container_key_ops_float()              class_associative_container_key_ops_float_ins()
#define g_class_associative_container_key_ops_double()             class_associative_container_key_ops_double_ins()
#define g_class_associative_container_key_ops_chars()              class_associative_container_key_ops_chars_ins()
#define g_class_associative_container_key_ops_string()             g_class_associative_container_key_ops_chars()

/* Descending key order. */
#define g_class_associative_container_key_ops_uint8_t_greater()    class_associative_container_key_ops_uint8_t_greater_ins()
#define g_class_associative_container_key_ops_uint16_t_greater()   class_associative_container_key_ops_uint16_t_greater_ins()
#define g_class_associative_container_key_ops_uint32_t_greater()   class_associative_container_key_ops_uint32_t_greater_ins()
#define g_class_associative_container_key_ops_uint64_t_greater()   class_associative_container_key_ops_uint64_t_greater_ins()
#define g_class_associative_container_key_ops_int8_t_greater()     class_associative_container_key_ops_int8_t_greater_ins()
#define g_class_associative_container_key_ops_int16_t_greater()    class_associative_container_key_ops_int16_t_greater_ins()
#define g_class_associative_container_key_ops_int32_t_greater()    class_associative_container_key_ops_int32_t_greater_ins()
#define g_class_associative_container_key_ops_int64_t_greater()    class_associative_container_key_ops_int64_t_greater_ins()
#define g_class_associative_container_key_ops_uintptr_t_greater()  class_associative_container_key_ops_uintptr_t_greater_ins()
#define g_class_associative_container_key_ops_intptr_t_greater()   class_associative_container_key_ops_intptr_t_greater_ins()
#define g_class_associative_container_key_ops_size_t_greater()     class_associative_container_key_ops_size_t_greater_ins()
#define g_class_associative_container_key_ops_ssize_t_greater()    class_associative_container_key_ops_ssize_t_greater_ins()
#define g_class_associative_container_key_ops_float_greater()      class_associative_container_key_ops_float_greater_ins()
#define g_class_associative_container_key_ops_double_greater()     class_associative_container_key_ops_double_greater_ins()
#define g_class_associative_container_key_ops_chars_greater()      class_associative_container_key_ops_chars_greater_ins()
#define g_class_associative_container_key_ops_string_greater()     g_class_associative_container_key_ops_chars_greater()

#define g_class_associative_container_value_ops_uint8_t()          class_associative_container_value_ops_uint8_t_ins()
#define g_class_associative_container_value_ops_uint16_t()         class_associative_container_value_ops_uint16_t_ins()
#define g_class_associative_container_value_ops_uint32_t()         class_associative_container_value_ops_uint32_t_ins()
#define g_class_associative_container_value_ops_uint64_t()         class_associative_container_value_ops_uint64_t_ins()
#define g_class_associative_container_value_ops_int8_t()           class_associative_container_value_ops_int8_t_ins()
#define g_class_associative_container_value_ops_int16_t()          class_associative_container_value_ops_int16_t_ins()
#define g_class_associative_container_value_ops_int32_t()          class_associative_container_value_ops_int32_t_ins()
#define g_class_associative_container_value_ops_int64_t()          class_associative_container_value_ops_int64_t_ins()
#define g_class_associative_container_value_ops_uintptr_t()        class_associative_container_value_ops_uintptr_t_ins()
#define g_class_associative_container_value_ops_intptr_t()         class_associative_container_value_ops_intptr_t_ins()
#define g_class_associative_container_value_ops_size_t()           class_associative_container_value_ops_size_t_ins()
#define g_class_associative_container_value_ops_ssize_t()          class_associative_container_value_ops_ssize_t_ins()
#define g_class_associative_container_value_ops_float()            class_associative_container_value_ops_float_ins()
#define g_class_associative_container_value_ops_double()           class_associative_container_value_ops_double_ins()
#define g_class_associative_container_value_ops_chars()            class_associative_container_value_ops_chars_ins()
#define g_class_associative_container_value_ops_string()           g_class_associative_container_value_ops_chars()

/* Associative key constructors. */
#ifndef __DS_KEY_CTORS_DEFINED
#define __DS_KEY_CTORS_DEFINED
#define ds_key(v)         __ds_key(v)
#define ds_skey(v)        __ds_skey(v)

#define ds_u8key(v)       __ds_u8key(v)
#define ds_u16key(v)      __ds_u16key(v)
#define ds_u32key(v)      __ds_u32key(v)
#define ds_u64key(v)      __ds_u64key(v)
#define ds_s8key(v)       __ds_s8key(v)
#define ds_s16key(v)      __ds_s16key(v)
#define ds_s32key(v)      __ds_s32key(v)
#define ds_s64key(v)      __ds_s64key(v)
#define ds_uptrkey(v)     __ds_uptrkey(v)
#define ds_iptrkey(v)     __ds_iptrkey(v)
#define ds_sizekey(v)     __ds_sizekey(v)
#define ds_ssizekey(v)    __ds_ssizekey(v)
#define ds_fkey(v)        __ds_fkey(v)
#define ds_lfkey(v)       __ds_lfkey(v)
#endif /* __DS_KEY_CTORS_DEFINED */

/* Associative value constructors. */
#ifndef __DS_VALUE_CTORS_DEFINED
#define __DS_VALUE_CTORS_DEFINED
#define ds_value(v)       __ds_value(v)
#define ds_svalue(v)      __ds_svalue(v)

#define ds_u8value(v)     __ds_u8value(v)
#define ds_u16value(v)    __ds_u16value(v)
#define ds_u32value(v)    __ds_u32value(v)
#define ds_u64value(v)    __ds_u64value(v)
#define ds_s8value(v)     __ds_s8value(v)
#define ds_s16value(v)    __ds_s16value(v)
#define ds_s32value(v)    __ds_s32value(v)
#define ds_s64value(v)    __ds_s64value(v)
#define ds_uptrvalue(v)   __ds_uptrvalue(v)
#define ds_iptrvalue(v)   __ds_iptrvalue(v)
#define ds_sizevalue(v)   __ds_sizevalue(v)
#define ds_ssizevalue(v)  __ds_ssizevalue(v)
#define ds_fvalue(v)      __ds_fvalue(v)
#define ds_lfvalue(v)     __ds_lfvalue(v)
#endif /* __DS_VALUE_CTORS_DEFINED */

HEADER_STATIC_INLINE
class_associative_container_ops_t class_associative_container_ops_compose(const class_associative_container_key_ops_t* ops_key,
                                                                          const class_associative_container_value_ops_t* ops_value)
{
    class_associative_container_ops_t ops;

    ops.valid_key   = ops_key   ? ops_key->valid_key     : NULL;
    ops.__lt        = ops_key   ? ops_key->__lt          : NULL;
    ops.copy_key    = ops_key   ? ops_key->copy_key      : NULL;
    ops.free_key    = ops_key   ? ops_key->free_key      : NULL;
    ops.valid_value = ops_value ? ops_value->valid_value : NULL;
    ops.copy_value  = ops_value ? ops_value->copy_value  : NULL;
    ops.free_value  = ops_value ? ops_value->free_value  : NULL;

    return ops;
}

#endif /* __J_ASSOCIATIVE_CONTAINER_OPS_H */
