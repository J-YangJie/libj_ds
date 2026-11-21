/*
  Multiset Demos
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

#include <multiset/multiset.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include "demo_struct.h"

#define cds cmultiset
#define TAG "[demo_multiset]"

#define _from(x) ((x) ? (x) : "null string")

#define foreach()           { for (multiset_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(it))  pr_test("%zd", it_data(it)); }
#define foreach_kstring()   { for (multiset_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(it))  pr_test("%s", it_sdata(it)); }
#define foreach_r_kstring() { for (multiset_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(it)) pr_test("%s", it_sdata(it)); }

static void demo_base_and_iterator(void)
{
    multiset_t* demo = MULTISET_NEW_T(int32_t);
    multiset_size_t ret;
    multiset_count_t count;

    (void)ret;
    (void)count;

    for (int32_t i = 1; i <= 8; ++i)
        cds->insert(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->insert(demo, 6);        // [ 1, 2, 3, 4, 5, 6, 6, 7, 8 ]
    cds->insert(demo, 6);        // [ 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 ]

    ret   = cds->size(demo);     // size = 10
    count = cds->count(demo, 6); // count = 3
    count = cds->count(demo, 9); // count = 0 (key not found)

    foreach();                    // [ 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 ]
    pr_test("");

    {
        for (multiset_iterator_t   it = cds->end(demo);    it_ne(cds->begin(demo), it); )  { it = cds->prev(it);  pr_test("%zd", it_data(it)); }
        pr_test("");             // [ 8, 7, 6, 6, 6, 5, 4, 3, 2, 1 ]
        for (multiset_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(it))       pr_test("%zd", it_data(it));
        pr_test("");             // [ 8, 7, 6, 6, 6, 5, 4, 3, 2, 1 ]
        for (multiset_r_iterator_t it = cds->rend(demo);   it_ne(cds->rbegin(demo), it); ) { it = cds->rprev(it); pr_test("%zd", it_data(it)); }
        pr_test("");             // [ 1, 2, 3, 4, 5, 6, 6, 6, 7, 8 ]
    }

    MULTISET_DELETE(&demo);
}

static void demo_about_insert(void)
{
    multiset_t* demo = MULTISET_NEW_T(string);

    cds->insert(demo, ds_skey("j"));
    cds->insert(demo, ds_skey("and"));
    cds->insert(demo, ds_skey("abc"));
    cds->insert(demo, ds_skey("jerry")); // [ 'abc', 'and', 'j', 'jerry' ]
    cds->insert(demo, ds_skey("jerry")); // duplicate keys are kept

    foreach_kstring();
    pr_test("");

    foreach_r_kstring();
    pr_test("");

    MULTISET_DELETE(&demo);
}

static bool demo_remove_if_condition_k(ds_key_t key)
{
    return NULL != strstr(_from((char*)key), "3");
}

static void demo_about_erase(void)
{
    multiset_t* demo = MULTISET_NEW_T(string);
    multiset_iterator_t it;
    multiset_size_t ret;

    (void)it;
    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (int i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(demo, ds_skey(id[i]));
    cds->insert(demo, ds_skey(id[0]));
    cds->insert(demo, ds_skey(id[0]));
    // after for [ '123', '?混搭33*&', 'jy', 'test', 'yj', 'yj', 'yj', '中文' ]

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); ) {
        if (NULL != strstr(id[1], it_sdata(it)))
            it = cds->erase(demo, it);
        else
            it = cds->next(it);
    }
    // after for [ '123', '?混搭33*&', 'test', 'yj', 'yj', 'yj', '中文' ]

    it = cds->erase(demo, cds->prev(cds->end(demo)));         // [ '123', '?混搭33*&', 'test', 'yj', 'yj', 'yj' ], it -> end()

    ret = cds->remove(demo, ds_skey(id[0]));                  // [ '123', '?混搭33*&', 'test' ], return 3(3 elements have been removed)

    ret = cds->remove(demo, ds_skey("not-exist"));            // [ '123', '?混搭33*&', 'test' ], return 0(0 element has been removed)

    ret = cds->remove_if(demo, demo_remove_if_condition_k);   // [ 'test' ], return 2(2 elements have been removed)

    ret = cds->clear(demo);                                   // [ ], return 1(1 element has been removed)

    MULTISET_DELETE(&demo);
}

static void demo_about_find(void)
{
    multiset_t* demo = MULTISET_NEW_T(int64_t);
    multiset_iterator_t it;

    (void)it;

    for (int64_t i = 1; i <= 8; ++i)
        cds->insert(demo, ds_s64key(i));
    for (int64_t i = 1; i <= 8; ++i)
        cds->insert(demo, ds_s64key(i));
    // after for [ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8 ]

    it = cds->find(demo, ds_s64key(3));  // found, it -> the first 3
    it = cds->find(demo, ds_s64key(66)); // no found, it -> end()

    it = cds->lower_bound(demo, ds_s64key(0)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> 1
    it = cds->lower_bound(demo, ds_s64key(6)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> 6
    it = cds->lower_bound(demo, ds_s64key(8)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> 8
    it = cds->lower_bound(demo, ds_s64key(9)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> end()

    it = cds->upper_bound(demo, ds_s64key(0)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> 1
    it = cds->upper_bound(demo, ds_s64key(6)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> 7
    it = cds->upper_bound(demo, ds_s64key(8)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> end()
    it = cds->upper_bound(demo, ds_s64key(9)); pr_test("%lld", it_ne(cds->end(demo), it) ? (long long)it_s64data(it) : -1); // it -> end()
    pr_test("");

    MULTISET_DELETE(&demo);
}

/*
 *  Preset key types
 *
 *  1. MULTISET_NEW_T(_type) selects the native key operations.
 *     Use the matching `it_*data` accessors to read values.
 *  2. Use `ds_u64*`/`ds_s64*` for `uint64_t`/`int64_t`, `ds_f*`/`ds_lf*`
 *     for floating-point values, and `ds_skey` for strings.
 *     Other integer types may be passed directly; their `ds_*key`
 *     constructors are optional.
 *  3. Every preset type appears once as a key below.
 *  4. MULTISET_NEW_T_GT(_type) orders keys in descending order.
 *  5. Custom keys are demonstrated in demo_struct_heap.
 */
static void demo_preset_type(void)
{
    {
        multiset_t* demo = MULTISET_NEW_T(uint8_t);

        cds->insert(demo, 3); /* cds->insert(demo, ds_u8key(3)); */
        cds->insert(demo, 3); // duplicate keys are kept
        cds->insert(demo, 1);
        cds->insert(demo, 2);

        pr_test("uint8_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%u", it_u8data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(uint16_t);

        cds->insert(demo, 300); /* cds->insert(demo, ds_u16key(300)); */
        cds->insert(demo, 100);
        cds->insert(demo, 200);

        pr_test("uint16_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%u", it_u16data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(uint32_t);

        cds->insert(demo, 30000); /* cds->insert(demo, ds_u32key(30000)); */
        cds->insert(demo, 10000);
        cds->insert(demo, 20000);

        pr_test("uint32_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%u", it_u32data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(uint64_t);

        cds->insert(demo, ds_u64key(30000000000)); /* Required: always use the constructor */
        cds->insert(demo, ds_u64key(10000000000));
        cds->insert(demo, ds_u64key(20000000000));

        pr_test("uint64_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%llu", (unsigned long long)it_u64data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(int8_t);

        cds->insert(demo, -3); /* cds->insert(demo, ds_s8key(-3)); */
        cds->insert(demo, 1);
        cds->insert(demo, -1);

        pr_test("int8_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%d", it_s8data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(int16_t);

        cds->insert(demo, -300); /* cds->insert(demo, ds_s16key(-300)); */
        cds->insert(demo, 100);
        cds->insert(demo, -100);

        pr_test("int16_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%d", it_s16data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(int32_t);

        cds->insert(demo, -30000); /* cds->insert(demo, ds_s32key(-30000)); */
        cds->insert(demo, 10000);
        cds->insert(demo, -10000);

        pr_test("int32_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%d", it_s32data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(int64_t);

        cds->insert(demo, ds_s64key(-30000000000)); /* Required: always use the constructor */
        cds->insert(demo, ds_s64key(10000000000));
        cds->insert(demo, ds_s64key(-10000000000));

        pr_test("int64_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%lld", (long long)it_s64data(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(uintptr_t);

        cds->insert(demo, 30); /* cds->insert(demo, ds_uptrkey(30)); */
        cds->insert(demo, 10);
        cds->insert(demo, 20);

        pr_test("uintptr_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%zu", it_uptrdata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(intptr_t);

        cds->insert(demo, -3); /* cds->insert(demo, ds_iptrkey(-3)); */
        cds->insert(demo, 1);
        cds->insert(demo, -1);

        pr_test("intptr_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%zd", it_iptrdata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(size_t);

        cds->insert(demo, 300); /* cds->insert(demo, ds_sizekey(300)); */
        cds->insert(demo, 100);
        cds->insert(demo, 200);

        pr_test("size_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%zu", it_sizedata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(ssize_t);

        cds->insert(demo, -3); /* cds->insert(demo, ds_ssizekey(-3)); */
        cds->insert(demo, 1);
        cds->insert(demo, -1);

        pr_test("ssize_t:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%zd", it_ssizedata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(float);

        cds->insert(demo, ds_fkey(-1.5f)); /* Required: always use the constructor */
        cds->insert(demo, ds_fkey(0.5f));
        cds->insert(demo, ds_fkey(2.25f));

        pr_test("float:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%f", it_fdata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T_GT(double);

        cds->insert(demo, ds_lfkey(-1.5)); /* Required: always use the constructor */
        cds->insert(demo, ds_lfkey(0.5));
        cds->insert(demo, ds_lfkey(2.25));

        pr_test("double, descending:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%lf", it_lfdata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }

    {
        multiset_t* demo = MULTISET_NEW_T(string);

        cds->insert(demo, ds_skey("j")); /* Required: always use the constructor */
        cds->insert(demo, ds_skey("and"));
        cds->insert(demo, ds_skey("abc"));

        pr_test("string:");
        for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("%s", it_sdata(it));
        pr_test("");

        MULTISET_DELETE(&demo);
    }
}

/*
 *  Strings
 *
 *  MULTISET_NEW_T(string) uses the default string key operations.
 *  MULTISET_NEW_T_GT(string) orders string keys descending.
 */
static void demo_string(void)
{
    multiset_t* demo    = MULTISET_NEW_T(string);
    multiset_t* demo_gt = MULTISET_NEW_T_GT(string);
    const char* names[] = { "j", "and", "abc", "jerry" };
    multiset_iterator_t it;

    for (intptr_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        cds->insert(demo, ds_skey(names[i]));
        cds->insert(demo, ds_skey(names[i]));
        cds->insert(demo_gt, ds_skey(names[i]));
        cds->insert(demo_gt, ds_skey(names[i]));
    }

    pr_test("string, ascending:");
    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("%s", it_sdata(it));
    pr_test("");

    pr_test("string, descending:");
    for (it = cds->begin(demo_gt); it_ne(cds->end(demo_gt), it); it = cds->next(it))
        pr_test("%s", it_sdata(it));
    pr_test("");

    it = cds->find(demo, ds_skey("abc"));
    pr_test("find abc -> %s", it_ne(cds->end(demo), it) ? it_sdata(it) : "not found");
    pr_test("");

    MULTISET_DELETE(&demo);
    MULTISET_DELETE(&demo_gt);
}

/*
 *  Struct heap mode
 *
 *  Heap mode is the ownership model shared by every container: the copy
 *  callback allocates the object on the heap and the container stores the
 *  pointer. For multiset, `multiset_key_t` is token-sized, so heap mode is
 *  the general form for structured keys.
 *
 *  Sequence containers also support node mode, which stores the object
 *  directly in the element and avoids one allocation. That is a
 *  sequence-container optimization. See demo_vector/demo_list/demo_deque
 *  for node mode.
 */
static class_multiset_ops_t demo_struct_heap_ops = {
    .valid_key = demo_struct_heap_valid,
    .__lt      = __demo_struct_heap_lt,
    .copy_key  = demo_struct_heap_deep_copy,
    .free_key  = demo_struct_heap_free,
};

static void demo_struct_heap(void)
{
    multiset_t* demo = MULTISET_NEW_OPS(&demo_struct_heap_ops);

    int ids[] = { 5, 1, 4, 2, 1, 4 };
    char* names[] = { "e", "b", "d", "c", "a", "d" };

    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t key;

        if (!__demo_struct_constructor(&key, ids[i], names[i], names[i]))
            continue;

        cds->insert(demo, ds_cstkey(&key));
        demo_struct_destructor(&key);
    }

    pr_test("Struct heap mode:");
    for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        const demo_struct_t* key = (const demo_struct_t*)(intptr_t)it_data(it);

        pr_test("ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                key->id, key->name1.p, key->name2, key->sub_struct->a, key->p == &key->c);
    }
    pr_test("");

    {
        demo_struct_t key;

        if (__demo_struct_constructor(&key, 4, "d", "d")) {
            multiset_iterator_t it = cds->find(demo, ds_cstkey(&key));

            pr_test("find ID 4 -> %s", it_ne(cds->end(demo), it)
                    ? ((const demo_struct_t*)(intptr_t)it_data(it))->name1.p : "not found");
            pr_test("remove ID 4 -> %zd", cds->remove(demo, ds_cstkey(&key)));
            demo_struct_destructor(&key);
        }
    }
    pr_test("");

    MULTISET_DELETE(&demo);
}

#if JDSC_ALIGNMEM
/*
 *  Aligned struct heap mode
 *
 *  Aligned keys use the same heap ownership model: copy_key allocates
 *  aligned objects and stores their pointers in the multiset tokens. There
 *  is no aligned node mode for multiset because the multiset node stores
 *  only a token-sized key field.
 */
static class_multiset_ops_t demo_align_struct_heap_ops = {
    .valid_key = NULL,
    .__lt      = __demo_align_struct_heap_lt,
    .copy_key  = demo_align_struct_heap_copy,
    .free_key  = demo_align_struct_heap_free,
};

static void demo_align_struct_heap(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = (size_t)JDSC_ALIGNOF(demo_align_struct_t);

    multiset_t* demo = MULTISET_NEW_OPS(&demo_align_struct_heap_ops);

    demo_align_struct_t keys[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };

    pr_test("Align struct heap mode, sizeof [ %zu ] alignof [ %zu ]", size, align);

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
        cds->insert(demo, ds_cstkey(&keys[i]));

    for (multiset_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        const demo_align_struct_t* key = (const demo_align_struct_t*)(intptr_t)it_data(it);

        pr_test("a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                key->a, key->b, key->c, key->d, (const void*)key,
                (size_t)((uintptr_t)key % 512));
    }
    pr_test("");

    MULTISET_DELETE(&demo);
}
#endif /* JDSC_ALIGNMEM */

int main(void)
{
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();

    demo_preset_type();
    demo_string();

    demo_struct_heap();

#if JDSC_ALIGNMEM
    demo_align_struct_heap();
#endif /* JDSC_ALIGNMEM */

    return 0;
}
