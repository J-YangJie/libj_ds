/*
  libj_ds permanent tests vector invariant fixtures
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

#ifndef __J_VECTOR_VALUE_H
#define __J_VECTOR_VALUE_H

#include <vector/vector.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  Returns NULL when the internal representation is consistent, otherwise a
  short description of the first violation. The check is read-only.
*/
const char* vector_check_invariants(const vector_t* self);

#ifdef __cplusplus
}
#endif

#endif /* __J_VECTOR_VALUE_H */
