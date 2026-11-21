/*
  Sorting Algorithms Implementations
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

#ifndef __J_TEST_ADAPTER_QUEUE_H
#define __J_TEST_ADAPTER_QUEUE_H

#include "stress_adapter.h"

const class_stress_adapter_queue_interface_t* class_stress_adapter_queue_ins(void);
#define cs_adapter_queue class_stress_adapter_queue_ins()

#endif /* __J_TEST_ADAPTER_QUEUE_H */
