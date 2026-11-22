/*
  libj_ds permanent tests stack models
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

static void stack_model_register(void)
{
    test_adaptor_model_register(stack_desc_vector());
    test_adaptor_model_register(stack_desc_deque());
    test_adaptor_model_register(stack_desc_list());
}

TEST_CTOR static void stack_register_models(void)
{
    stack_model_register();
}
