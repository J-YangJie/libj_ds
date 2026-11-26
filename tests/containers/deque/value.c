/*
  libj_ds permanent tests deque invariant fixtures
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

const char* deque_check_invariants(const deque_t* self)
{
    deque_bcount_t index;
    deque_size_t size;

    if (self == NULL)
        return "The deque pointer is null.";
    if (self->step == 0)
        return "`step` is zero.";
    if (self->bkt_size < 8
        || (self->bkt_size & (self->bkt_size - 1)) != 0)
        return "`bkt_size` is not a power of two of at least 8.";
    if (self->bkt == NULL)
        return "The bucket map is null.";
    if (self->bkt_count < 3)
        return "The bucket map is too small.";
    if (self->begin.bkt < self->bkt
        || self->end.bkt >= self->bkt + self->bkt_count)
        return "An iterator bucket is outside the bucket map.";
    if (self->begin.bkt > self->end.bkt)
        return "The begin bucket follows the end bucket.";

    for (index = (deque_bcount_t)(self->begin.bkt - self->bkt);
         index <= (deque_bcount_t)(self->end.bkt - self->bkt); ++index) {
        if (self->bkt[index] == NULL)
            return "An active bucket is null.";
    }

    if (self->begin.cur < self->begin.begin
        || self->begin.cur > __i_deque_ptr_sub(self->begin.end, 1, self->step))
        return "`begin.cur` is outside its bucket.";
    if (self->end.cur < self->end.begin
        || self->end.cur > __i_deque_ptr_sub(self->end.end, 1, self->step))
        return "`end.cur` is outside its bucket.";
    if (self->begin.bkt == self->end.bkt && self->begin.cur > self->end.cur)
        return "The same-bucket iterators are reversed.";

    size = __i_deque_size(self);
    if (size < 0)
        return "The computed size is negative.";
    if (size == 0
        && (self->begin.bkt != self->end.bkt
            || self->begin.cur != self->end.cur))
        return "An empty deque has distinct begin and end.";

#if JDSC_ALIGNMEM
    if (self->align != 0) {
        deque_iterator_t it = cdeque->begin(self);
        deque_iterator_t end = cdeque->end(self);

        while (it.cur != end.cur) {
            if ((uintptr_t)it.cur % (size_t)self->align != 0)
                return "An element address does not satisfy `align`.";
            it = cdeque->next(it);
        }
    }
#endif /* JDSC_ALIGNMEM */

    if (self->sso) {
        deque_iterator_t it = cdeque->begin(self);
        deque_iterator_t end = cdeque->end(self);

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
            it = cdeque->next(it);
        }
    }

    return NULL;
}
