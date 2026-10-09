/*
  List Demos
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

#include <list/list.h>
#include <_log.h>
#include <operations/ds_ops_string.h>

#define cds clist
#define TAG "[demo_list]"

#define _to(x)   ((list_data_t)(x))

/* sort 的第三分支：用户自己给的比较器（这里就是升序，等价于 LIST_SORT_ASC） */
static bool demo_int_lt(list_data_t left, list_data_t right)
{
    return left < right;
}

/* SSO 链的比较器：槽里是 ds_sso_t。cmp 传 NULL/1/2 是按 list_data_t 数值排，
   对字符串链没意义，所以这里自己给一个。 */
static bool demo_sso_lt(list_data_t left, list_data_t right)
{
    return 0 > ds_sso_cmp((const ds_sso_t*)left, (const ds_sso_t*)right);
}

#define foreach()          { for (list_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(demo, it))  pr_test("%zd", it_data_safe(it)); }
#define foreach_string()   { for (list_iterator_t it = cds->begin(demo);    it_ne(cds->end(demo), it);  it = cds->next(demo, it))  pr_test("%s", it_sdata_safe(it)); }
#define foreach_r_string() { for (list_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(demo, it)) pr_test("%s", it_sdata_safe(it)); }

static void demo_base_and_iterator(void)
{
    list_t* demo = LIST_NEW();

    for (int i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    cds->push_back(demo, 8); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ], size = 9
    cds->size(demo);         // size = 9
    cds->count(demo, 8);     // count(8) = 2
    cds->first(demo, -1);    // return 1
    cds->last(demo, -1);     // return 8

    foreach(); // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
    pr_test("");

    {
        for (list_iterator_t   it = cds->end(demo);    it_ne(cds->begin(demo), it); )  { it = cds->prev(demo, it);  pr_test("%zd", it_data_safe(it)); }  // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (list_r_iterator_t it = cds->rbegin(demo); it_ne(cds->rend(demo), it); it = cds->rnext(demo, it))       pr_test("%zd", it_data_safe(it));  // [ 8, 8, 7, 6, 5, 4, 3, 2, 1 ]
        pr_test("");
        for (list_r_iterator_t it = cds->rend(demo);   it_ne(cds->rbegin(demo), it); ) { it = cds->rprev(demo, it); pr_test("%zd", it_data_safe(it)); }  // [ 1, 2, 3, 4, 5, 6, 7, 8, 8 ]
        pr_test("");
    }

    LIST_DELETE(&demo);
}

static void demo_about_insert(void)
{
    list_t* demo = LIST_NEW();

    cds->push_back(demo, 5);  // [ 5 ]
    cds->push_back(demo, 6);  // [ 5, 6 ]
    cds->push_back(demo, 7);  // [ 5, 6, 7 ]

    cds->push_front(demo, 0); // [ 0, 5, 6, 7 ]
    cds->push_front(demo, 1); // [ 1, 0, 5, 6, 7 ]

    cds->insert(demo, cds->begin(demo), 66); // [ 66, 1, 0, 5, 6, 7 ]
    cds->insert(demo, cds->end(demo), 66);   // [ 66, 1, 0, 5, 6, 7, 66 ]

    LIST_DELETE(&demo);
}

static bool demo_remove_if_condition(ds_data_t data)
{
    return data >= 5;
}

static void demo_about_erase(void)
{
    list_t* demo = LIST_NEW();
    list_iterator_t it;
    list_size_t ret;

    (void)it;
    (void)ret;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->erase_range(demo, cds->prev(demo, cds->end(demo)), cds->end(demo)); // [ 1, 2, 3, 4, 5, 6, 7 ], it -> end()

    it = cds->erase(demo, cds->begin(demo)); // [ 2, 3, 4, 5, 6, 7 ], it -> 2

    cds->pop_front(demo); // [ 3, 4, 5, 6, 7 ]
    cds->pop_back(demo);  // [ 3, 4, 5, 6 ]

    ret = cds->remove(demo, 66); // [ 3, 4, 5, 6 ], return 0(0 elements has been removed)

    ret = cds->remove_if(demo, demo_remove_if_condition); // [ 3, 4 ], return 2(2 elements has been removed)

    ret = cds->clear(demo); // [ ], return 2(2 elements has been removed)

    LIST_DELETE(&demo);
}

static void demo_about_find(void)
{
    list_t* demo = LIST_NEW();
    list_iterator_t it;

    (void)it;

    for (int i = 1; i <= 8; ++i)
        cds->push_back(demo, i);
    // after for [ 1, 2, 3, 4, 5, 6, 7, 8 ]

    it = cds->find(demo, 3);  // found, it -> 3
    it = cds->find(demo, 66); // no found, it -> end()

    LIST_DELETE(&demo);
}

/* 这一组是本次重写新补上的，思路照 bits/stl_list.h */
static void demo_about_algorithms(void)
{
    list_t* demo  = LIST_NEW();
    list_t* other = LIST_NEW();
    int     a[] = { 5, 1, 4, 2, 8, 8, 3, 7, 1 };
    int     b[] = { 6, 0, 9 };

    for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); ++i)
        cds->push_back(demo, a[i]);
    foreach(); // [ 5, 1, 4, 2, 8, 8, 3, 7, 1 ]
    pr_test("");

    cds->sort(demo, LIST_SORT_ASC);  // 升序：carry + tmp[64] 的 64 路归并，元素一个都不搬（传 NULL 等价）
    foreach(); // [ 1, 1, 2, 3, 4, 5, 7, 8, 8 ]
    pr_test("");

    cds->sort(demo, LIST_SORT_DESC); // 降序：魔术数字 2
    foreach(); // [ 8, 8, 7, 5, 4, 3, 2, 1, 1 ]
    pr_test("");

    cds->sort(demo, demo_int_lt);    // 第三分支：直接给比较器，效果同升序
    foreach(); // [ 1, 1, 2, 3, 4, 5, 7, 8, 8 ]
    pr_test("");

    cds->unique(demo); // 去掉连续重复
    foreach(); // [ 1, 2, 3, 4, 5, 7, 8 ]
    pr_test("");

    cds->reverse(demo); // _M_reverse：就地翻转 next/prev
    foreach(); // [ 8, 7, 5, 4, 3, 2, 1 ]
    pr_test("");
    cds->reverse(demo);

    for (size_t i = 0; i < sizeof(b) / sizeof(b[0]); ++i)
        cds->push_back(other, b[i]);
    cds->sort(other, LIST_SORT_ASC); // [ 0, 6, 9 ]
    cds->merge(demo, other);   // 两条有序链归并，other 被搬空
    foreach(); // [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ]
    pr_test("");

    cds->resize(demo, 5, -1);  // 截短
    foreach(); // [ 0, 1, 2, 3, 4 ]
    pr_test("");
    cds->resize(demo, 7, -1);  // 补默认值
    foreach(); // [ 0, 1, 2, 3, 4, -1, -1 ]
    pr_test("");

    cds->assign(demo, 3, 42);  // 清空再填 3 份
    foreach(); // [ 42, 42, 42 ]
    pr_test("");

    cds->clear(other);
    cds->push_back(other, 100);
    cds->push_back(other, 200);
    cds->splice_range(demo, cds->begin(demo), other,
                      cds->begin(other), cds->next(other, cds->begin(other))); // 搬 other 的首元素过来
    foreach(); // [ 100, 42, 42, 42 ]
    pr_test("");

    cds->splice(demo, cds->end(demo), other); // 剩下的整条搬到尾部
    foreach(); // [ 100, 42, 42, 42, 200 ]
    pr_test("");

    cds->swap(demo, other); // 换的是两条链的头，元素一个都不动
    foreach(); // [ ]
    pr_test("");
    cds->swap(demo, other);

    LIST_DELETE(&demo);
    LIST_DELETE(&other);
}

static void demo_string(void)
{
    list_t* demo = LIST_NEW_STRING();

    cds->push_back(demo, _to("j"));
    cds->push_back(demo, _to("and"));
    cds->push_back(demo, _to("jerry")); // [ 'j', 'and', 'jerry' ]

    foreach_string();
    pr_test("");

    foreach_r_string();
    pr_test("");

    LIST_DELETE(&demo);
}

/* SSO：<= DS_SSO_LOCAL_CAP(15) 字节的串直接内联在节点里，不再单独 malloc 一块串体；
   ds_sso_t 的 p 在 offset 0，所以 it_sdata(it) / it_data(it) 的用法都不变。 */
static void demo_about_sso(void)
{
    list_t* demo = LIST_NEW_SSO();

    cds->push_back(demo, _to("j"));
    cds->push_back(demo, _to("sso-inline"));               // 10 字节：内联
    cds->push_back(demo, _to("this one is over 15 bytes")); // 25 字节：超了才单独 malloc

    for (list_iterator_t it = cds->begin(demo); it_ne(cds->end(demo), it); it = cds->next(demo, it)) {
        ds_sso_t* s = (ds_sso_t*)it.d;
        pr_test("%-24s len=%-3zu %s", s->p, s->len, s->p == s->buf ? "local (no extra malloc)" : "heap");
    }

    cds->sort(demo, demo_sso_lt); // 大小串混排；SSO 的比较器得自己给
    foreach_string();
    pr_test("");

    LIST_DELETE(&demo);
}

int main(void)
{
    /* values */
    demo_base_and_iterator();
    demo_about_insert();
    demo_about_erase();
    demo_about_find();
    demo_about_algorithms();

    /* strings */
    demo_string();
    demo_about_sso();
    return 0;
}
