/*
  Vector Demos
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>
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

#include <vector/vector.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include "demo_struct.h"

#define cds cvector
#define TAG "[demo_vector]"

#define foreach(_v)          { for (vector_iterator_t it = cds->begin(_v);    it_ne(cds->end(_v), it);  it = cds->next(it))  pr_test("%d", it_s32data(it)); }
#define foreach_string(_v)   { for (vector_iterator_t it = cds->begin(_v);    it_ne(cds->end(_v), it);  it = cds->next(it))  pr_test("%s", it_sdata(it)); }
#define foreach_r_string(_v) { for (vector_r_iterator_t it = cds->rbegin(_v); it_ne(cds->rend(_v), it); it = cds->rnext(it)) pr_test("%s", it_sdata(it)); }

/*
 *  Base and iterator api demo
 */
static void demo_base_and_iterator(void)
{
    vector_t* demo = VECTOR_NEW_T(int32_t);

    pr_test("Container empty [ %d ], size [ %zd ]",
            cds->__empty(demo), cds->size(demo));

    for (int32_t i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->push_back(demo, 8);
    // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ], size = 9

    pr_test("After insert, size [ %zd ], capacity [ %zd ], max_size [ %zd ]",
            cds->size(demo), cds->capacity(demo), cds->max_size(demo));
    pr_test("Count(8) [ %zd ], empty [ %d ]",
            cds->count(demo, 8), cds->__empty(demo));
    pr_test("front [ %d ], back [ %d ], at(2) [ %d ]",
            *cds->front(demo).s32, *cds->back(demo).s32, *cds->at(demo, 2).s32);

    foreach(demo); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
    pr_test("");

    {
        for (vector_iterator_t   it = cds->end(demo);    it_ne(cds->begin(demo), it); )  { it = cds->prev(it);  pr_test("%d", it_s32data(it)); }  // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (vector_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(it))       pr_test("%d", it_s32data(it));    // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (vector_r_iterator_t it = cds->rend(demo);   it_ne(cds->rbegin(demo), it); ) { it = cds->rprev(it); pr_test("%d", it_s32data(it)); }  // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
        pr_test("");
    }

    VECTOR_DELETE(&demo);
}

/*
 *  Low-level interfaces
 *
 *  `__`-prefixed methods skip parameter checks. Use them only when the
 *  index or iterator is already known to be valid.
 */
static void demo_low_level_api(void)
{
    vector_t* demo = VECTOR_NEW_T(int32_t);
    vector_iterator_t it;

    for (int32_t i = 1; i <= 5; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5 ]

    // __front -> 1, __back -> 5, __at(2) -> 3, __it(3) -> 4
    (void)cds->__front(demo);
    (void)cds->__back(demo);
    (void)cds->__at(demo, 2);
    it = cds->__it(demo, 3);
    pr_test("__Front|__back|__at(2)|__it(3) = [ %d | %d | %d | %d ]",
            *cds->__front(demo).s32, *cds->__back(demo).s32,
            *cds->__at(demo, 2).s32, it_s32data(cds->__it(demo, 3)));

    cds->__replace_index(demo, 1, 20); // [ 1, 20, 3, 4, 5 ]
    cds->__replace_it(demo, it, 40);   // [ 1, 20, 3, 40, 5 ]
    pr_test("After replace, at(1) and at(3) = [ %d | %d ]",
            *cds->__at(demo, 1).s32, *cds->__at(demo, 3).s32);

    foreach(demo); // [ 1, 20, 3, 40, 5 ]
    pr_test("");

    VECTOR_DELETE(&demo);
}

/*
 *  About insert interfaces
 */
static void demo_about_insert(void)
{
    vector_t* demo  = VECTOR_NEW_T(int32_t);
    vector_t* other = VECTOR_NEW_T(int32_t);

    cds->push_back(demo, 5);
    cds->push_back(demo, 6);
    cds->push_back(demo, 7);
    // [ 5, 6, 7 ]

    cds->insert(demo, cds->begin(demo), 1); // [ 1, 5, 6, 7 ]
    cds->insert(demo, cds->end(demo), 8);   // [ 1, 5, 6, 7, 8 ]

    cds->insert_n(demo, cds->it(demo, 1), 2, 2); // [ 1, 2, 2, 5, 6, 7, 8 ]

    cds->push_back(other, 10);
    cds->push_back(other, 20); // now other [ 10, 20 ]
    cds->insert_range(demo, cds->end(demo),
                      cds->begin(other), cds->end(other)); // [ 1, 2, 2, 5, 6, 7, 8, 10, 20 ]

    foreach(demo); // [ 1, 2, 2, 5, 6, 7, 8, 10, 20 ], now other [ 10, 20 ]
    pr_test("");

    VECTOR_DELETE(&demo);
    VECTOR_DELETE(&other);
}

/*
 *  About remove interfaces
 *
 *  The expected container state after each call, and the return values
 *  of remove/remove_if/clear, are written in the comments below; this
 *  demo does not print them.
 */
static bool demo_remove_if_condition(ds_data_t data)
{
    return data >= 7;
}

static void demo_about_erase(void)
{
    vector_t* demo = VECTOR_NEW_T(int32_t);

    for (int32_t i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->erase_range(demo, cds->it(demo, 1), cds->it(demo, 4)); // [ 1, 5, 6, 7, 8 ]
    cds->erase(demo, cds->begin(demo));                         // [ 5, 6, 7, 8 ]
    cds->pop_back(demo);                                        // [ 5, 6, 7 ]

    cds->remove(demo, 66);                          // [ 5, 6, 7 ], return 0
    cds->remove_if(demo, demo_remove_if_condition); // [ 5, 6 ], return 1
    cds->clear(demo);                               // [ ], return 2

    VECTOR_DELETE(&demo);
}

/*
 *  About find and replace interfaces
 */
static void demo_about_find_and_replace(void)
{
    vector_t* demo = VECTOR_NEW_T(int32_t);
    vector_iterator_t it;

    for (int32_t i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->find(demo, 3);  // found, it -> 3
    pr_test("find(3) [ %d ]", it_s32data(it));

    it = cds->find(demo, 66); // no found, it -> end()
    pr_test("find(66) is end [ %d ]", it_eq(cds->end(demo), it));

    cds->replace_index(demo, 2, 30); // [ 1, 2, 30, 4, 5, 6, 7, 8 ]
    it = cds->it(demo, 3);
    cds->replace_it(demo, it, 40);   // [ 1, 2, 30, 40, 5, 6, 7, 8 ]

    foreach(demo); // [ 1, 2, 30, 40, 5, 6, 7, 8 ]
    pr_test("");

    VECTOR_DELETE(&demo);
}

/*
 *  Sort functions generated by sort_macro
 *
 *  The macros below define `sort_s32_asc(first, last)` and
 *  `sort_s32_desc(first, last)`, plus their `_sorted` checks.
 */
vector_sort_define_less(sort_s32_asc, int32_t)
vector_sort_define_greater(sort_s32_desc, int32_t)

/*
 *  About sort interfaces
 *
 *  `vector_sort_define` generates a sort function for one element
 *  type. The function sorts [first, last), like std::sort.
 */
static void demo_about_sort(void)
{
    vector_t* demo = VECTOR_NEW_T(int32_t);
    int32_t values[] = { 5, 1, 4, 2, 3 };

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
        cds->push_back(demo, values[i]);

    pr_test("Before sort:");
    foreach(demo);
    pr_test("");

    sort_s32_asc(cds->begin(demo), cds->end(demo));
    pr_test("After ascending sort, sorted [ %d ]",
            sort_s32_asc_sorted(cds->begin(demo), cds->end(demo)));
    foreach(demo);
    pr_test("");

    sort_s32_desc(cds->begin(demo), cds->end(demo));
    pr_test("After descending sort, sorted [ %d ]",
            sort_s32_desc_sorted(cds->begin(demo), cds->end(demo)));
    foreach(demo);
    pr_test("");

    VECTOR_DELETE(&demo);
}

/*
 *  About other interfaces
 *
 *  The expected states of assign/assign_range/swap are written in the
 *  comments below; after the swap only `other` is printed.
 */
static void demo_reserve_and_resize_and_assign(void)
{
    vector_t* demo  = VECTOR_NEW_T(int32_t);
    vector_t* other = VECTOR_NEW_T(int32_t);

    cds->push_back(demo, 65);
    cds->push_back(demo, 67);
    cds->push_back(demo, 69);
    // [ 65, 67, 69 ], now size = 3, capacity = 4

    pr_test("Before reserve(5), size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));
    cds->reserve(demo, 5);
    // [ 65, 67, 69 ], now size = 3, capacity = 5
    pr_test("After  reserve(5), size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));

    cds->push_back(demo, 1);
    cds->push_back(demo, 3);
    cds->push_back(demo, 5);
    // [ 65, 67, 69, 1, 3, 5 ], now size = 6, capacity = 10

    pr_test("Before resize(8), size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));
    cds->resize(demo, 8, 66);
    // [ 65, 67, 69, 1, 3, 5, 66, 66 ], now size = 8, capacity = 10
    pr_test("After  resize(8), size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));

    foreach(demo); // [ 65, 67, 69, 1, 3, 5, 66, 66 ]
    pr_test("");

    pr_test("Before shrink_to_fit, size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));
    cds->shrink_to_fit(demo);
    // [ 65, 67, 69, 1, 3, 5, 66, 66 ], now size = 8, capacity = 8
    pr_test("After  shrink_to_fit, size [ %zd ], capacity [ %zd ]",
            cds->size(demo), cds->capacity(demo));

    cds->assign(demo, 3, 42);
    // [ 42, 42, 42 ]

    cds->push_back(other, 100);
    cds->push_back(other, 200); // other [ 100, 200 ]
    cds->assign_range(demo, cds->begin(other), cds->end(other));
    // [ 100, 200 ]

    cds->clear(other);
    cds->push_back(other, 7);
    cds->push_back(other, 8); // other [ 7, 8 ]
    cds->swap(demo, other);
    // demo [ 7, 8 ], other [ 100, 200 ]

    foreach(other); // [ 100, 200 ]
    pr_test("");

    cds->clear(demo);
    cds->clear(other);

    VECTOR_DELETE(&demo);
    VECTOR_DELETE(&other);
}

/*
 *  About preset type
 *
 *  1. VECTOR_NEW_T(_type) selects the element width and the matching preset
 *     ops. Read values with the matching `it_*data` accessor.
 *  2. Use `ds_u64data`/`ds_s64data` for `uint64_t`/`int64_t`,
 *     `ds_fdata`/`ds_lfdata` for floating-point values, and `ds_sdata`
 *     for strings.
 *  3. Other integer types may be passed directly; their `ds_*data`
 *     constructors are optional.
 */
static void demo_preset_type(void)
{
    vector_t* demo_u8     = VECTOR_NEW_T(uint8_t);
    vector_t* demo_u16    = VECTOR_NEW_T(uint16_t);
    vector_t* demo_u32    = VECTOR_NEW_T(uint32_t);
    vector_t* demo_u64    = VECTOR_NEW_T(uint64_t);
    vector_t* demo_s8     = VECTOR_NEW_T(int8_t);
    vector_t* demo_s16    = VECTOR_NEW_T(int16_t);
    vector_t* demo_s32    = VECTOR_NEW_T(int32_t);
    vector_t* demo_s64    = VECTOR_NEW_T(int64_t);
    vector_t* demo_uptr   = VECTOR_NEW_T(uintptr_t);
    vector_t* demo_iptr   = VECTOR_NEW_T(intptr_t);
    vector_t* demo_size   = VECTOR_NEW_T(size_t);
    vector_t* demo_ssize  = VECTOR_NEW_T(ssize_t);
    vector_t* demo_f      = VECTOR_NEW_T(float);
    vector_t* demo_lf     = VECTOR_NEW_T(double);
    vector_t* demo_sso    = VECTOR_NEW_T(ds_sso_t);

    {
        for (uint8_t i = 0; i < 5; ++i)
            cds->push_back(demo_u8, i); /* cds->push_back(demo_u8, ds_u8data(i)); */
        pr_test("uint8_t: ");
        for (vector_iterator_t it = cds->begin(demo_u8); it_ne(cds->end(demo_u8), it); it = cds->next(it))
            pr_test("%u ", it_u8data(it));
        pr_test("");
    }

    {
        for (uint16_t i = 0; i < 5; ++i)
            cds->push_back(demo_u16, i); /* cds->push_back(demo_u16, ds_u16data(i)); */
        pr_test("uint16_t: ");
        for (vector_iterator_t it = cds->begin(demo_u16); it_ne(cds->end(demo_u16), it); it = cds->next(it))
            pr_test("%u ", it_u16data(it));
        pr_test("");
    }

    {
        for (uint32_t i = 0; i < 5; ++i)
            cds->push_back(demo_u32, i); /* cds->push_back(demo_u32, ds_u32data(i)); */
        pr_test("uint32_t: ");
        for (vector_iterator_t it = cds->begin(demo_u32); it_ne(cds->end(demo_u32), it); it = cds->next(it))
            pr_test("%u ", it_u32data(it));
        pr_test("");
    }

    {
        for (uint64_t i = 0; i < 5; ++i)
            cds->push_back(demo_u64, ds_u64data(i)); /* Required: always use the constructor */
        pr_test("uint64_t: ");
        for (vector_iterator_t it = cds->begin(demo_u64); it_ne(cds->end(demo_u64), it); it = cds->next(it))
            pr_test("%llu ", (unsigned long long)it_u64data(it));
        pr_test("");
    }

    {
        for (int8_t i = -2; i <= 2; ++i)
            cds->push_back(demo_s8, i); /* cds->push_back(demo_s8, ds_s8data(i)); */
        pr_test("int8_t: ");
        for (vector_iterator_t it = cds->begin(demo_s8); it_ne(cds->end(demo_s8), it); it = cds->next(it))
            pr_test("%d ", it_s8data(it));
        pr_test("");
    }

    {
        for (int16_t i = -2; i <= 2; ++i)
            cds->push_back(demo_s16, i); /* cds->push_back(demo_s16, ds_s16data(i)); */
        pr_test("int16_t: ");
        for (vector_iterator_t it = cds->begin(demo_s16); it_ne(cds->end(demo_s16), it); it = cds->next(it))
            pr_test("%d ", it_s16data(it));
        pr_test("");
    }

    {
        for (int32_t i = -2; i <= 2; ++i)
            cds->push_back(demo_s32, i); /* cds->push_back(demo_s32, ds_s32data(i)); */
        pr_test("int32_t: ");
        for (vector_iterator_t it = cds->begin(demo_s32); it_ne(cds->end(demo_s32), it); it = cds->next(it))
            pr_test("%d ", it_s32data(it));
        pr_test("");
    }

    {
        for (int64_t i = -2; i <= 2; ++i)
            cds->push_back(demo_s64, ds_s64data(i)); /* Required: always use the constructor */
        pr_test("int64_t: ");
        for (vector_iterator_t it = cds->begin(demo_s64); it_ne(cds->end(demo_s64), it); it = cds->next(it))
            pr_test("%lld ", (long long)it_s64data(it));
        pr_test("");
    }

    {
        for (uintptr_t i = 0; i < 5; ++i)
            cds->push_back(demo_uptr, i); /* cds->push_back(demo_uptr, ds_uptrdata(i)); */
        pr_test("uintptr_t: ");
        for (vector_iterator_t it = cds->begin(demo_uptr); it_ne(cds->end(demo_uptr), it); it = cds->next(it))
            pr_test("%zu ", it_uptrdata(it));
        pr_test("");
    }

    {
        for (intptr_t i = -2; i <= 2; ++i)
            cds->push_back(demo_iptr, i); /* cds->push_back(demo_iptr, ds_iptrdata(i)); */
        pr_test("intptr_t: ");
        for (vector_iterator_t it = cds->begin(demo_iptr); it_ne(cds->end(demo_iptr), it); it = cds->next(it))
            pr_test("%zd ", it_iptrdata(it));
        pr_test("");
    }

    {
        for (size_t i = 0; i < 5; ++i)
            cds->push_back(demo_size, i); /* cds->push_back(demo_size, ds_sizedata(i)); */
        pr_test("size_t: ");
        for (vector_iterator_t it = cds->begin(demo_size); it_ne(cds->end(demo_size), it); it = cds->next(it))
            pr_test("%zu ", it_sizedata(it));
        pr_test("");
    }

    {
        for (ssize_t i = -2; i <= 2; ++i)
            cds->push_back(demo_ssize, i); /* cds->push_back(demo_ssize, ds_ssizedata(i)); */
        pr_test("ssize_t: ");
        for (vector_iterator_t it = cds->begin(demo_ssize); it_ne(cds->end(demo_ssize), it); it = cds->next(it))
            pr_test("%zd ", it_ssizedata(it));
        pr_test("");
    }

    {
        for (int i = 0; i < 5; ++i)
            cds->push_back(demo_f, ds_fdata(i / 10.0f)); /* Required: always use the constructor */
        pr_test("float: ");
        for (vector_iterator_t it = cds->begin(demo_f); it_ne(cds->end(demo_f), it); it = cds->next(it))
            pr_test("%f ", it_fdata(it));
        pr_test("");
    }

    {
        for (int i = 0; i < 5; ++i)
            cds->push_back(demo_lf, ds_lfdata(i / 10.0)); /* Required: always use the constructor */
        pr_test("double: ");
        for (vector_iterator_t it = cds->begin(demo_lf); it_ne(cds->end(demo_lf), it); it = cds->next(it))
            pr_test("%lf ", it_lfdata(it));
        pr_test("");
    }

    {
        const char* names[] = { "sso", "inline string", "this string is longer than the inline buffer" };

        for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
            cds->push_back(demo_sso, ds_sdata(names[i])); /* Required: always use the constructor */

        pr_test("ds_sso_t: ");
        for (vector_iterator_t it = cds->begin(demo_sso); it_ne(cds->end(demo_sso), it); it = cds->next(it))
            pr_test("[ ( \"%s\" or \"%s\" or \"%s\" ) ], len [ %zu ] ", it_sdata(it), *it.sd, it.ssd->p, it.ssd->len);
        pr_test("");
    }

    VECTOR_DELETE(&demo_u8);
    VECTOR_DELETE(&demo_u16);
    VECTOR_DELETE(&demo_u32);
    VECTOR_DELETE(&demo_u64);
    VECTOR_DELETE(&demo_s8);
    VECTOR_DELETE(&demo_s16);
    VECTOR_DELETE(&demo_s32);
    VECTOR_DELETE(&demo_s64);
    VECTOR_DELETE(&demo_uptr);
    VECTOR_DELETE(&demo_iptr);
    VECTOR_DELETE(&demo_size);
    VECTOR_DELETE(&demo_ssize);
    VECTOR_DELETE(&demo_f);
    VECTOR_DELETE(&demo_lf);
    VECTOR_DELETE(&demo_sso);
}

/*
 *  String sort functions
 *
 *  SSO elements are `ds_sso_t` values, so comparisons take the element
 *  address and moves use the SSO move callback, which rebases the inline
 *  pointer. `char*` elements move as plain pointers with the default move.
 */
vector_sort_define_string_less(sort_str_sso_asc)
vector_sort_define_chars_less(sort_str_char_asc)

/*
 *  Strings
 *
 *  1. VECTOR_NEW_CHAR wraps char* and does not store the string length.
 *  2. VECTOR_NEW_SSO is the optimized string form for sequence
 *     containers (vector/deque/list) and container adaptors
 *     (queue/stack/priority_queue); SSO stores the string length.
 *  3. VECTOR_NEW_CHAR and VECTOR_NEW_SSO are interchangeable without
 *     changing the code (as long as you do not use the SSO length field).
 *     SSO just gives O(1) access to the length instead of an O(n) strlen.
 */
static void demo_string(void)
{
    vector_t* demo1 = VECTOR_NEW_SSO();    /* same as VECTOR_NEW_T(ds_sso_t) */
    vector_t* demo2 = VECTOR_NEW_CHAR();
    vector_t* demo3 = VECTOR_NEW_STRING(); /* same as VECTOR_NEW_SSO() */
    vector_size_t ret; (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };

    for (size_t i = 0; i < sizeof(id) / sizeof(id[0]); ++i) {
        cds->push_back(demo1, ds_sdata(id[i]));
        cds->push_back(demo2, ds_sdata(id[i]));
        cds->push_back(demo3, ds_sdata(id[i]));
    }
    // after for [ 'yj', 'jy', '123', '?混搭33*&', '中文', 'test' ]

    ret = cds->size(demo1); // size = 6

    /* Vector can traverse forward and backward; stack/queue cannot. */
    foreach_string(demo1);
    pr_test("");

    foreach_r_string(demo1);
    pr_test("");

    foreach_string(demo2);
    pr_test("");

    /* Sort the SSO vector ascending; the SSO move rebases inline strings. */
    sort_str_sso_asc(cds->begin(demo1), cds->end(demo1));
    pr_test("After ascending sort, sorted [ %d ]",
            sort_str_sso_asc_sorted(cds->begin(demo1), cds->end(demo1)));
    foreach_string(demo1);
    pr_test("");

    /* Sort the char* vector ascending; elements move as plain pointers. */
    sort_str_char_asc(cds->begin(demo2), cds->end(demo2));
    pr_test("After ascending sort, sorted [ %d ]",
            sort_str_char_asc_sorted(cds->begin(demo2), cds->end(demo2)));
    foreach_string(demo2);
    pr_test("");

    while (!cds->__empty(demo1)) {
        pr_test("[ ( \"%s\" or \"%s\" ) | \"%s\" | \"%s\" ]", *cds->back(demo1).sd, cds->back(demo1).ssd->p, *cds->back(demo2).sd, *cds->back(demo3).sd);
        pr_test("sso len [ %zu ], char* len [ %zu ]", cds->back(demo1).ssd->len, strlen(*cds->back(demo2).sd));
        cds->pop_back(demo1);
        cds->pop_back(demo2);
        cds->pop_back(demo3);
    }

    ret = cds->clear(demo1); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo2); // [ ], return 0(0 elements has been removed)
    ret = cds->clear(demo3); // [ ], return 0(0 elements has been removed)

    VECTOR_DELETE(&demo1);
    VECTOR_DELETE(&demo2);
    VECTOR_DELETE(&demo3);
}

/*
 *  Sort functions for demo_struct_t
 *
 *  Node mode stores demo_struct_t directly in the container. The type owns
 *  pointers and an SSO buffer, so sort must use the same move callback as
 *  the container instead of the default bitwise move.
 */
vector_sort_define_move(sort_demo_struct_node, demo_struct_t,
                        __demo_struct_node_sort_less, __demo_struct_node_sort_move)

/*
 *  Node mode
 *
 *  1. The element stores a demo_struct_t object, like
 *     std::vector<demo_class> in STL.
 *  2. demo_struct_t lives inside the element; no extra allocation is
 *     needed for the object itself.
 *  3. The caller builds the object, and ops->copy_data deep-copies it
 *     into the element.
 */
static class_vector_ops_t demo_struct_node_ops = {
    .valid_data = demo_struct_node_valid,
    .__eq       = __demo_struct_node_eq,
    .copy_data  = demo_struct_node_deep_copy,
    .free_data  = demo_struct_node_free,
    .__move     = __demo_struct_node_move,
};

static void demo_struct_node(void)
{
    vector_t* demo = VECTOR_NEW_COMPLATE(&demo_struct_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_struct_t, 0);

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            cds->push_back(demo, ds_cstdata(&obj));
            demo_struct_destructor(&obj);
        }
    }
    // before sort [ 5(e), 1(b), 4(d), 2(c), 1(a) ]

    pr_test("Before sort:");
    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_struct_t* s = (demo_struct_t*)it.d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                s->id, s->name1.p, s->name2, s->sub_struct->a, s->p == &s->c);
    }
    pr_test("");

    sort_demo_struct_node(cds->begin(demo), cds->end(demo)); // after sort [ 1(a), 1(b), 2(c), 4(d), 5(e) ]
    pr_test("After ascending sort, sorted [ %d ]",
            sort_demo_struct_node_sorted(cds->begin(demo), cds->end(demo)));
    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_struct_t* s = (demo_struct_t*)it.d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                s->id, s->name1.p, s->name2, s->sub_struct->a, s->p == &s->c);
    }
    pr_test("");

    VECTOR_DELETE(&demo);
}

/*
 *  Sort functions for demo_struct_t*
 *
 *  Heap mode stores pointers, so sort can compare the pointed objects and
 *  move the pointers with the default move.
 */
vector_sort_define(sort_demo_struct_heap, demo_struct_t*, __demo_struct_heap_sort_less)

/*
 *  Heap mode
 *
 *  1. The element stores a pointer, like std::vector<demo_class*> in STL.
 *  2. The memory for demo_struct_t is allocated in ops->copy_data.
 *  3. The caller builds the object, and it is deep-copied into the
 *     memory allocated by ops->copy_data.
 */
static class_vector_ops_t demo_struct_heap_ops = {
    .valid_data = demo_struct_heap_valid,
    .__eq       = __demo_struct_heap_eq,
    .copy_data  = demo_struct_heap_deep_copy,
    .free_data  = demo_struct_heap_free,
    .__move     = NULL,
};

static void demo_struct_heap(void)
{
    vector_t* demo = VECTOR_NEW_COMPLATE(&demo_struct_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_struct_t*, 0);

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t obj;

        if (__demo_struct_constructor(&obj, ids[i], names[i], names[i])) {
            cds->push_back(demo, ds_cstdata(&obj));
            demo_struct_destructor(&obj);
        }
    }
    // before sort [ 5(e), 1(b), 4(d), 2(c), 1(a) ]

    pr_test("Before sort:");
    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_struct_t* s = (demo_struct_t*)(intptr_t)*it.d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                s->id, s->name1.p, s->name2, s->sub_struct->a, s->p == &s->c);
    }
    pr_test("");

    sort_demo_struct_heap(cds->begin(demo), cds->end(demo)); // after sort [ 1(a), 1(b), 2(c), 4(d), 5(e) ]
    pr_test("After ascending sort, sorted [ %d ]",
            sort_demo_struct_heap_sorted(cds->begin(demo), cds->end(demo)));
    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_struct_t* s = (demo_struct_t*)(intptr_t)*it.d;

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                s->id, s->name1.p, s->name2, s->sub_struct->a, s->p == &s->c);
    }
    pr_test("");

    VECTOR_DELETE(&demo);
}

#if JDSC_ALIGNMEM
/*
 *  Aligned node mode
 *
 *  demo_align_struct_t is 512-byte aligned and stored directly in the
 *  element. Build with WITH_ALIGNMEM=y (JDSC_ALIGNMEM=1) to enable the
 *  aligned element allocation.
 */
static class_vector_ops_t demo_align_node_ops = {
    .valid_data = NULL,
    .__eq       = __demo_align_struct_node_eq,
    .copy_data  = demo_align_struct_node_copy,
    .free_data  = demo_align_struct_node_free,
    .__move     = NULL,
};

static void demo_align_struct_node(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = JDSC_ALIGNOF(demo_align_struct_t);

    pr_test("Align struct, sizeof [ %zu ] alignof [ %zu ]", size, align);

    vector_t* demo = VECTOR_NEW_COMPLATE(&demo_align_node_ops, SEQ_CNTR_DTYPE_STRUCT, demo_align_struct_t, 0);

    demo_align_struct_t objs[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    for (int i = 0; i < sizeof(objs) / sizeof(objs[0]); ++i)
        cds->push_back(demo, ds_cstdata(&objs[i]));

    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_align_struct_t* s = (demo_align_struct_t*)it.d;

        pr_test("Element: a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                s->a, s->b, s->c, s->d, s,
                (uintptr_t)s % 512);
    }

    VECTOR_DELETE(&demo);
}

/*
 *  Aligned heap mode
 *
 *  demo_align_struct_t is 512-byte aligned, and ops->copy_data
 *  allocates it on the heap; the element stores the pointer.
 */
static class_vector_ops_t demo_align_heap_ops = {
    .valid_data = NULL,
    .__eq       = __demo_align_struct_heap_eq,
    .copy_data  = demo_align_struct_heap_copy,
    .free_data  = demo_align_struct_heap_free,
    .__move     = NULL,
};

static void demo_align_struct_heap(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = JDSC_ALIGNOF(demo_align_struct_t);

    pr_test("Align struct, sizeof [ %zu ] alignof [ %zu ]", size, align);

    vector_t* demo = VECTOR_NEW_COMPLATE(&demo_align_heap_ops, SEQ_CNTR_DTYPE_PTR, demo_align_struct_t*, 0);

    demo_align_struct_t objs[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    for (int i = 0; i < sizeof(objs) / sizeof(objs[0]); ++i)
        cds->push_back(demo, ds_cstdata(&objs[i]));

    for (vector_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        demo_align_struct_t* s = (demo_align_struct_t*)(intptr_t)*it.d;

        pr_test("Element: a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                s->a, s->b, s->c, s->d, s,
                (uintptr_t)s % 512);
    }

    VECTOR_DELETE(&demo);
}
#endif /* JDSC_ALIGNMEM */

int main(void)
{
    /* interfaces */
    demo_base_and_iterator();
    demo_low_level_api();
    demo_about_insert();
    demo_about_erase();
    demo_about_find_and_replace();
    demo_about_sort();
    demo_reserve_and_resize_and_assign();

    /* preset types */
    demo_preset_type();

    /* strings */
    demo_string();

    /* struct */
    demo_struct_node();
    pr_test("");
    demo_struct_heap();
    pr_test("");

#if JDSC_ALIGNMEM
    /* aligned struct */
    demo_align_struct_node();
    pr_test("");
    demo_align_struct_heap();
#endif /* JDSC_ALIGNMEM */

    return 0;
}
