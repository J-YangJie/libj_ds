/*
  libj_ds permanent tests associative allocation failures
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

#ifndef __J_ASSOC_ALLOC_H
#define __J_ASSOC_ALLOC_H

#include "assoc_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Register fail-at-N allocation cases for one associative descriptor. */
void test_assoc_alloc_register(const test_assoc_desc_t* desc);

#ifdef __cplusplus
}
#endif

#endif /* __J_ASSOC_ALLOC_H */
