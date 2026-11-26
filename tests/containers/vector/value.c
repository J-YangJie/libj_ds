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

#include <string.h>

#include "value.h"

const char* vector_check_invariants(const vector_t* self)
{
    const vector_step_t step = self != NULL ? self->step : 0;
    vector_size_t size;
    vector_size_t capacity;

    if (self == NULL)
        return "The vector pointer is null.";
    if (step == 0)
        return "`step` is zero.";
    if (self->align > step
        || (self->align != 0
            && (((self->align & (self->align - 1)) != 0)
                || (step % self->align) != 0)))
        return "`align` is not a power of two that divides `step`.";

    if (self->rend == NULL)
        return "The vector buffer is null.";
    if (self->begin != __i_vector_ptr_add(self->rend, 1, step))
        return "`begin` is not one element after the guard slot.";
    if (self->end < self->begin || self->end > self->end_of_storage)
        return "`end` is outside the buffer.";
    if (self->end_of_storage < self->end)
        return "`end_of_storage` precedes `end`.";

    if ((size_t)(self->end - self->begin) % step != 0)
        return "`end` is not aligned to `step`.";
    if ((size_t)(self->end_of_storage - self->begin) % step != 0)
        return "`end_of_storage` is not aligned to `step`.";

    size = __i_vector_size(self);
    capacity = __i_vector_capacity(self);
    if (size < 0 || capacity < 0)
        return "The computed size or capacity is negative.";
    if (capacity < size)
        return "`capacity` is smaller than `size`.";
    if (self->end_of_storage
        != __i_vector_ptr_add(self->begin, capacity, step))
        return "`end_of_storage` does not match `capacity`.";

#if JDSC_ALIGNMEM
    if (self->align != 0) {
        if ((uintptr_t)self->rend % (size_t)self->align != 0)
            return "The buffer does not satisfy `align`.";
    }
#endif /* JDSC_ALIGNMEM */

    if (self->sso) {
        vector_iterator_t it = cvector->begin(self);
        vector_iterator_t end = cvector->end(self);

        while (it.cur != end.cur) {
            const ds_sso_t* value = it.ssd;
            size_t len;

            if (value->p == NULL)
                return "An SSO element has a null string pointer.";
            len = strlen(value->p);
            if (len != value->len)
                return "An SSO element length does not match its string.";
            if ((len <= __DS_SSO_LOCAL_CAP) != (value->p == value->buf))
                return "An SSO element inline pointer is not canonical.";
            it = cvector->next(it);
        }
    }

    return NULL;
}
