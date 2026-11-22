/*
  libj_ds permanent tests tracked value fixture
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

/*
  The tracked fixture is stored by value (node mode). Token and pointer
  conversions are required by the library's callback interface.
*/
#include <_types.h>
#include <sequence_container/sequence_container_ops.h>

#include <stdint.h>
#include <string.h>

#include "test.h"

#define TEST_TRACKED_MAGIC  0x6a6473747261636bULL

void* _p_malloc(size_t size);
void  _p_free(void** ptr);

static size_t tracked_live;
static size_t tracked_copies;
static size_t tracked_frees;
static size_t tracked_moves;
static int32_t tracked_next_id = 1;

static test_tracked_t* tracked_from(ds_data_t data)
{
    return (test_tracked_t*)(uintptr_t)data;
}

test_tracked_t test_tracked_make(int32_t payload)
{
    test_tracked_t value;
    int32_t* owned = _p_malloc(sizeof(*owned));

    memset(&value, 0, sizeof(value));
    if (owned == NULL)
        return value;

    *owned = payload;
    value.magic = TEST_TRACKED_MAGIC;
    value.payload = payload;
    value.id = tracked_next_id++;
    value.owned = owned;
    ++tracked_live;
    return value;
}

void test_tracked_dispose(test_tracked_t* value)
{
    if (value == NULL || value->magic != TEST_TRACKED_MAGIC)
        return;
    if (value->owned != NULL) {
        void* owned = value->owned;

        _p_free(&owned);
        value->owned = NULL;
        --tracked_live;
    }
    value->magic = 0;
}

const test_tracked_t* test_tracked_slot_at(const void* slot)
{
    return slot;
}

bool test_tracked_valid_at(const test_tracked_t* value)
{
    return value != NULL && value->magic == TEST_TRACKED_MAGIC
        && value->owned != NULL;
}

int32_t test_tracked_payload_at(const test_tracked_t* value)
{
    return value->payload;
}

size_t test_tracked_live(void)
{
    return tracked_live;
}

size_t test_tracked_copies(void)
{
    return tracked_copies;
}

size_t test_tracked_frees(void)
{
    return tracked_frees;
}

size_t test_tracked_moves(void)
{
    return tracked_moves;
}

static bool tracked_ops_valid(ds_data_t data)
{
    return data != 0 && tracked_from(data)->magic == TEST_TRACKED_MAGIC;
}

static bool tracked_ops_eq(ds_data_t left, ds_data_t right)
{
    const test_tracked_t* a = tracked_from(left);
    const test_tracked_t* b = tracked_from(right);

    if (!test_tracked_valid_at(a) || !test_tracked_valid_at(b))
        return false;
    return a->payload == b->payload;
}

static bool tracked_ops_copy(ds_data_t in, ds_data_t* out)
{
    const test_tracked_t* src = tracked_from(in);
    test_tracked_t* dst = (test_tracked_t*)out;
    int32_t* owned;

    if (!test_tracked_valid_at(src))
        return false;
    owned = _p_malloc(sizeof(*owned));
    if (owned == NULL)
        return false;

    *owned = src->payload;
    dst->magic = TEST_TRACKED_MAGIC;
    dst->payload = src->payload;
    dst->id = tracked_next_id++;
    dst->owned = owned;
    ++tracked_live;
    ++tracked_copies;
    return true;
}

static void tracked_ops_free(ds_data_t* data)
{
    test_tracked_t* value = (test_tracked_t*)data;

    if (value->magic != TEST_TRACKED_MAGIC || value->owned == NULL)
        return;
    {
        void* owned = value->owned;

        _p_free(&owned);
    }
    value->owned = NULL;
    value->magic = 0;
    --tracked_live;
    ++tracked_frees;
}

static void tracked_ops_move(ds_data_t dest, ds_data_t src)
{
    test_tracked_t* to = tracked_from(dest);
    test_tracked_t* from = tracked_from(src);

    if (to == from)
        return;
    memmove(to, from, sizeof(*to));
    from->magic = 0;
    from->owned = NULL;
    ++tracked_moves;
}

static const class_sequence_container_ops_t tracked_ops = {
    .valid_data = tracked_ops_valid,
    .__eq = tracked_ops_eq,
    .copy_data = tracked_ops_copy,
    .free_data = tracked_ops_free,
    .__move = tracked_ops_move,
};

const class_sequence_container_ops_t* test_ops_tracked(void)
{
    return &tracked_ops;
}
