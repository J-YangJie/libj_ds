/*
  Iterator macro
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

#ifndef __J_ITERATOR_H
#define __J_ITERATOR_H

#include <stdbool.h>
#include <stdint.h>

#define it_hash(it_obj)        ((it_obj).d->hash)
#define it_key(it_obj)         ((it_obj).d->key)
#define it_skey(it_obj)        ((it_obj).d->skey)
#define it_value(it_obj)       ((it_obj).d->value)
#define it_svalue(it_obj)      ((it_obj).d->svalue)
#define it_data(it_obj)        (*(it_obj).d)
#define it_sdata(it_obj)       (*(it_obj).sd)

#define it_hash_safe(it_obj)   ((it_obj).d                      ? it_hash(it_obj)   : 0)
#define it_key_safe(it_obj)    ((it_obj).d                      ? it_key(it_obj)    : INTPTR_MIN)
#define it_skey_safe(it_obj)   ((it_obj).d && it_skey(it_obj)   ? it_skey(it_obj)   : "null")
#define it_value_safe(it_obj)  ((it_obj).d                      ? it_value(it_obj)  : INTPTR_MIN)
#define it_svalue_safe(it_obj) ((it_obj).d && it_svalue(it_obj) ? it_svalue(it_obj) : "null")
#define it_data_safe(it_obj)   ((it_obj).d                      ? it_data(it_obj)   : INTPTR_MIN)
#define it_sdata_safe(it_obj)  ((it_obj).d && it_sdata(it_obj)  ? it_sdata(it_obj)  : "null")


static inline bool __it_eq(void* _d1, void* _d2)
{
    return _d1 == _d2;
}

static inline bool __it_ne(void* _d1, void* _d2)
{
    return _d1 != _d2;
}

#define it_eq(_it1, _it2)  (__it_eq((_it1).d, (_it2).d))
#define it_ne(_it1, _it2)  (__it_ne((_it1).d, (_it2).d))


#endif /* __J_ITERATOR_H */
