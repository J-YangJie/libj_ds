/*
  libj_ds permanent tests deque descriptor
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

#ifndef __J_DEQUE_DESC_H
#define __J_DEQUE_DESC_H

#include "../../contracts/sequence_desc.h"
#include "../../contracts/sequence_str_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

const test_seq_desc_t*     deque_desc_int32(void);
const test_seq_str_desc_t* deque_desc_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __J_DEQUE_DESC_H */
