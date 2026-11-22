/*
  libj_ds permanent tests list invariant fixtures
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

#include <stddef.h>
#include <string.h>

#include "value.h"

const char* list_check_invariants(const list_t* self)
{
    const struct list_head* head;
    const struct list_head* pos;
    list_size_t count = 0;

    if (self == NULL)
        return "The list pointer is null.";
    if (self->step == 0)
        return "`step` is zero.";
    if (self->swap == NULL)
        return "The swap buffer is null.";
    if (!__sequence_container_dtype_valid(self->dtype))
        return "The data type is invalid.";
    if (self->sso
        && (self->dtype != SEQ_CNTR_DTYPE_SSO
            || self->step != sizeof(ds_sso_t)
            || self->ops != g_class_list_ops_sso()))
        return "The SSO flag does not match the data layout.";
    if (self->size < 0 || self->size > __i_dlist_max_size(self))
        return "`size` is out of range.";

    head = &self->head;
    if (head->next == NULL || head->prev == NULL)
        return "A sentinel link is null.";
    if (head->next->prev != head || head->prev->next != head)
        return "The sentinel links are not reciprocal.";
    if (self->size == 0 && head->next != head)
        return "An empty list has a non-sentinel head link.";

    for (pos = head->next; pos != head; pos = pos->next) {
        const list_node_t* node =
            (const list_node_t*)((const uint8_t*)pos
                                 - offsetof(list_node_t, node));

        if (pos->next == NULL || pos->prev == NULL)
            return "A node link is null.";
        if (pos->next->prev != pos || pos->prev->next != pos)
            return "The forward and backward links disagree.";
        if (++count > self->size)
            return "The chain is cyclic or longer than `size`.";

#if JDSC_ALIGNMEM
        if (self->align != 0
            && (uintptr_t)node->data % (uintptr_t)self->align != 0)
            return "An element address does not satisfy `align`.";
#endif /* JDSC_ALIGNMEM */

        if (self->sso) {
            const ds_sso_t* value = (const ds_sso_t*)node->data;
            size_t len;

            if (value->p == NULL)
                return "An SSO element has a null string pointer.";
            len = strlen(value->p);
            if (len != value->len)
                return "An SSO element length does not match its string.";
            if ((len <= __DS_SSO_LOCAL_CAP) != (value->p == value->buf))
                return "An SSO element inline pointer is not canonical.";
        }
    }

    if (count != self->size)
        return "`size` does not match the node count.";
    return NULL;
}
