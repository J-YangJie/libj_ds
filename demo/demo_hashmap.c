/*
  Hashmap Demos
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

#include <hashmap/hashmap.h>
#include <string.h>
#include <_log.h>
#include <operations/ds_ops_string.h>
#include "demo_struct.h"

#define cds chashmap
#define TAG "[demo_hashmap]"

#define foreach()           { for (hashmap_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(it))  pr_test("(%zd, %zd)",       it_key(it), it_value(it)); }
#define foreach_kstring()   { for (hashmap_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(it))  pr_test("(%s(0x%zx), %zd)", it_skey(it), it_hash(it), it_value(it)); }
#define foreach_r_kstring() { for (hashmap_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(it)) pr_test("(%s(0x%zx), %zd)", it_skey(it), it_hash(it), it_value(it)); }

static void demo_base_and_iterator(void)
{
    hashmap_t* demo = HASHMAP_NEW_KV(int32_t, int32_t);
    hashmap_size_t ret;
    hashmap_bcount_t bcount;
    hashmap_count_t count;

    (void)ret;
    (void)bcount;
    (void)count;

    for (int32_t i = 1; i <= 8; ++i)
        cds->insert(demo, i, i);
    // after for [ (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    cds->insert(demo, 35, 35);        // [ (1, 1), (2, 2), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    cds->insert(demo, 19, 19);        // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]

    ret    = cds->size(demo);         // size = 10
    ret    = cds->bucket_count(demo); // bucket count = 16
    bcount = cds->bucket_valid_count(demo); // bucket valid count = 8
    count  = cds->count(demo, 8);     // count = 1
    count  = cds->count(demo, 9);     // count = 0 (key not found)

    foreach();                         // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    pr_test("");

    {
        for (hashmap_iterator_t   it = cds->end(demo);    it_ne(cds->begin(demo), it); )  { it = cds->prev(it);  pr_test("(%zd, %zd)", it_key(it), it_value(it)); }
        pr_test("");                  // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (35, 35), (19, 19), (2, 2), (1, 1) ]
        for (hashmap_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(it))       pr_test("(%zd, %zd)", it_key(it), it_value(it));
        pr_test("");                  // [ (8, 8), (7, 7), (6, 6), (5, 5), (4, 4), (3, 3), (35, 35), (19, 19), (2, 2), (1, 1) ]
        for (hashmap_r_iterator_t it = cds->rend(demo);   it_ne(cds->rbegin(demo), it); ) { it = cds->rprev(it); pr_test("(%zd, %zd)", it_key(it), it_value(it)); }
        pr_test("");                  // [ (1, 1), (2, 2), (19, 19), (35, 35), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8) ]
    }

    HASHMAP_DELETE(&demo);
}

static void demo_about_insert(void)
{
    hashmap_t* demo = HASHMAP_NEW_KV(string, intptr_t);

    cds->insert(demo, ds_skey("j"),     6);
    cds->insert(demo, ds_skey("and"),   6);
    cds->insert(demo, ds_skey("abc"),   6);
    cds->insert(demo, ds_skey("jerry"), 6);
    cds->insert_replace(demo, ds_skey("jerry"), 99); // existing key: value replaced
    cds->insert(demo, ds_skey("jerry"), 100);        // duplicate key: ignored, returns a NULL iterator
    cds->insert_replace(demo, ds_skey("tom"), 7);    // new key: inserted

    foreach_kstring();
    pr_test("");

    foreach_r_kstring();
    pr_test("");

    HASHMAP_DELETE(&demo);
}

static void demo_about_erase(void)
{
    hashmap_t* demo = HASHMAP_NEW_KV(string, uint32_t);
    hashmap_iterator_t it;
    hashmap_size_t ret;

    (void)it;
    (void)ret;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (uint32_t i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(demo, ds_skey(id[i]), i);
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5), ('jy', 1) ]

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); ) {
        if (NULL != strstr(id[1], it_skey(it)))
            it = cds->erase(demo, it);
        else
            it = cds->next(it);
    }
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5) ]

    it = cds->erase(demo, cds->prev(cds->end(demo)));         // [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0) ], it -> end()

    ret = cds->remove(demo, ds_skey(id[0]));                  // [ ('?混搭33*&', 3), ('中文', 4), ('123', 2) ], return 1(1 element has been removed)

    ret = cds->remove(demo, ds_skey("not-exist"));            // [ ('?混搭33*&', 3), ('中文', 4), ('123', 2) ], return 0(0 element has been removed)

    ret = cds->clear(demo);                                   // [ ], return 3(3 elements have been removed)

    HASHMAP_DELETE(&demo);
}

static void demo_about_find(void)
{
    hashmap_t* demo = HASHMAP_NEW_KV(string, intptr_t);
    hashmap_iterator_t it;
    hashmap_count_t count;

    (void)it;
    (void)count;

    char id[][20] = { "yj", "jy", "123", "?混搭33*&", "中文", "test" };
    for (intptr_t i = 0; i < sizeof(id) / sizeof(id[0]); ++i)
        cds->insert(demo, ds_skey(id[i]), i);
    // after for [ ('?混搭33*&', 3), ('中文', 4), ('123', 2), ('yj', 0), ('test', 5), ('jy', 1) ]

    it = cds->find(demo, ds_skey(id[0]));   // found, it -> ('yj', 0)
    it = cds->find(demo, ds_skey("jerry")); // no found, it -> end()

    count = cds->count(demo, ds_skey(id[0]));   // count = 1
    count = cds->count(demo, ds_skey("jerry")); // count = 0 (key not found)

    HASHMAP_DELETE(&demo);
}

/*
 *  Preset key/value types
 *
 *  1. HASHMAP_NEW_KV(_ktype, _vtype) selects the native key/value operations.
 *     Use the matching `it_*key`/`it_*value` accessors to read values.
 *  2. Use `ds_u64*`/`ds_s64*` for `uint64_t`/`int64_t`, `ds_f*`/`ds_lf*`
 *     for floating-point values, and `ds_skey`/`ds_svalue` for strings.
 *     Other integer types may be passed directly; their `ds_*key`/
 *     `ds_*value` constructors are optional.
 *  3. Every preset type appears once as a key and once as a value below.
 *  4. Iteration order is determined by the hash table, not by key order.
 *  5. Custom keys/values are demonstrated in demo_struct_heap.
 */
static void demo_preset_type(void)
{
    {
        hashmap_t* demo = HASHMAP_NEW_KV(uint8_t, uint16_t);

        cds->insert(demo, 3, 300); /* cds->insert(demo, ds_u8key(3), ds_u16value(300)); */
        cds->insert(demo, 1, 100);
        cds->insert(demo, 2, 200);

        pr_test("uint8_t/uint16_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%u, %u)", it_u8key(it), it_u16value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(uint16_t, uint32_t);

        cds->insert(demo, 300, 30000); /* cds->insert(demo, ds_u16key(300), ds_u32value(30000)); */
        cds->insert(demo, 100, 10000);
        cds->insert(demo, 200, 20000);

        pr_test("uint16_t/uint32_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%u, %u)", it_u16key(it), it_u32value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(uint32_t, uint64_t);

        cds->insert(demo, 3, ds_u64value(30000000000)); /* key: ds_u32key(3) is optional; value: Required: always use the constructor */
        cds->insert(demo, 1, ds_u64value(10000000000));
        cds->insert(demo, 2, ds_u64value(20000000000));

        pr_test("uint32_t/uint64_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%u, %llu)", it_u32key(it), (unsigned long long)it_u64value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(uint64_t, int8_t);

        cds->insert(demo, ds_u64key(3), -30); /* key: Required: always use the constructor; value: ds_s8value(-30) is optional */
        cds->insert(demo, ds_u64key(1), 10);
        cds->insert(demo, ds_u64key(2), -10);

        pr_test("uint64_t/int8_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%llu, %d)", (unsigned long long)it_u64key(it), it_s8value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(int8_t, int16_t);

        cds->insert(demo, -3, -300); /* cds->insert(demo, ds_s8key(-3), ds_s16value(-300)); */
        cds->insert(demo, 1, 100);
        cds->insert(demo, -1, -100);

        pr_test("int8_t/int16_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%d, %d)", it_s8key(it), it_s16value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(int16_t, int32_t);

        cds->insert(demo, -3, -30000); /* cds->insert(demo, ds_s16key(-3), ds_s32value(-30000)); */
        cds->insert(demo, 1, 10000);
        cds->insert(demo, -1, -10000);

        pr_test("int16_t/int32_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%d, %d)", it_s16key(it), it_s32value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(int32_t, int64_t);

        cds->insert(demo, -3, ds_s64value(-30000000000)); /* key: ds_s32key(-3) is optional; value: Required: always use the constructor */
        cds->insert(demo, 1, ds_s64value(10000000000));
        cds->insert(demo, -1, ds_s64value(-10000000000));

        pr_test("int32_t/int64_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%d, %lld)", it_s32key(it), (long long)it_s64value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(int64_t, uintptr_t);

        cds->insert(demo, ds_s64key(-3), 30); /* key: Required: always use the constructor; value: ds_uptrvalue(30) is optional */
        cds->insert(demo, ds_s64key(1), 10);
        cds->insert(demo, ds_s64key(-1), 255);

        pr_test("int64_t/uintptr_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%lld, %zu)", (long long)it_s64key(it), it_uptrvalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(uintptr_t, intptr_t);

        cds->insert(demo, 3, -3); /* cds->insert(demo, ds_uptrkey(3), ds_iptrvalue(-3)); */
        cds->insert(demo, 1, 1);
        cds->insert(demo, 2, -2);

        pr_test("uintptr_t/intptr_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%zu, %zd)", it_uptrkey(it), it_iptrvalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(intptr_t, size_t);

        cds->insert(demo, -3, 300); /* cds->insert(demo, ds_iptrkey(-3), ds_sizevalue(300)); */
        cds->insert(demo, 1, 100);
        cds->insert(demo, -1, 200);

        pr_test("intptr_t/size_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%zd, %zu)", it_iptrkey(it), it_sizevalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(size_t, ssize_t);

        cds->insert(demo, 3, -3); /* cds->insert(demo, ds_sizekey(3), ds_ssizevalue(-3)); */
        cds->insert(demo, 1, 1);
        cds->insert(demo, 2, -2);

        pr_test("size_t/ssize_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%zu, %zd)", it_sizekey(it), it_ssizevalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(ssize_t, float);

        cds->insert(demo, -3, ds_fvalue(-1.5f)); /* key: ds_ssizekey(-3) is optional; value: Required: always use the constructor */
        cds->insert(demo, 1, ds_fvalue(0.5f));
        cds->insert(demo, -1, ds_fvalue(-2.25f));

        pr_test("ssize_t/float:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%zd, %f)", it_ssizekey(it), it_fvalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(float, double);

        cds->insert(demo, ds_fkey(-1.5f), ds_lfvalue(-15.5)); /* key/value: Required: always use the constructor */
        cds->insert(demo, ds_fkey(0.5f), ds_lfvalue(5.5));
        cds->insert(demo, ds_fkey(2.25f), ds_lfvalue(22.5));

        pr_test("float/double:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%f, %lf)", it_fkey(it), it_lfvalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(double, string);

        cds->insert(demo, ds_lfkey(-1.5), ds_svalue("minus one and a half")); /* key/value: Required: always use the constructor */
        cds->insert(demo, ds_lfkey(0.5), ds_svalue("one half"));
        cds->insert(demo, ds_lfkey(2.25), ds_svalue("two and a quarter"));

        pr_test("double/string:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%lf, %s)", it_lfkey(it), it_svalue(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }

    {
        hashmap_t* demo = HASHMAP_NEW_KV(string, uint8_t);

        cds->insert(demo, ds_skey("j"), 30); /* key: Required: always use the constructor; value: ds_u8value(30) is optional */
        cds->insert(demo, ds_skey("and"), 20);
        cds->insert(demo, ds_skey("abc"), 10);

        pr_test("string/uint8_t:");
        for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
            pr_test("(%s, %u)", it_skey(it), it_u8value(it));
        pr_test("");

        HASHMAP_DELETE(&demo);
    }
}

/*
 *  Strings and custom operations
 *
 *  HASHMAP_NEW_KV(string, ...) uses the default string hash key operations.
 *  HASHMAP_NEW_OPS(_ops) installs an explicit operation table; the table
 *  below uses the same default string callbacks.
 *  String values use copy/free callbacks, so the hashmap owns its copy.
 */
static class_hashmap_ops_t demo_string_ops = {
    .__hash      = __ds_ops_hash_default_string,
    .valid_key   = ds_ops_valid_key_default_string_max_128,
    .__lt        = __ds_ops_lt_default_string,
    .copy_key    = ds_ops_copy_data_default_string,
    .free_key    = ds_ops_free_data_default_string,
    .valid_value = NULL,
    .copy_value  = NULL,
    .free_value  = NULL,
};

static void demo_string(void)
{
    hashmap_t* demo_kv  = HASHMAP_NEW_KV(string, string);
    hashmap_t* demo_ops_map = HASHMAP_NEW_OPS(&demo_string_ops);
    const char* names[] = { "j", "and", "abc", "jerry" };
    hashmap_iterator_t it;

    for (intptr_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        cds->insert(demo_kv, ds_skey(names[i]), ds_svalue(names[i]));
        cds->insert(demo_ops_map, ds_skey(names[i]), i);
    }

    pr_test("string/string:");
    for (it = cds->begin(demo_kv); it_ne(cds->end(demo_kv), it); it = cds->next(it))
        pr_test("(%s(0x%zx), %s)", it_skey(it), it_hash(it), it_svalue(it));
    pr_test("");

    pr_test("custom string ops, string/intptr_t:");
    for (it = cds->begin(demo_ops_map); it_ne(cds->end(demo_ops_map), it); it = cds->next(it))
        pr_test("(%s(0x%zx), %zd)", it_skey(it), it_hash(it), it_iptrvalue(it));
    pr_test("");

    it = cds->find(demo_kv, ds_skey("abc"));
    pr_test("find abc -> %s", it_ne(cds->end(demo_kv), it) ? it_svalue(it) : "not found");
    pr_test("");

    HASHMAP_DELETE(&demo_kv);
    HASHMAP_DELETE(&demo_ops_map);
}

/*
 *  Bucket configuration
 *
 *  HASHMAP_NEW_3(_bucket_count_init, _bucket_count_max, _load_factor)
 *  sets the initial bucket count, the maximum bucket count (0 means the
 *  library default) and the load factor. The table grows when
 *  size > bucket_count * load_factor.
 */
static void demo_bucket(void)
{
    hashmap_t* demo = HASHMAP_NEW_3(4, 0, 0.75f);

    cds->insert(demo, 0, 0);
    pr_test("bucket count after first insert [ %zd ]", cds->bucket_count(demo));

    for (int i = 1; i < 8; ++i)
        cds->insert(demo, i, i);

    pr_test("after 8 inserts: size [ %zd ], bucket count [ %zd ], bucket valid count [ %zd ]",
            cds->size(demo), cds->bucket_count(demo), cds->bucket_valid_count(demo));
    pr_test("");

    HASHMAP_DELETE(&demo);
}

/*
 *  Struct heap mode
 *
 *  Heap mode is the ownership model shared by every container: the copy
 *  callback allocates the object on the heap and the container stores the
 *  pointer. For hashmap, `hashmap_key_t` and `hashmap_value_t` are
 *  token-sized, so heap mode is the general form for structured keys and
 *  values.
 *
 *  Sequence containers also support node mode, which stores the object
 *  directly in the element and avoids one allocation. That is a
 *  sequence-container optimization; hashmap nodes have no inline object
 *  storage. See demo_vector/demo_list/demo_deque for node mode.
 */
static class_hashmap_key_ops_t demo_struct_heap_key_ops = {
    .__hash    = __demo_struct_heap_hash,
    .valid_key = demo_struct_heap_valid,
    .__eq      = __demo_struct_heap_eq,
    .__lt      = __demo_struct_heap_lt,
    .copy_key  = demo_struct_heap_deep_copy,
    .free_key  = demo_struct_heap_free,
};

static class_hashmap_value_ops_t demo_struct_heap_value_ops = {
    .valid_value = demo_struct_heap_valid,
    .copy_value  = demo_struct_heap_deep_copy,
    .free_value  = demo_struct_heap_free,
};

static void demo_struct_heap(void)
{
    hashmap_t* demo = HASHMAP_NEW_KV_OPS_4(&demo_struct_heap_key_ops, &demo_struct_heap_value_ops, 0, 0, 0.0f, NULL);

    int ids[] = { 5, 1, 4, 2, 1 };
    char* names[] = { "e", "b", "d", "c", "a" };

    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        demo_struct_t key;
        demo_struct_t value;

        if (!__demo_struct_constructor(&key, ids[i], names[i], names[i]))
            continue;

        if (!__demo_struct_constructor(&value, ids[i] + 10, names[i], names[i])) {
            demo_struct_destructor(&key);
            continue;
        }

        cds->insert(demo, ds_cstkey(&key), ds_cstvalue(&value));
        demo_struct_destructor(&key);
        demo_struct_destructor(&value);
    }

    pr_test("Struct heap mode:");
    for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        const demo_struct_t* key   = (const demo_struct_t*)(intptr_t)it.d->key;
        const demo_struct_t* value = (const demo_struct_t*)(intptr_t)it.d->value;

        pr_test("Key ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ] | "
                "Value ID [ %d ] name1 [ %s ] name2 [ %s ] sub_struct->a [ %d ] p ok [ %d ]",
                key->id, key->name1.p, key->name2, key->sub_struct->a, key->p == &key->c,
                value->id, value->name1.p, value->name2, value->sub_struct->a, value->p == &value->c);
    }
    pr_test("");

    {
        demo_struct_t key;

        if (__demo_struct_constructor(&key, 4, "d", "d")) {
            hashmap_iterator_t it = cds->find(demo, ds_cstkey(&key));

            pr_test("find ID 4 -> %s", it_ne(cds->end(demo), it)
                    ? ((const demo_struct_t*)(intptr_t)it.d->value)->name1.p : "not found");
            demo_struct_destructor(&key);
        }
    }

    {
        demo_struct_t key;

        if (__demo_struct_constructor(&key, 2, "c", "c")) {
            pr_test("remove ID 2 -> %zd", cds->remove(demo, ds_cstkey(&key)));
            demo_struct_destructor(&key);
        }
    }
    pr_test("");

    HASHMAP_DELETE(&demo);
}

#if JDSC_ALIGNMEM
/*
 *  Aligned struct heap mode
 *
 *  Aligned keys and values use the same heap ownership model:
 *  copy_key/copy_value allocate aligned objects and store their pointers
 *  in the hashmap tokens. There is no aligned node mode for hashmap
 *  because the hashmap node stores only token-sized key/value fields.
 */
static ds_hash_t __demo_align_struct_hash(ds_key_t key)
{
    const demo_align_struct_t* s = (const demo_align_struct_t*)key;
    uintptr_t h = (uintptr_t)(unsigned char)s->a;

    h = h * 31 + (uintptr_t)s->b;
    h = h * 31 + (uintptr_t)s->c;
    h = h * 31 + (uintptr_t)s->d;
    return (ds_hash_t)h;
}

static class_hashmap_key_ops_t demo_align_struct_heap_key_ops = {
    .__hash    = __demo_align_struct_hash,
    .valid_key = NULL,
    .__eq      = __demo_align_struct_heap_eq,
    .__lt      = __demo_align_struct_heap_lt,
    .copy_key  = demo_align_struct_heap_copy,
    .free_key  = demo_align_struct_heap_free,
};

static class_hashmap_value_ops_t demo_align_struct_heap_value_ops = {
    .valid_value = NULL,
    .copy_value  = demo_align_struct_heap_copy,
    .free_value  = demo_align_struct_heap_free,
};

static void demo_align_struct_heap(void)
{
    const size_t size  = sizeof(demo_align_struct_t);
    const size_t align = (size_t)JDSC_ALIGNOF(demo_align_struct_t);

    hashmap_t* demo = HASHMAP_NEW_KV_OPS_4(&demo_align_struct_heap_key_ops, &demo_align_struct_heap_value_ops, 0, 0, 0.0f, NULL);

    demo_align_struct_t keys[] = {
        { 'd', 4, 40, 400 },
        { 'b', 2, 20, 200 },
        { 'a', 1, 10, 100 },
        { 'c', 3, 30, 300 },
    };
    demo_align_struct_t values[] = {
        { 'D', 4, 40, 400 },
        { 'B', 2, 20, 200 },
        { 'A', 1, 10, 100 },
        { 'C', 3, 30, 300 },
    };

    pr_test("Align struct heap mode, sizeof [ %zu ] alignof [ %zu ]", size, align);

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
        cds->insert(demo, ds_cstkey(&keys[i]), ds_cstvalue(&values[i]));

    for (hashmap_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it)) {
        const demo_align_struct_t* key   = (const demo_align_struct_t*)(intptr_t)it.d->key;
        const demo_align_struct_t* value = (const demo_align_struct_t*)(intptr_t)it.d->value;

        pr_test("Key a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ] | "
                "Value a [ %c ] b [ %d ] c [ %d ] d [ %ld ] addr [ %p ] %% 512 [ %zu ]",
                key->a, key->b, key->c, key->d, (const void*)key,
                (size_t)((uintptr_t)key % 512),
                value->a, value->b, value->c, value->d, (const void*)value,
                (size_t)((uintptr_t)value % 512));
    }
    pr_test("");

    HASHMAP_DELETE(&demo);
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
    demo_bucket();

    demo_struct_heap();

#if JDSC_ALIGNMEM
    demo_align_struct_heap();
#endif /* JDSC_ALIGNMEM */

    return 0;
}
