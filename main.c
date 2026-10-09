/*
  Performance Testing
  Copyright (C) 2021  YangJie <yangjie98765@yeah.net>

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

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iterator/iterator_inter.h>
#include <hashmap/hashmap.h>
#include <list/list.h>
#include <vector/vector.h>
#include <deque/deque.h>
#include <priority_queue/priority_queue.h>
#include <map/map.h>
#include <multimap/multimap.h>
#include <set/set.h>
#include <multiset/multiset.h>
#include <operations/ds_ops_string.h>   /* hashmap/map/multimap 的字符串基准要用到（sso 那个 deque.h 已经带进来了） */

#define cpqueue cpriority_queue

#define GET_DURATION(_data, _time) do { gettimeofday(&time_begin, NULL); _data gettimeofday(&time_end, NULL); \
                                        _time += time_end.tv_usec - time_begin.tv_usec + 1000000 * (time_end.tv_sec - time_begin.tv_sec); } while (0)

//#define DSL_V
#ifdef DSL_V
#define DSL(_ds, _method) (_ds)->_method
#else
#define DSL(_ds, _method) _ds##_##_method
#endif

//#define TEST_HASHMAP        1
//#define TEST_MAP            1
//#define TEST_SET            1
//#define TEST_MULTIMAP       1
//#define TEST_MULTISET       1
//#define TEST_LIST           1
//#define TEST_VECTOR         1
//#define TEST_PQUEUE         1

#ifndef TIMES_INSERT
#define TIMES_INSERT   10000000
#endif /* TIMES_INSERT */

#ifndef TIMES_FIND
#define TIMES_FIND     100000000
#endif /* TIMES_FIND */
#ifndef TIMES_FIND_V_L
#define TIMES_FIND_V_L 100
#endif /* TIMES_FIND_V_L */

#ifndef TIMES_REMOVE
#define TIMES_REMOVE   50000000
#endif /* TIMES_REMOVE */
#ifndef TIMES_REMOVE_V_L
#define TIMES_REMOVE_V_L 100
#endif /* TIMES_REMOVE_V_L */

#ifndef SORT_SEED
#define SORT_SEED 12345
#endif /* SORT_SEED */

#ifndef HASHMAP_CAPACITY_INIT
#define HASHMAP_CAPACITY_INIT 2 * TIMES_INSERT
#endif /* HASHMAP_CAPACITY_INIT */

#ifdef TEST_HASHMAP
#define DS_NAME      "hashmap"
#define TIME_DS      time_hashmap
#elif defined(TEST_MAP)
#define DS_NAME      "map"
#define TIME_DS      time_map
#elif defined(TEST_SET)
#define DS_NAME      "set"
#define TIME_DS      time_set
#elif defined(TEST_MULTIMAP)
#define DS_NAME      "multimap"
#define TIME_DS      time_multimap
#elif defined(TEST_MULTISET)
#define DS_NAME      "multiset"
#define TIME_DS      time_multiset
#elif defined(TEST_LIST)
#define DS_NAME      "list"
#define TIME_DS      time_list
#elif defined(TEST_VECTOR)
#define DS_NAME      "vector"
#define TIME_DS      time_vector
#elif defined(TEST_DEQUE)
#define DS_NAME      "deque"
#define TIME_DS      time_deque
#elif defined(TEST_PQUEUE)
#define DS_NAME      "priority_queue"
#define TIME_DS      time_pqueue
#else
#define DS_NAME      "unknown"
#define TIME_DS      time_vector
#endif

#if defined(TEST_LIST) || defined(TEST_VECTOR) || defined(TEST_DEQUE)
#define FIND_OPS   TIMES_FIND_V_L
#define REMOVE_OPS TIMES_REMOVE_V_L
#else
#define FIND_OPS   TIMES_FIND
#define REMOVE_OPS TIMES_REMOVE
#endif

#define TOUCH_TIMERS() do { (void)time_hashmap; (void)time_map; (void)time_set; \
                            (void)time_multimap; (void)time_multiset; (void)time_list; \
                            (void)time_vector; (void)time_pqueue; \
                            (void)time_deque; } while (0)


#include <sort/sort_intro.h>

#define DQ_MOVE_NUM(_d, _s)   (*(_d) = *(_s))
#define DQ_LT_NUM(_a, _b)     ((_a) < (_b))
#define DQ_GT_NUM(_a, _b)     ((_a) > (_b))

#ifdef DEQUE_STR_SSO
#define DQ_MOVE_STR(_d, _s)   ds_sso_move((_d), (_s))
#define DQ_LT_STR(_a, _b)     (0 > ds_sso_cmp(&(_a), &(_b)))
#define DQ_GT_STR(_a, _b)     (0 < ds_sso_cmp(&(_a), &(_b)))
#define DQ_STR_T              ds_sso_t
#else
#define DQ_MOVE_STR(_d, _s)   (*(_d) = *(_s))
#define DQ_LT_STR(_a, _b)     (0 > strcmp((const char*)(_a), (const char*)(_b)))
#define DQ_GT_STR(_a, _b)     (0 < strcmp((const char*)(_a), (const char*)(_b)))
#define DQ_STR_T              char*
#endif

I_DEQUE_SORT_DEFINE(sort_dq_int,  deque_data_t, DQ_MOVE_NUM, DQ_LT_NUM)
I_DEQUE_SORT_DEFINE(sort_dq_ides, deque_data_t, DQ_MOVE_NUM, DQ_GT_NUM)
I_DEQUE_SORT_DEFINE(sort_dq_str,  DQ_STR_T,     DQ_MOVE_STR, DQ_LT_STR)
I_DEQUE_SORT_DEFINE(sort_dq_sdes, DQ_STR_T,     DQ_MOVE_STR, DQ_GT_STR)



#define VT_MOVE_NUM(_d, _s)   (*(_d) = *(_s))
#define VT_LT_NUM(_a, _b)     ((_a) < (_b))
#define VT_GT_NUM(_a, _b)     ((_a) > (_b))

/* vector 的字符串元素类型跟 deque 用同一个开关（Makefile 的 vector 目标也传
   -DDEQUE_STR_SSO），所以这里保持和 DQ_ 完全一样的两分支：
   SSO 版元素是 ds_sso_t，char* 版元素是 char*。
   注意 I_VECTOR_SORT_DEFINE 的 _IT 是裸 _T*，排序调用要拿迭代器的 .cur。 */
#ifdef DEQUE_STR_SSO
#define VT_MOVE_STR(_d, _s)   ds_sso_move((_d), (_s))
#define VT_LT_STR(_a, _b)     (0 > ds_sso_cmp(&(_a), &(_b)))
#define VT_GT_STR(_a, _b)     (0 < ds_sso_cmp(&(_a), &(_b)))
#define VT_STR_T              ds_sso_t
#else
#define VT_MOVE_STR(_d, _s)   (*(_d) = *(_s))
#define VT_LT_STR(_a, _b)     (0 > strcmp((const char*)(_a), (const char*)(_b)))
#define VT_GT_STR(_a, _b)     (0 < strcmp((const char*)(_a), (const char*)(_b)))
#define VT_STR_T              char*
#endif

I_VECTOR_SORT_DEFINE(sort_vt_int,  vector_data_t, VT_MOVE_NUM, VT_LT_NUM)
I_VECTOR_SORT_DEFINE(sort_vt_ides, vector_data_t, VT_MOVE_NUM, VT_GT_NUM)
I_VECTOR_SORT_DEFINE(sort_vt_str,  VT_STR_T,      VT_MOVE_STR, VT_LT_STR)
I_VECTOR_SORT_DEFINE(sort_vt_sdes, VT_STR_T,      VT_MOVE_STR, VT_GT_STR)


#ifdef TEST_LIST
/* list 的 sort 自己要一个比较器参数：不传（NULL）或传 LIST_SORT_ASC/DESC 是按
   list_data_t 数值排，对字符串链没意义。所以 SSO 这边升/降序各给一个真的比较器。
   两侧都是「节点里存的值」的地址，SSO 槽里就是 ds_sso_t。 */
static inline bool list_sso_lt(list_data_t left, list_data_t right)
{
    return 0 > ds_sso_cmp((const ds_sso_t*)left, (const ds_sso_t*)right);
}

static inline bool list_sso_gt(list_data_t left, list_data_t right)
{
    return 0 < ds_sso_cmp((const ds_sso_t*)left, (const ds_sso_t*)right);
}

/* list 迭代器要带容器，所以这两条自己走一遍；it_data/it_sdata 的语义和基准里一致 */
static inline list_iterator_t list_it_from_back(list_t* _this, int n)
{
    list_iterator_t it = DSL(clist, end)(_this);

    while (n-- > 0)
        it = DSL(clist, prev)(_this, it);
    return it;
}

static inline int list_sorted_num(list_t* _this, int desc)
{
    list_data_t prev  = 0;
    int         first = 1;
    int         ok    = 1;

    for (list_iterator_t it = DSL(clist, begin)(_this); it_ne(DSL(clist, end)(_this), it); it = DSL(clist, next)(_this, it)) {
        list_data_t cur = it_data(it);

        if (!first && (desc ? cur > prev : cur < prev)) { ok = 0; break; }
        prev  = cur;
        first = 0;
    }
    return ok;
}

static inline int list_sorted_str(list_t* _this, int desc)
{
    const char* prev = NULL;
    int         ok   = 1;

    for (list_iterator_t it = DSL(clist, begin)(_this); it_ne(DSL(clist, end)(_this), it); it = DSL(clist, next)(_this, it)) {
        const char* cur = it_sdata(it);

        /* 升序违例：prev > cur（strcmp > 0）；降序违例：prev < cur（strcmp < 0） */
        if (NULL != prev && (desc ? 0 > strcmp(prev, cur) : 0 < strcmp(prev, cur))) { ok = 0; break; }
        prev = cur;
    }
    return ok;
}
#endif /* TEST_LIST */


static void test_i_for(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_i  = HASHMAP_NEW_3(HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_i      = MAP_NEW();
#elif TEST_SET
    set_t*            ds_set_i      = SET_NEW();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_i = MULTIMAP_NEW();
#elif TEST_MULTISET
    multiset_t*       ds_multiset_i = MULTISET_NEW();
#elif TEST_LIST
    list_t*           ds_list_i     = LIST_NEW();
#elif TEST_VECTOR
    vector_t*         ds_vector_i   = VECTOR_NEW();
#elif TEST_DEQUE
    deque_t*          ds_deque_i    = DEQUE_NEW();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PRIORITY_QUEUE_NEW();
#endif

    printf("%s\n", __func__);

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(chashmap, insert)(ds_hashmap_i, i, i);   }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmap, insert)(ds_map_i, i, i);           }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cset, insert)(ds_set_i, i);              }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmultimap, insert)(ds_multimap_i, i, i); }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cmultiset, insert)(ds_multiset_i, i);    }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, i);         }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, i);     }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, i);       }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cpqueue, push)(ds_pqueue_i, i);          }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, i);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, i);
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        /* unsupport */
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);   /* only 10^2 */
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector); /* only 10^2 */
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_FIND);
        }, time_deque);  /* only 10^2 */
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
    }

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_i);               }, time_map);
        MAP_DELETE(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_i);               }, time_set);
        SET_DELETE(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_i);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_i);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_i);             }, time_list);
        LIST_DELETE(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_i);         }, time_vector);
        VECTOR_DELETE(&ds_vector_i);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_i);           }, time_deque);
        DEQUE_DELETE(&ds_deque_i);
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_i);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
    }

}

static void test_i_rand(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_i  = HASHMAP_NEW_3(HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_i      = MAP_NEW();
#elif TEST_SET
    set_t*            ds_set_i      = SET_NEW();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_i = MULTIMAP_NEW();
#elif TEST_MULTISET
    multiset_t*       ds_multiset_i = MULTISET_NEW();
#elif TEST_LIST
    list_t*           ds_list_i     = LIST_NEW();
#elif TEST_VECTOR
    vector_t*         ds_vector_i   = VECTOR_NEW();
#elif TEST_DEQUE
    deque_t*          ds_deque_i    = DEQUE_NEW();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_i   = PRIORITY_QUEUE_NEW();
#endif

    srand(time(0));
    printf("%s\n", __func__);

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_i, rand() % TIMES_FIND, i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_i, rand() % TIMES_FIND, i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cset, insert)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_i, rand() % TIMES_FIND, i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_i, rand() % TIMES_FIND);
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_i, rand() % TIMES_FIND);
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_i);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_i, rand() % TIMES_FIND);
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_i);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_i, rand() % TIMES_FIND);
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_i);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_i);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_i, rand() % TIMES_FIND);
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_i);
        /* unsupport */
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_i, rand() % TIMES_FIND);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_i, rand() % TIMES_FIND);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_i, rand() % TIMES_FIND);
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_i, rand() % TIMES_FIND);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_i, rand() % TIMES_FIND);
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_i, rand() % TIMES_FIND);
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_i, rand() % TIMES_FIND);
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_i, rand() % TIMES_FIND);
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        /* count */
        {
            deque_count_t cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(cdeque, count)(ds_deque_i, rand() % TIMES_FIND); }, time_deque);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), cnt, DSL(cdeque, size)(ds_deque_i));
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, DSL(cdeque, next)(DSL(cdeque, begin)(ds_deque_i)), DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i))); }, time_deque);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(cdeque, size)(ds_deque_i), (time_deque / 1000));
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i), rand() % TIMES_FIND); }, time_deque);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_i, DSL(cdeque, begin)(ds_deque_i));                       }, time_deque);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_i, rand() % TIMES_FIND);                             }, time_deque);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_i);                                                   }, time_deque);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), rand() % TIMES_FIND);   }, time_deque);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_i, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_i)));      }, time_deque);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);                              }, time_deque);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_i);                                                    }, time_deque);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert front+10 + erase front+10 */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert back-10 + erase back-10 */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) - 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) - 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        DSL(cdeque, clear)(ds_deque_i);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, rand() % TIMES_FIND);

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_i, it, rand() % TIMES_FIND); }, time_deque);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_i, it); }, time_deque);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cdeque, clear)(ds_deque_i);
        {
            const deque_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_i, DSL(cdeque, end)(ds_deque_i), (deque_size_t)TIMES_INSERT, 9); }, time_deque);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_i, __cdeque_it(ds_deque_i, DSL(cdeque, size)(ds_deque_i) / 2), (deque_size_t)MID_N, 9); }, time_deque);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            {
                deque_size_t     s  = DSL(cdeque, size)(ds_deque_i);
                deque_iterator_t b  = __cdeque_it(ds_deque_i, (s - MID_N) / 2);
                deque_iterator_t e  = __cdeque_it(ds_deque_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_i, b, e); }, time_deque);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_deque / 1000), DSL(cdeque, size)(ds_deque_i));

            time_deque = 0;
            GET_DURATION({ removed = DSL(cdeque, remove)(ds_deque_i, 9); }, time_deque);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_i));
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, (deque_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort_dq_int(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000),
               sort_dq_int_sorted(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)),
               DSL(cdeque, size)(ds_deque_i));

        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_i, (deque_data_t)(rand()));

        time_deque = 0;
        GET_DURATION({ sort_dq_ides(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000),
               sort_dq_ides_sorted(DSL(cdeque, begin)(ds_deque_i), DSL(cdeque, end)(ds_deque_i)),
               DSL(cdeque, size)(ds_deque_i));
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        /* count */
        {
            vector_count_t cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(cvector, count)(ds_vector_i, rand() % TIMES_FIND); }, time_vector);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), cnt, DSL(cvector, size)(ds_vector_i));
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, DSL(cvector, next)(DSL(cvector, begin)(ds_vector_i)), DSL(cvector, prev)(DSL(cvector, end)(ds_vector_i))); }, time_vector);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(cvector, size)(ds_vector_i), (time_vector / 1000));
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, DSL(cvector, begin)(ds_vector_i), DSL(cvector, end)(ds_vector_i)); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_INSERT / 500 times */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { DSL(cvector, insert)(ds_vector_i, DSL(cvector, begin)(ds_vector_i), rand() % TIMES_FIND); }, time_vector);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { DSL(cvector, erase)(ds_vector_i, DSL(cvector, begin)(ds_vector_i));                       }, time_vector);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert/push back + erase/pop back */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, insert)(ds_vector_i, DSL(cvector, end)(ds_vector_i), rand() % TIMES_FIND);   }, time_vector);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, erase)(ds_vector_i, DSL(cvector, prev)(DSL(cvector, end)(ds_vector_i)));      }, time_vector);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);                              }, time_vector);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_i);                                                    }, time_vector);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert front+10 + erase front+10 */ /* only TIMES_INSERT / 500 times */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert back-10 + erase back-10 */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) - 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) - 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, rand() % TIMES_FIND);

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_i, it, rand() % TIMES_FIND); }, time_vector);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_i, it); }, time_vector);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i);
        {
            const vector_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_i, DSL(cvector, end)(ds_vector_i), (vector_size_t)TIMES_INSERT, 9); }, time_vector);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_i, __cvector_it(ds_vector_i, DSL(cvector, size)(ds_vector_i) / 2), (vector_size_t)MID_N, 9); }, time_vector);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            {
                vector_size_t     s  = DSL(cvector, size)(ds_vector_i);
                vector_iterator_t b  = __cvector_it(ds_vector_i, (s - MID_N) / 2);
                vector_iterator_t e  = __cvector_it(ds_vector_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(cvector, erase_range)(ds_vector_i, b, e); }, time_vector);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_vector / 1000), DSL(cvector, size)(ds_vector_i));

            time_vector = 0;
            GET_DURATION({ removed = DSL(cvector, remove)(ds_vector_i, 9); }, time_vector);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), removed, DSL(cvector, size)(ds_vector_i));
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, (vector_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort_vt_int((vector_data_t*)DSL(cvector, begin)(ds_vector_i).cur, (vector_data_t*)DSL(cvector, end)(ds_vector_i).cur); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000),
               sort_vt_int_sorted((vector_data_t*)DSL(cvector, begin)(ds_vector_i).cur, (vector_data_t*)DSL(cvector, end)(ds_vector_i).cur),
               DSL(cvector, size)(ds_vector_i));

        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_i); DSL(cvector, shrink_to_fit)(ds_vector_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_i, (vector_data_t)(rand()));

        time_vector = 0;
        GET_DURATION({ sort_vt_ides((vector_data_t*)DSL(cvector, begin)(ds_vector_i).cur, (vector_data_t*)DSL(cvector, end)(ds_vector_i).cur); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000),
               sort_vt_ides_sorted((vector_data_t*)DSL(cvector, begin)(ds_vector_i).cur, (vector_data_t*)DSL(cvector, end)(ds_vector_i).cur),
               DSL(cvector, size)(ds_vector_i));
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        /* count */
        {
            list_count_t cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(clist, count)(ds_list_i, rand() % TIMES_FIND); }, time_list);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_list / 1000), cnt, DSL(clist, size)(ds_list_i));
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ DSL(clist, erase_range)(ds_list_i, DSL(clist, next)(ds_list_i, DSL(clist, begin)(ds_list_i)), DSL(clist, prev)(ds_list_i, DSL(clist, end)(ds_list_i))); }, time_list);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(clist, size)(ds_list_i), (time_list / 1000));
        GET_DURATION({ DSL(clist, erase_range)(ds_list_i, DSL(clist, begin)(ds_list_i), DSL(clist, end)(ds_list_i)); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, insert)(ds_list_i, DSL(clist, begin)(ds_list_i), rand() % TIMES_FIND); }, time_list);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, erase)(ds_list_i, DSL(clist, begin)(ds_list_i));                       }, time_list);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_front)(ds_list_i, rand() % TIMES_FIND);                             }, time_list);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_front)(ds_list_i);                                                   }, time_list);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, insert)(ds_list_i, DSL(clist, end)(ds_list_i), rand() % TIMES_FIND);   }, time_list);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, erase)(ds_list_i, DSL(clist, prev)(ds_list_i, DSL(clist, end)(ds_list_i)));      }, time_list);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);                              }, time_list);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_back)(ds_list_i);                                                    }, time_list);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert front+10 + erase front+10 */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_i, it, rand() % TIMES_FIND); }, time_list);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_i, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_i, it); }, time_list);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert back-10 + erase back-10 */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = list_it_from_back(ds_list_i, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_i, it, rand() % TIMES_FIND); }, time_list);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = list_it_from_back(ds_list_i, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_i, it); }, time_list);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));


        /* insert mid + erase mid */ /* only TIMES_INSERT / 5 times */
        DSL(clist, clear)(ds_list_i);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_i, rand() % TIMES_FIND);
        {
            list_iterator_t mid = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { DSL(clist, insert)(ds_list_i, mid, rand() % TIMES_FIND); }, time_list);
            printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), DSL(clist, size)(ds_list_i));
        }

        {
            list_iterator_t mid = __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { mid = DSL(clist, erase)(ds_list_i, mid); }, time_list);
            printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), DSL(clist, size)(ds_list_i));
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(clist, clear)(ds_list_i);
        {
            const list_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_i, DSL(clist, end)(ds_list_i), (list_size_t)TIMES_INSERT, 9); }, time_list);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_i, __clist_it(ds_list_i, DSL(clist, size)(ds_list_i) / 2), (list_size_t)MID_N, 9); }, time_list);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            {
                list_size_t     s  = DSL(clist, size)(ds_list_i);
                list_iterator_t b  = __clist_it(ds_list_i, (s - MID_N) / 2);
                list_iterator_t e  = __clist_it(ds_list_i, (s + MID_N) / 2);
                GET_DURATION({ DSL(clist, erase_range)(ds_list_i, b, e); }, time_list);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_list / 1000), DSL(clist, size)(ds_list_i));

            time_list = 0;
            GET_DURATION({ removed = DSL(clist, remove)(ds_list_i, 9); }, time_list);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), removed, DSL(clist, size)(ds_list_i));
        }


        /* sort: 升序 —— cmp 传 NULL（等同 LIST_SORT_ASC）：对普通数字升序，不看 ops */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_i, LIST_SORT_ASC); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000),
               list_sorted_num(ds_list_i, 0),
               DSL(clist, size)(ds_list_i));

        /* sort: 降序 —— 方向由 cmp 决定，魔术数字 2 就是普通数字降序，不用再另开一条链 */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_i); for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_i, (list_data_t)(rand()));

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_i, LIST_SORT_DESC); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000),
               list_sorted_num(ds_list_i, 1),
               DSL(clist, size)(ds_list_i));
    }
#endif

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_i);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_i);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_i);               }, time_map);
        MAP_DELETE(&ds_map_i);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_i);               }, time_set);
        SET_DELETE(&ds_set_i);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_i);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_i);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_i);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_i);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_i);             }, time_list);
        LIST_DELETE(&ds_list_i);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_i);         }, time_vector);
        VECTOR_DELETE(&ds_vector_i);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_i);           }, time_deque);
        DEQUE_DELETE(&ds_deque_i);
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_i);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_i);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
    }
}

#ifndef S_POOL_LEN
#define S_POOL_LEN    10000000
#endif /* S_POOL_LEN */
#ifndef S_STR_LEN_MIN
#define S_STR_LEN_MIN 16
#endif /* S_STR_LEN_MIN */
#ifndef S_STR_LEN_MAX
#define S_STR_LEN_MAX 31
#endif /* S_STR_LEN_MAX */

static char* g_s_pool = NULL;

static void s_pool_init(void)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    if (g_s_pool)
        return;

    g_s_pool = malloc(S_POOL_LEN);
    if (!g_s_pool) {
        fprintf(stderr, "%s: pool malloc fail\n", __func__);
        exit(1);
    }
    for (int i = 0; i < S_POOL_LEN; ++i)
        g_s_pool[i] = charset[rand() % (int)(sizeof(charset) - 1)];
}

static char* s_pool_str(char* dst)
{
    int len   = S_STR_LEN_MIN + rand() % (S_STR_LEN_MAX - S_STR_LEN_MIN + 1); /* 16..31 */
    int index = rand() % (S_POOL_LEN - S_STR_LEN_MAX + 1);                    /* index+len ≤ S_POOL_LEN */

    for (int i = 0; i < len; ++i)
        dst[i] = g_s_pool[index + i];
    dst[len] = '\0';
    return dst;
}

static void test_s_rand(void)
{
    struct timeval time_begin, time_end;
    clock_t time_hashmap  = 0;
    clock_t time_map      = 0;
    clock_t time_set      = 0;
    clock_t time_multimap = 0;
    clock_t time_multiset = 0;
    clock_t time_list     = 0;
    clock_t time_vector   = 0;
    clock_t time_deque    = 0;
    clock_t time_pqueue   = 0;
    TOUCH_TIMERS();

#ifdef TEST_HASHMAP
    class_hashmap_ops_t tops_hashmap = {
        .__hash      = __ds_ops_hash_default_string,
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .__eq        = __ds_ops_eq_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MAP
    class_map_ops_t tops_map = {
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#elif TEST_MULTIMAP
    class_multimap_ops_t tops_multimap = {
        .valid_key   = ds_ops_valid_data_default_string,
        .__lt        = __ds_ops_lt_default_string,
        .copy_key    = ds_ops_copy_data_default_string,
        .free_key    = ds_ops_free_data_default_string,
    };
#endif

#ifdef TEST_HASHMAP
    hashmap_t*        ds_hashmap_s  = HASHMAP_NEW_OPS_3(&tops_hashmap, HASHMAP_CAPACITY_INIT, 0, 0.0);
    printf("Hashmap reserve: %d\n", HASHMAP_CAPACITY_INIT);
#elif TEST_MAP
    map_t*            ds_map_s      = MAP_NEW_OPS(&tops_map);
#elif TEST_SET
    set_t*            ds_set_s      = SET_NEW_STRING();
#elif TEST_MULTIMAP
    multimap_t*       ds_multimap_s = MULTIMAP_NEW_OPS(&tops_multimap);
#elif TEST_MULTISET
    multiset_t*       ds_multiset_s = MULTISET_NEW_STRING();
#elif TEST_LIST
    list_t*           ds_list_s     = LIST_NEW_STRING();
#elif TEST_VECTOR
    vector_t*         ds_vector_s   = VECTOR_NEW_STRING();
#elif TEST_DEQUE
    deque_t*          ds_deque_s    = DEQUE_NEW_STRING();
#elif TEST_PQUEUE
    priority_queue_t* ds_pqueue_s   = PRIORITY_QUEUE_NEW_STRING();
#endif

    srand(time(0));
    printf("%s\n", __func__);
    s_pool_init();

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(chashmap, insert)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst), i);
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmap, insert)(ds_map_s, (map_key_t)s_pool_str(dst), i);
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cset, insert)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultimap, insert)(ds_multimap_s, (multimap_key_t)s_pool_str(dst), i);
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cmultiset, insert)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(clist, push_back)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cvector, push_back)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cdeque, push_back)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_PQUEUE
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) {
            DSL(cpqueue, push)(ds_pqueue_s, (priority_queue_data_t)s_pool_str(dst));
        }, time_pqueue);
#endif

        printf("RESULT %s insert %s %zd %zd ms\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000));
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        size_t times_succ = 0;
        ds_size_t ds_size;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        ds_size = DSL(chashmap, size)(ds_hashmap_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            hashmap_iterator_t it = DSL(chashmap, find)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_hashmap);
#elif TEST_MAP
        ds_size = DSL(cmap, size)(ds_map_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            map_iterator_t it = DSL(cmap, find)(ds_map_s, (map_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_map);
#elif TEST_SET
        ds_size = DSL(cset, size)(ds_set_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            set_iterator_t it = DSL(cset, find)(ds_set_s, (set_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_set);
#elif TEST_MULTIMAP
        ds_size = DSL(cmultimap, size)(ds_multimap_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multimap_iterator_t it = DSL(cmultimap, find)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multimap);
#elif TEST_MULTISET
        ds_size = DSL(cmultiset, size)(ds_multiset_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND; ++i) {
            multiset_iterator_t it = DSL(cmultiset, find)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
            if (it.d && iterator_end() != it.d) times_succ++;
        }, time_multiset);
#elif TEST_LIST
        ds_size = DSL(clist, size)(ds_list_s);
        list_iterator_t iterator_end = DSL(clist, end)(ds_list_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            list_iterator_t it = DSL(clist, find)(ds_list_s, (list_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_list);
#elif TEST_VECTOR
        ds_size = DSL(cvector, size)(ds_vector_s);
        vector_iterator_t iterator_end = DSL(cvector, end)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = DSL(cvector, find)(ds_vector_s, (vector_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_vector);
#elif TEST_DEQUE
        ds_size = DSL(cdeque, size)(ds_deque_s);
        deque_iterator_t iterator_end = DSL(cdeque, end)(ds_deque_s);
        GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = DSL(cdeque, find)(ds_deque_s, (deque_data_t)s_pool_str(dst));
            if (iterator_end.d != it.d) times_succ++;
        }, time_deque);
#elif TEST_PQUEUE
        ds_size = DSL(cpqueue, size)(ds_pqueue_s);
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s find %s %zd %zd ms succ=%zu ds_size=%zd\n", __func__, DS_NAME, (ssize_t)FIND_OPS, (TIME_DS / 1000), times_succ, ds_size);
    }

    if (1) // if (0)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_deque    = 0;
        time_pqueue   = 0;

        ds_size_t removed = 0;
        char dst[S_STR_LEN_MAX + 1];

#ifdef TEST_HASHMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(chashmap, remove)(ds_hashmap_s, (hashmap_key_t)s_pool_str(dst));
        }, time_hashmap);
#elif TEST_MAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmap, remove)(ds_map_s, (map_key_t)s_pool_str(dst));
        }, time_map);
#elif TEST_SET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cset, remove)(ds_set_s, (set_key_t)s_pool_str(dst));
        }, time_set);
#elif TEST_MULTIMAP
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultimap, remove)(ds_multimap_s, (multimap_key_t)s_pool_str(dst));
        }, time_multimap);
#elif TEST_MULTISET
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE; ++i) {
            removed += DSL(cmultiset, remove)(ds_multiset_s, (multiset_key_t)s_pool_str(dst));
        }, time_multiset);
#elif TEST_LIST
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(clist, remove)(ds_list_s, (list_data_t)s_pool_str(dst));
        }, time_list);
#elif TEST_VECTOR
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cvector, remove)(ds_vector_s, (vector_data_t)s_pool_str(dst));
        }, time_vector);
#elif TEST_DEQUE
        GET_DURATION(for (int i = 0; i < TIMES_REMOVE_V_L; ++i) {
            removed += DSL(cdeque, remove)(ds_deque_s, (deque_data_t)s_pool_str(dst));
        }, time_deque);
#elif TEST_PQUEUE
        removed = 0;
        /* unsupport */
        (void)dst;
#endif

        printf("RESULT %s remove %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)REMOVE_OPS, (TIME_DS / 1000), removed);
    }

#ifdef TEST_DEQUE
    if (1)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((deque_data_t)s_pool_str(dst))

        /* count */
        {
            deque_count_t cnt = 0;

            time_deque = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(cdeque, count)(ds_deque_s, DS_ARG()); }, time_deque);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), cnt, DSL(cdeque, size)(ds_deque_s));
        }


        /* erase_range */
        time_deque = 0;
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, DSL(cdeque, next)(DSL(cdeque, begin)(ds_deque_s)), DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_s))); }, time_deque);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(cdeque, size)(ds_deque_s), (time_deque / 1000));
        GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);


        /* insert/push front + erase/pop front */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s), DS_ARG());       }, time_deque);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_s, DSL(cdeque, begin)(ds_deque_s));                  }, time_deque);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_front)(ds_deque_s, DS_ARG());                                   }, time_deque);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_front)(ds_deque_s);                                              }, time_deque);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert/push back + erase/pop back */
        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, insert)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), DS_ARG());         }, time_deque);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, erase)(ds_deque_s, DSL(cdeque, prev)(DSL(cdeque, end)(ds_deque_s))); }, time_deque);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, push_back)(ds_deque_s, DS_ARG());                                    }, time_deque);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cdeque, pop_back)(ds_deque_s);                                               }, time_deque);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert front+10 + erase front+10 */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert back-10 + erase back-10 */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < 20; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) - 10);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) - 10);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        DSL(cdeque, clear)(ds_deque_s);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);
            GET_DURATION({ DSL(cdeque, insert)(ds_deque_s, it, DS_ARG()); }, time_deque);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

        time_deque = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            deque_iterator_t it = __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2);
            GET_DURATION({ DSL(cdeque, erase)(ds_deque_s, it); }, time_deque);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cdeque, clear)(ds_deque_s);
        s_pool_str(fixbuf);
        {
            const deque_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_s, DSL(cdeque, end)(ds_deque_s), (deque_size_t)TIMES_INSERT, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ DSL(cdeque, insert_n)(ds_deque_s, __cdeque_it(ds_deque_s, DSL(cdeque, size)(ds_deque_s) / 2), (deque_size_t)MID_N, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            {
                deque_size_t     s  = DSL(cdeque, size)(ds_deque_s);
                deque_iterator_t b  = __cdeque_it(ds_deque_s, (s - MID_N) / 2);
                deque_iterator_t e  = __cdeque_it(ds_deque_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(cdeque, erase_range)(ds_deque_s, b, e); }, time_deque);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_deque / 1000), DSL(cdeque, size)(ds_deque_s));

            time_deque = 0;
            GET_DURATION({ removed = DSL(cdeque, remove)(ds_deque_s, (deque_data_t)fixbuf); }, time_deque);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_deque / 1000), removed, DSL(cdeque, size)(ds_deque_s));
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort_dq_str(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000),
               sort_dq_str_sorted(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)),
               DSL(cdeque, size)(ds_deque_s));

        srand(SORT_SEED);
        DSL(cdeque, clear)(ds_deque_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cdeque, push_back)(ds_deque_s, DS_ARG());

        time_deque = 0;
        GET_DURATION({ sort_dq_sdes(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)); }, time_deque);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_deque / 1000),
               sort_dq_sdes_sorted(DSL(cdeque, begin)(ds_deque_s), DSL(cdeque, end)(ds_deque_s)),
               DSL(cdeque, size)(ds_deque_s));

#undef DS_ARG
    }
#endif

#ifdef TEST_VECTOR
    if (1)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((vector_data_t)s_pool_str(dst))

        /* count */
        {
            vector_count_t cnt = 0;

            time_vector = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(cvector, count)(ds_vector_s, DS_ARG()); }, time_vector);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), cnt, DSL(cvector, size)(ds_vector_s));
        }


        /* erase_range */
        time_vector = 0;
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, DSL(cvector, next)(DSL(cvector, begin)(ds_vector_s)), DSL(cvector, prev)(DSL(cvector, end)(ds_vector_s))); }, time_vector);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(cvector, size)(ds_vector_s), (time_vector / 1000));
        GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, DSL(cvector, begin)(ds_vector_s), DSL(cvector, end)(ds_vector_s)); }, time_vector);


        /* insert/push front + erase/pop front */ /* only TIMES_INSERT / 500 times */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { DSL(cvector, insert)(ds_vector_s, DSL(cvector, begin)(ds_vector_s), DS_ARG());       }, time_vector);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT / 500; ++i) { DSL(cvector, erase)(ds_vector_s, DSL(cvector, begin)(ds_vector_s));                  }, time_vector);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert/push back + erase/pop back */
        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, insert)(ds_vector_s, DSL(cvector, end)(ds_vector_s), DS_ARG());         }, time_vector);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, erase)(ds_vector_s, DSL(cvector, prev)(DSL(cvector, end)(ds_vector_s))); }, time_vector);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, push_back)(ds_vector_s, DS_ARG());                                    }, time_vector);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(cvector, pop_back)(ds_vector_s);                                               }, time_vector);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert front+10 + erase front+10 */ /* only TIMES_INSERT / 500 times */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT / 500; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 500, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert back-10 + erase back-10 */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < 20; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) - 10);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) - 10);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert mid + erase mid */ /* only TIMES_FIND_V_L times */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2);
            GET_DURATION({ DSL(cvector, insert)(ds_vector_s, it, DS_ARG()); }, time_vector);
        }
        printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

        time_vector = 0;
        for (int i = 0; i < TIMES_FIND_V_L; ++i) {
            vector_iterator_t it = __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2);
            GET_DURATION({ DSL(cvector, erase)(ds_vector_s, it); }, time_vector);
        }
        printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s);
        s_pool_str(fixbuf);
        {
            const vector_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_s, DSL(cvector, end)(ds_vector_s), (vector_size_t)TIMES_INSERT, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            GET_DURATION({ DSL(cvector, insert_n)(ds_vector_s, __cvector_it(ds_vector_s, DSL(cvector, size)(ds_vector_s) / 2), (vector_size_t)MID_N, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            {
                vector_size_t     s  = DSL(cvector, size)(ds_vector_s);
                vector_iterator_t b  = __cvector_it(ds_vector_s, (s - MID_N) / 2);
                vector_iterator_t e  = __cvector_it(ds_vector_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(cvector, erase_range)(ds_vector_s, b, e); }, time_vector);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_vector / 1000), DSL(cvector, size)(ds_vector_s));

            time_vector = 0;
            GET_DURATION({ removed = DSL(cvector, remove)(ds_vector_s, (vector_data_t)fixbuf); }, time_vector);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_vector / 1000), removed, DSL(cvector, size)(ds_vector_s));
        }


        /* sort */
        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort_vt_str((VT_STR_T*)DSL(cvector, begin)(ds_vector_s).cur, (VT_STR_T*)DSL(cvector, end)(ds_vector_s).cur); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000),
               sort_vt_str_sorted((VT_STR_T*)DSL(cvector, begin)(ds_vector_s).cur, (VT_STR_T*)DSL(cvector, end)(ds_vector_s).cur),
               DSL(cvector, size)(ds_vector_s));

        srand(SORT_SEED);
        DSL(cvector, clear)(ds_vector_s); DSL(cvector, shrink_to_fit)(ds_vector_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(cvector, push_back)(ds_vector_s, DS_ARG());

        time_vector = 0;
        GET_DURATION({ sort_vt_sdes((VT_STR_T*)DSL(cvector, begin)(ds_vector_s).cur, (VT_STR_T*)DSL(cvector, end)(ds_vector_s).cur); }, time_vector);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_vector / 1000),
               sort_vt_sdes_sorted((VT_STR_T*)DSL(cvector, begin)(ds_vector_s).cur, (VT_STR_T*)DSL(cvector, end)(ds_vector_s).cur),
               DSL(cvector, size)(ds_vector_s));

#undef DS_ARG
    }
#endif

#ifdef TEST_LIST
    if (1)
    {
        char dst[S_STR_LEN_MAX + 1];
        char fixbuf[S_STR_LEN_MAX + 1];

#define DS_ARG()   ((list_data_t)s_pool_str(dst))

        /* count */
        {
            list_count_t cnt = 0;

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_FIND_V_L; ++i) { cnt += DSL(clist, count)(ds_list_s, DS_ARG()); }, time_list);
            printf("RESULT %s count        %s %zd %zd ms cnt=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_FIND_V_L, (time_list / 1000), cnt, DSL(clist, size)(ds_list_s));
        }


        /* erase_range */
        time_list = 0;
        GET_DURATION({ DSL(clist, erase_range)(ds_list_s, DSL(clist, next)(ds_list_s, DSL(clist, begin)(ds_list_s)), DSL(clist, prev)(ds_list_s, DSL(clist, end)(ds_list_s))); }, time_list);
        printf("RESULT %s erase_range  %s %zd %zd ms\n", __func__, DS_NAME, TIMES_INSERT - DSL(clist, size)(ds_list_s), (time_list / 1000));
        GET_DURATION({ DSL(clist, erase_range)(ds_list_s, DSL(clist, begin)(ds_list_s), DSL(clist, end)(ds_list_s)); }, time_list);


        /* insert/push front + erase/pop front */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, insert)(ds_list_s, DSL(clist, begin)(ds_list_s), DS_ARG());       }, time_list);
        printf("RESULT %s insert front %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, erase)(ds_list_s, DSL(clist, begin)(ds_list_s));                  }, time_list);
        printf("RESULT %s erase front  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_front)(ds_list_s, DS_ARG());                                   }, time_list);
        printf("RESULT %s push_front   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_front)(ds_list_s);                                              }, time_list);
        printf("RESULT %s pop_front    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert/push back + erase/pop back */
        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, insert)(ds_list_s, DSL(clist, end)(ds_list_s), DS_ARG());         }, time_list);
        printf("RESULT %s insert back  %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, erase)(ds_list_s, DSL(clist, prev)(ds_list_s, DSL(clist, end)(ds_list_s))); }, time_list);
        printf("RESULT %s erase back   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, push_back)(ds_list_s, DS_ARG());                                    }, time_list);
        printf("RESULT %s push_back    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        GET_DURATION(for (int i = 0; i < TIMES_INSERT; ++i) { DSL(clist, pop_back)(ds_list_s);                                               }, time_list);
        printf("RESULT %s pop_back     %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert front+10 + erase front+10 */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_s, it, DS_ARG()); }, time_list);
        }
        printf("RESULT %s insert@+10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = __clist_it(ds_list_s, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_s, it); }, time_list);
        }
        printf("RESULT %s erase@+10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert back-10 + erase back-10 */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < 20; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = list_it_from_back(ds_list_s, 10);
            GET_DURATION({ DSL(clist, insert)(ds_list_s, it, DS_ARG()); }, time_list);
        }
        printf("RESULT %s insert@-10   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

        time_list = 0;
        for (int i = 0; i < TIMES_INSERT; ++i) {
            list_iterator_t it = list_it_from_back(ds_list_s, 10);
            GET_DURATION({ DSL(clist, erase)(ds_list_s, it); }, time_list);
        }
        printf("RESULT %s erase@-10    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));


        /* insert mid + erase mid */ /* only TIMES_INSERT / 5 times */
        DSL(clist, clear)(ds_list_s);
        for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());
        {
            list_iterator_t mid = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { DSL(clist, insert)(ds_list_s, mid, DS_ARG()); }, time_list);
            printf("RESULT %s insert@mid   %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), DSL(clist, size)(ds_list_s));
        }

        {
            list_iterator_t mid = __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2);

            time_list = 0;
            GET_DURATION(for (int i = 0; i < TIMES_INSERT / 5; ++i) { mid = DSL(clist, erase)(ds_list_s, mid); }, time_list);
            printf("RESULT %s erase@mid    %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT / 5, (time_list / 1000), DSL(clist, size)(ds_list_s));
        }


        /* insert_n + insert_n mid + erase_range mid + remove all */
        DSL(clist, clear)(ds_list_s);
        s_pool_str(fixbuf);
        {
            const list_data_t MID_N = TIMES_INSERT / 5;
            ds_size_t removed;

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_s, DSL(clist, end)(ds_list_s), (list_size_t)TIMES_INSERT, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %s insert_n all %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ DSL(clist, insert_n)(ds_list_s, __clist_it(ds_list_s, DSL(clist, size)(ds_list_s) / 2), (list_size_t)MID_N, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %s insert_n mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            {
                list_size_t     s  = DSL(clist, size)(ds_list_s);
                list_iterator_t b  = __clist_it(ds_list_s, (s - MID_N) / 2);
                list_iterator_t e  = __clist_it(ds_list_s, (s + MID_N) / 2);
                GET_DURATION({ DSL(clist, erase_range)(ds_list_s, b, e); }, time_list);
            }
            printf("RESULT %s erase_range mid %s %zd %zd ms size=%zd\n", __func__, DS_NAME, (ssize_t)MID_N, (time_list / 1000), DSL(clist, size)(ds_list_s));

            time_list = 0;
            GET_DURATION({ removed = DSL(clist, remove)(ds_list_s, (list_data_t)fixbuf); }, time_list);
            printf("RESULT %s remove all   %s %zd %zd ms removed=%zd size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (time_list / 1000), removed, DSL(clist, size)(ds_list_s));
        }


        /* sort: 升序 —— 显式传字符串比较器（cmp 不传就会按指针数值排，对字符串链没意义）*/
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_s, list_sso_lt); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=asc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000),
               list_sorted_str(ds_list_s, 0),
               DSL(clist, size)(ds_list_s));

        /* sort: 降序 —— 同一个容器，换个比较器就行 */
        srand(SORT_SEED);
        DSL(clist, clear)(ds_list_s); for (int i = 0; i < TIMES_INSERT; ++i) DSL(clist, push_back)(ds_list_s, DS_ARG());

        time_list = 0;
        GET_DURATION({ DSL(clist, sort)(ds_list_s, list_sso_gt); }, time_list);
        printf("RESULT %s sort         %s %zd %zd ms dir=desc sorted=%d size=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT,
               (time_list / 1000),
               list_sorted_str(ds_list_s, 1),
               DSL(clist, size)(ds_list_s));
#undef DS_ARG
    }
#endif

    if (1)
    {
        time_hashmap  = 0;
        time_map      = 0;
        time_set      = 0;
        time_multimap = 0;
        time_multiset = 0;
        time_list     = 0;
        time_vector   = 0;
        time_pqueue   = 0;
        time_deque    = 0;

        ds_size_t removed = 0;

#ifdef TEST_HASHMAP
        GET_DURATION({ removed = DSL(chashmap, clear)(ds_hashmap_s);       }, time_hashmap);
        HASHMAP_DELETE(&ds_hashmap_s);
#elif TEST_MAP
        GET_DURATION({ removed = DSL(cmap, clear)(ds_map_s);               }, time_map);
        MAP_DELETE(&ds_map_s);
#elif TEST_SET
        GET_DURATION({ removed = DSL(cset, clear)(ds_set_s);               }, time_set);
        SET_DELETE(&ds_set_s);
#elif TEST_MULTIMAP
        GET_DURATION({ removed = DSL(cmultimap, clear)(ds_multimap_s);     }, time_multimap);
        MULTIMAP_DELETE(&ds_multimap_s);
#elif TEST_MULTISET
        GET_DURATION({ removed = DSL(cmultiset, clear)(ds_multiset_s);     }, time_multiset);
        MULTISET_DELETE(&ds_multiset_s);
#elif TEST_LIST
        GET_DURATION({ removed = DSL(clist, clear)(ds_list_s);             }, time_list);
        LIST_DELETE(&ds_list_s);
#elif TEST_VECTOR
        GET_DURATION({ removed = DSL(cvector, clear)(ds_vector_s);         }, time_vector);
        VECTOR_DELETE(&ds_vector_s);
#elif TEST_DEQUE
        GET_DURATION({ removed = DSL(cdeque, clear)(ds_deque_s);           }, time_deque);
        DEQUE_DELETE(&ds_deque_s);
#elif TEST_PQUEUE
        GET_DURATION({ removed = DSL(cpqueue, clear)(ds_pqueue_s);         }, time_pqueue);
        PRIORITY_QUEUE_DELETE(&ds_pqueue_s);
#endif

        printf("RESULT %s deinit %s %zd %zd ms removed=%zd\n", __func__, DS_NAME, (ssize_t)TIMES_INSERT, (TIME_DS / 1000), removed);
    }
}

int main(int argc, char** argv)
{
    int n;

    if (argc <= 1) {
        test_i_for();
        malloc_trim(0);
        sleep(1);
        test_i_rand();
        malloc_trim(0);
        sleep(1);
        test_s_rand();
    } else if (1 == (n = atoi(argv[1])))
        test_i_for();
    else if (2 == n)
        test_i_rand();
    else if (3 == n)
        test_s_rand();

    return 0;
}
