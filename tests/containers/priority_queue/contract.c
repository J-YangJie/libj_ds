/*
  libj_ds permanent tests priority queue contracts
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

#include "../../core/test.h"
#include "desc.h"

static void priority_queue_contract_register(void)
{
    test_adaptor_contract_register(priority_queue_desc_vector_max());
    test_adaptor_contract_register(priority_queue_desc_vector_min());
    test_adaptor_contract_register(priority_queue_desc_deque_max());
    test_adaptor_contract_register(priority_queue_desc_deque_min());
}

TEST_CTOR static void priority_queue_register_contracts(void)
{
    priority_queue_contract_register();
}
