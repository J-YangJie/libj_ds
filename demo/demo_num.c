/*
  Numeric Data Demos
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

#include <inttypes.h>
#include <_log.h>
#include <deque/deque.h>
#include <map/map.h>
#include <multimap/multimap.h>
#include <multiset/multiset.h>
#include <set/set.h>
#include <vector/vector.h>

#define TAG "[demo_num]"

#define cds   cmap
#define cvec  cvector
#define cdeq  cdeque
#define cmset cmultiset
#define cmmap cmultimap

static bool demo_custom_key_valid(map_key_t key)
{
    return key > 0;
}

static bool demo_custom_key_lt(map_key_t left, map_key_t right)
{
    return left < right;
}

static const class_map_key_ops_t demo_custom_key_ops = {
    .valid_key = demo_custom_key_valid,
    .__lt      = demo_custom_key_lt,
    .copy_key  = NULL,
    .free_key  = NULL,
};

/*
 * The source below is identical on 32-bit and 64-bit.
 *
 * On 32-bit, insert/find/remove receive transient addresses for
 * uint64_t/int64_t/double. The key/value half ops box the 8 bytes;
 * the node stores the box address. On 64-bit the same 8 bytes live inline
 * in `ds_key_t`/`ds_value_t`. The iterator macros hide the difference.
 */

static void demo_map_u64_double(void)
{
    map_t* demo = MAP_NEW_KV(uint64_t, double);
    map_iterator_t it;

    for (uint64_t i = 0; i < 5; ++i)
        cds->insert(demo, ds_u64key(i * 100), ds_lfvalue((double)i / 2.0));

    /* A value that does not fit in 32 bits: this is the case that needs a
     * malloc'ed box on 32-bit and stays inline on 64-bit. */
    cds->insert(demo, ds_u64key(UINT64_C(0xFEDCBA9876543210)), ds_lfvalue(123.25));

    pr_test("map<uint64_t, double> size = %zd", cds->size(demo));

    it = cds->find(demo, ds_u64key(UINT64_C(0xFEDCBA9876543210)));
    if (it_ne(cds->end(demo), it))
        pr_test("  find 0x%" PRIx64 " -> %.2f", it_u64key(it), it_lfvalue(it));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("  (%" PRIu64 ", %.2f)", it_u64key(it), it_lfvalue(it));

    cds->insert_replace(demo, ds_u64key(100), ds_lfvalue(9.75));
    pr_test("  replace(100) -> %.2f", it_lfvalue(cds->find(demo, ds_u64key(100))));

    pr_test("  count(300) = %zd", cds->count(demo, ds_u64key(300)));
    pr_test("  remove(200) = %zd", cds->remove(demo, ds_u64key(200)));
    pr_test("  size = %zd", cds->size(demo));

    MAP_DELETE(&demo);
}

static void demo_map_double_string(void)
{
    map_t* demo = MAP_NEW_KV(double, string);
    map_iterator_t it;

    cds->insert(demo, ds_lfkey(2.5),  ds_svalue("two and a half"));
    cds->insert(demo, ds_lfkey(1.25), ds_svalue("one and a quarter"));
    cds->insert(demo, ds_lfkey(0.5),  ds_svalue("one half"));
    cds->insert(demo, ds_lfkey(3.75), ds_svalue("three and three quarters"));

    pr_test("map<double, string> size = %zd", cds->size(demo));

    it = cds->find(demo, ds_lfkey(1.25));
    if (it_ne(cds->end(demo), it))
        pr_test("  find %.2f -> %s", it_lfkey(it), it_svalue(it));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("  (%.2f, %s)", it_lfkey(it), it_svalue(it));

    pr_test("  remove(2.5) = %zd", cds->remove(demo, ds_lfkey(2.5)));

    MAP_DELETE(&demo);
}

static void demo_map_string_double(void)
{
    map_t* demo = MAP_NEW_KV(string, double);
    map_iterator_t it;

    cds->insert(demo, ds_skey("banana"), ds_lfvalue(2.0));
    cds->insert(demo, ds_skey("apple"),  ds_lfvalue(1.0));
    cds->insert(demo, ds_skey("cherry"), ds_lfvalue(3.0));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<string, double> (%s, %.2f)", it_skey(it), it_lfvalue(it));

    it = cds->find(demo, ds_skey("banana"));
    if (it_ne(cds->end(demo), it))
        pr_test("  find banana -> %.2f", it_lfvalue(it));

    pr_test("  remove(apple) = %zd", cds->remove(demo, ds_skey("apple")));

    MAP_DELETE(&demo);
}

static void demo_map_string_u64(void)
{
    map_t* demo = MAP_NEW_KV(string, uint64_t);
    map_iterator_t it;

    cds->insert(demo, ds_skey("answer"), ds_u64value(42));
    cds->insert(demo, ds_skey("big"),    ds_u64value(UINT64_C(0xFEDCBA9876543210)));
    cds->insert(demo, ds_skey("max"),    ds_u64value(UINT64_MAX));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<string, uint64_t> (%s, 0x%" PRIx64 ")", it_skey(it), it_u64value(it));

    it = cds->find(demo, ds_skey("big"));
    if (it_ne(cds->end(demo), it))
        pr_test("  find big -> 0x%" PRIx64, it_u64value(it));

    MAP_DELETE(&demo);
}

static void demo_map_custom_key_string(void)
{
    map_t* demo = MAP_NEW_KV_OPS(&demo_custom_key_ops,
                                 g_class_associative_container_value_ops_string());
    map_iterator_t it;

    cds->insert(demo, (map_key_t)7,  ds_svalue("seven"));
    cds->insert(demo, (map_key_t)3,  ds_svalue("three"));
    cds->insert(demo, (map_key_t)11, ds_svalue("eleven"));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<custom key, string> (%" PRIdPTR ", %s)", it_key(it), it_svalue(it));

    MAP_DELETE(&demo);
}

static map_t* demo_make_map_in_function(void)
{
    map_t* demo = MAP_NEW_KV_OPS(g_class_associative_container_key_ops_string(),
                                 g_class_associative_container_value_ops_double());

    cds->insert(demo, ds_skey("returned"), ds_lfvalue(99.5));
    return demo;
}

static void demo_map_compose_after_return(void)
{
    map_t* demo = demo_make_map_in_function();
    map_iterator_t it = cds->find(demo, ds_skey("returned"));

    if (it_ne(cds->end(demo), it))
        pr_test("map created by MAP_NEW_KV_OPS in a function: %.2f", it_lfvalue(it));

    MAP_DELETE(&demo);
}

static void demo_map_i64_i64(void)
{
    map_t* demo = MAP_NEW_KV(int64_t, int64_t);
    map_iterator_t it;

    cds->insert(demo, ds_s64key(-5), ds_s64value(-500));
    cds->insert(demo, ds_s64key(7),  ds_s64value(700));
    cds->insert(demo, ds_s64key(-9000000000000000000LL), ds_s64value(INT64_MIN));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<int64_t, int64_t> (%" PRId64 ", %" PRId64 ")", it_s64key(it), it_s64value(it));

    MAP_DELETE(&demo);
}

static void demo_map_i8_string(void)
{
    map_t* demo = MAP_NEW_KV(int8_t, string);
    map_iterator_t it;

    cds->insert(demo, ds_s8key(-3), ds_svalue("minus three"));
    cds->insert(demo, ds_s8key(9),  ds_svalue("nine"));
    cds->insert(demo, ds_s8key(0),  ds_svalue("zero"));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<int8_t, string> (%d, %s)", (int)it_s8key(it), it_svalue(it));

    MAP_DELETE(&demo);
}

static void demo_map_float_uint32(void)
{
    map_t* demo = MAP_NEW_KV(float, uint32_t);
    map_iterator_t it;

    cds->insert(demo, ds_fkey(-1.5f), ds_u32value(15));
    cds->insert(demo, ds_fkey(0.0f),  ds_u32value(0));
    cds->insert(demo, ds_fkey(3.25f), ds_u32value(325));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<float, uint32_t> (%.2f, %" PRIu32 ")", it_fkey(it), it_u32value(it));

    it = cds->find(demo, ds_fkey(3.25f));
    if (it_ne(cds->end(demo), it))
        pr_test("  find 3.25 -> %" PRIu32, it_u32value(it));

    MAP_DELETE(&demo);
}

static void demo_map_u16_i16(void)
{
    map_t* demo = MAP_NEW_KV(uint16_t, int16_t);
    map_iterator_t it;

    cds->insert(demo, ds_u16key(65535), ds_s16value(-32768));
    cds->insert(demo, ds_u16key(0),     ds_s16value(0));
    cds->insert(demo, ds_u16key(300),   ds_s16value(-300));

    for (it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(it))
        pr_test("map<uint16_t, int16_t> (%u, %d)", (unsigned)it_u16key(it), (int)it_s16value(it));

    MAP_DELETE(&demo);
}

static void demo_vector_double(void)
{
    vector_t* demo = VECTOR_NEW_T(double);
    vector_iterator_t it;

    cvec->push_back(demo, ds_lfdata(3.5));
    cvec->push_back(demo, ds_lfdata(1.25));
    cvec->push_back(demo, ds_lfdata(2.5));
    cvec->push_back(demo, ds_lfdata(1.25));

    pr_test("vector<double> size = %zd, capacity = %zd", cvec->size(demo), cvec->capacity(demo));

    it = cvec->find(demo, ds_lfdata(2.5));
    if (it_ne(cvec->end(demo), it))
        pr_test("  find 2.5 -> %.2f", it_lfdata(it));

    pr_test("  count(1.25) = %zd", cvec->count(demo, ds_lfdata(1.25)));

    for (it = cvec->begin(demo); it_ne(cvec->end(demo), it); it = cvec->next(it))
        pr_test("  %.2f", it_lfdata(it));

    pr_test("  remove(1.25) = %zd", cvec->remove(demo, ds_lfdata(1.25)));

    for (it = cvec->begin(demo); it_ne(cvec->end(demo), it); it = cvec->next(it))
        pr_test("  %.2f", it_lfdata(it));

    VECTOR_DELETE(&demo);
}

static void demo_vector_float(void)
{
    vector_t* demo = VECTOR_NEW_T(float);
    vector_iterator_t it;

    cvec->push_back(demo, ds_fdata(0.0f));
    cvec->push_back(demo, ds_fdata(1.5f));
    cvec->push_back(demo, ds_fdata(-2.25f));
    cvec->push_back(demo, ds_fdata(1.5f));

    pr_test("vector<float> size = %zd", cvec->size(demo));

    /* Numeric equality, not bit equality: -0.0f matches 0.0f. */
    it = cvec->find(demo, ds_fdata(-0.0f));
    if (it_ne(cvec->end(demo), it))
        pr_test("  find -0.0 -> %.2f", it_fdata(it));

    pr_test("  count(1.5) = %zd", cvec->count(demo, ds_fdata(1.5f)));

    for (it = cvec->begin(demo); it_ne(cvec->end(demo), it); it = cvec->next(it))
        pr_test("  %.2f", it_fdata(it));

    pr_test("  remove(1.5) = %zd", cvec->remove(demo, ds_fdata(1.5f)));

    VECTOR_DELETE(&demo);
}

static void demo_vector_u64(void)
{
    vector_t* demo = VECTOR_NEW_T(uint64_t);
    vector_iterator_t it;

    cvec->push_back(demo, ds_u64data(0));
    cvec->push_back(demo, ds_u64data(UINT64_C(0xFEDCBA9876543210)));
    cvec->push_back(demo, ds_u64data(42));

    for (it = cvec->begin(demo); it_ne(cvec->end(demo), it); it = cvec->next(it))
        pr_test("vector<uint64_t> 0x%" PRIx64, it_u64data(it));

    it = cvec->find(demo, ds_u64data(UINT64_C(0xFEDCBA9876543210)));
    if (it_ne(cvec->end(demo), it))
        pr_test("  find -> 0x%" PRIx64, it_u64data(it));

    pr_test("  remove(42) = %zd", cvec->remove(demo, ds_u64data(42)));

    VECTOR_DELETE(&demo);
}

static void demo_deque_double(void)
{
    deque_t* demo = DEQUE_NEW_T(double);
    deque_iterator_t it;

    cdeq->push_back(demo, ds_lfdata(3.5));
    cdeq->push_back(demo, ds_lfdata(1.25));
    cdeq->push_back(demo, ds_lfdata(2.5));
    cdeq->push_back(demo, ds_lfdata(1.25));

    pr_test("deque<double> size = %zd", cdeq->size(demo));

    it = cdeq->find(demo, ds_lfdata(2.5));
    if (it_ne(cdeq->end(demo), it))
        pr_test("  find 2.5 -> %.2f", it_lfdata(it));

    pr_test("  count(1.25) = %zd", cdeq->count(demo, ds_lfdata(1.25)));
    pr_test("  remove(1.25) = %zd", cdeq->remove(demo, ds_lfdata(1.25)));

    for (it = cdeq->begin(demo); it_ne(cdeq->end(demo), it); it = cdeq->next(it))
        pr_test("  %.2f", it_lfdata(it));

    DEQUE_DELETE(&demo);
}

static void demo_deque_u64(void)
{
    deque_t* demo = DEQUE_NEW_T(uint64_t);
    deque_iterator_t it;

    cdeq->push_back(demo, ds_u64data(0));
    cdeq->push_back(demo, ds_u64data(UINT64_C(0xFEDCBA9876543210)));
    cdeq->push_back(demo, ds_u64data(42));

    for (it = cdeq->begin(demo); it_ne(cdeq->end(demo), it); it = cdeq->next(it))
        pr_test("deque<uint64_t> 0x%" PRIx64, it_u64data(it));

    it = cdeq->find(demo, ds_u64data(UINT64_C(0xFEDCBA9876543210)));
    if (it_ne(cdeq->end(demo), it))
        pr_test("  find -> 0x%" PRIx64, it_u64data(it));

    pr_test("  remove(42) = %zd", cdeq->remove(demo, ds_u64data(42)));

    DEQUE_DELETE(&demo);
}

static void demo_set_i64(void)
{
    set_t* demo = SET_NEW_T(int64_t);
    set_iterator_t it;

    cset->insert(demo, ds_s64key(7));
    cset->insert(demo, ds_s64key(-5));
    cset->insert(demo, ds_s64key(INT64_MIN));

    for (it = cset->begin(demo); it_ne(cset->end(demo), it); it = cset->next(it))
        pr_test("set<int64_t> %" PRId64, __ds_token_to_s64(it_data(it)));

    pr_test("  remove(-5) = %zd", cset->remove(demo, ds_s64key(-5)));

    SET_DELETE(&demo);
}

static void demo_set_u64(void)
{
    set_t* demo = SET_NEW_T(uint64_t);
    set_iterator_t it;

    cset->insert(demo, ds_u64key(0));
    cset->insert(demo, ds_u64key(UINT64_C(0xFEDCBA9876543210)));
    cset->insert(demo, ds_u64key(42));

    pr_test("set<uint64_t> size = %zd", cset->size(demo));

    it = cset->find(demo, ds_u64key(UINT64_C(0xFEDCBA9876543210)));
    if (it_ne(cset->end(demo), it))
        pr_test("  find -> 0x%" PRIx64, __ds_token_to_u64(it_data(it)));

    for (it = cset->begin(demo); it_ne(cset->end(demo), it); it = cset->next(it))
        pr_test("  0x%" PRIx64, __ds_token_to_u64(it_data(it)));

    pr_test("  remove(42) = %zd", cset->remove(demo, ds_u64key(42)));

    SET_DELETE(&demo);
}

static void demo_set_double(void)
{
    set_t* demo = SET_NEW_T(double);
    set_iterator_t it;

    cset->insert(demo, ds_lfkey(2.5));
    cset->insert(demo, ds_lfkey(0.0));
    cset->insert(demo, ds_lfkey(-0.0)); /* numeric ordering: same key as 0.0 */
    cset->insert(demo, ds_lfkey(-1.25));

    pr_test("set<double> size = %zd", cset->size(demo));

    it = cset->find(demo, ds_lfkey(0.0));
    if (it_ne(cset->end(demo), it))
        pr_test("  find 0.0 -> %.2f", __ds_token_to_f64(it_data(it)));

    pr_test("  count(-0.0) = %zd", cset->count(demo, ds_lfkey(-0.0)));

    for (it = cset->begin(demo); it_ne(cset->end(demo), it); it = cset->next(it))
        pr_test("  %.2f", __ds_token_to_f64(it_data(it)));

    SET_DELETE(&demo);
}

static void demo_multiset_u64(void)
{
    multiset_t* demo = MULTISET_NEW_T(uint64_t);
    multiset_iterator_t it;

    cmset->insert(demo, ds_u64key(42));
    cmset->insert(demo, ds_u64key(0));
    cmset->insert(demo, ds_u64key(42));
    cmset->insert(demo, ds_u64key(UINT64_C(0xFEDCBA9876543210)));

    pr_test("multiset<uint64_t> size = %zd, count(42) = %zd",
            cmset->size(demo), cmset->count(demo, ds_u64key(42)));

    for (it = cmset->begin(demo); it_ne(cmset->end(demo), it); it = cmset->next(it))
        pr_test("  0x%" PRIx64, __ds_token_to_u64(it_data(it)));

    pr_test("  remove(42) = %zd", cmset->remove(demo, ds_u64key(42)));

    MULTISET_DELETE(&demo);
}

static void demo_multimap_double_u64(void)
{
    multimap_t* demo = MULTIMAP_NEW_KV(double, uint64_t);
    multimap_iterator_t it;

    cmmap->insert(demo, ds_lfkey(2.5), ds_u64value(25));
    cmmap->insert(demo, ds_lfkey(1.5), ds_u64value(UINT64_C(0xFEDCBA9876543210)));
    cmmap->insert(demo, ds_lfkey(2.5), ds_u64value(250));

    pr_test("multimap<double, uint64_t> size = %zd, count(2.5) = %zd",
            cmmap->size(demo), cmmap->count(demo, ds_lfkey(2.5)));

    for (it = cmmap->begin(demo); it_ne(cmmap->end(demo), it); it = cmmap->next(it))
        pr_test("  (%.2f, 0x%" PRIx64 ")", it_lfkey(it), it_u64value(it));

    MULTIMAP_DELETE(&demo);
}

int main(void)
{
#ifdef JDSC_64BIT
    pr_test("pointer size = 64-bit: associative 8-byte values are inline");
#else
    pr_test("pointer size = 32-bit: associative 8-byte values are boxed by key/value half ops");
#endif

    demo_map_u64_double();
    demo_map_double_string();
    demo_map_string_double();
    demo_map_string_u64();
    demo_map_custom_key_string();
    demo_map_compose_after_return();
    demo_map_i64_i64();
    demo_map_i8_string();
    demo_map_float_uint32();
    demo_map_u16_i16();
    demo_vector_double();
    demo_vector_float();
    demo_vector_u64();
    demo_deque_double();
    demo_deque_u64();
    demo_set_i64();
    demo_set_u64();
    demo_set_double();
    demo_multiset_u64();
    demo_multimap_double_u64();
    return 0;
}
