/*
  临时对照件：16 字节的 SSO 字符串，用来量 deque3 "任意字节数内联存储" 带来的收益。

  布局（16 字节）：

      char    buf[12];   内联内容（含结尾 '\0'）；堆串时前 8 字节放指针
      int32_t len;       带符号长度：>= 0 表示内容就在 buf 里；< 0 表示在堆上，长度是 -len

  几个刻意的选择：

  * 用长度符号位当"内联/堆"的判别位，不用单独的标志字段，也不像 libstdc++ 那样
    让指针指向自己的 buf —— 那样会有自引用，deque3 的区间平移是 memmove 整块搬字节，
    自引用字段搬完就指旧位置了（std::deque<string> 能这么干是靠移动构造修指针，
    C 里没这条件）。这里结构整体可搬运，正好符合 deque3 对元素的前提。

  * buf 放在结构体开头而不是末尾：元素地址是 node 基址 + 16 的倍数（node 来自 malloc，
    16 字节对齐），所以 buf 落在 8 字节对齐的地址上，往里塞 8 字节指针才是对齐的。
    写成 { int32_t len; char buf[12]; } 的话 buf 在偏移 4，指针就只剩 4 字节对齐了。

  * 堆指针的取/放走 memcpy（sso_str_heap_ptr / sso_str_set_heap_ptr），而不是
    从 char 数组里做指针类型双关。长度是编译期常量 8，-O2 下 GCC 直接内联成一条
    mov，和"具名指针成员"生成的机器码完全一致，所以这里不欠性能：

        memcpy(&p, s->buf, 8)  ->  movq (%rdi), %rax
        s->p                   ->  movq (%rdi), %rax

  * 内联容量 11 个字符（12 字节 buf 减掉结尾 '\0'）。超过就走堆。

  这是基准用的临时代码，不进 include/。
*/

#ifndef __J_TEST_SSO_STR_H
#define __J_TEST_SSO_STR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <deque/deque_ops.h>

#define SSO_STR_LOCAL_CAP 12    /* buf 的字节数，含结尾 '\0'：最多 11 个字符 */

typedef struct sso_str {
    char    buf[SSO_STR_LOCAL_CAP];
    int32_t len;                /* >= 0 内联；< 0 在堆上，长度 = -len */
} sso_str_t;

_Static_assert(sizeof(sso_str_t) == 16, "sso_str_t 应当是 16 字节");

/* 堆指针的存取：常量长度 8 的 memcpy，-O2 下就是一条 mov，见文件头注释 */
static inline char* sso_str_heap_ptr(const sso_str_t* s)
{
    char* p;

    memcpy(&p, s->buf, sizeof(p));
    return p;
}

static inline void sso_str_set_heap_ptr(sso_str_t* s, char* p)
{
    memcpy(s->buf, &p, sizeof(p));
}

static inline bool   sso_str_is_local(const sso_str_t* s) { return s->len >= 0; }
static inline size_t sso_str_size(const sso_str_t* s)
{
    return s->len >= 0 ? (size_t)s->len : (size_t)(-(int64_t)s->len);
}
static inline const char* sso_str_c_str(const sso_str_t* s)
{
    return s->len >= 0 ? s->buf : sso_str_heap_ptr(s);
}

static inline void sso_str_init(sso_str_t* s)
{
    s->len = 0;
    s->buf[0] = '\0';
}

/* 释放堆上那份。槽可能是全 0 的空洞：len 为 0 时什么都不做 */
static inline void sso_str_free(sso_str_t* s)
{
    if (s->len < 0) {
        char* p = sso_str_heap_ptr(s);

        if (NULL != p)
            free(p);
    }
    sso_str_init(s);
}

/* 深拷贝一份 src 进 s；s 里原有的内容由调用方负责（与 deque3 的 copy_data 契约一致） */
static inline bool sso_str_set(sso_str_t* s, const char* src)
{
    size_t len = strlen(src);

    if (len < SSO_STR_LOCAL_CAP) {              /* 内联放得下：一次分配都不用 */
        if (s->len < 0) {
            char* old = sso_str_heap_ptr(s);

            if (NULL != old)
                free(old);
        }
        memcpy(s->buf, src, len + 1);
        s->len = (int32_t)len;
        return true;
    }

    if (len > (size_t)INT32_MAX)                /* len 要能被 int32 表示 */
        return false;

    {
        char* p = (char*)malloc(len + 1);

        if (NULL == p)
            return false;
        memcpy(p, src, len + 1);
        if (s->len < 0) {
            char* old = sso_str_heap_ptr(s);

            if (NULL != old)
                free(old);
        }
        sso_str_set_heap_ptr(s, p);
        s->len = -(int32_t)len;
    }
    return true;
}

static inline bool sso_str_eq(const sso_str_t* a, const sso_str_t* b)
{
    size_t la = sso_str_size(a);

    return la == sso_str_size(b) && 0 == memcmp(sso_str_c_str(a), sso_str_c_str(b), la);
}

/* 字典序比较，排序要用 */
static inline int sso_str_cmp(const sso_str_t* a, const sso_str_t* b)
{
    size_t la = sso_str_size(a);
    size_t lb = sso_str_size(b);
    size_t m  = la < lb ? la : lb;
    int    r  = memcmp(sso_str_c_str(a), sso_str_c_str(b), m);

    if (0 != r)
        return r;
    return la < lb ? -1 : (la > lb ? 1 : 0);
}

/* ---------- class_deque_ops_t 回调 ----------
   step = sizeof(sso_str_t) = 16 > sizeof(deque_data_t)，所以信封里放的是元素地址，
   回调拿到的都是 sso_str_t*。 */

static inline const sso_str_t* sso_arg(deque_data_t data)
{
    return (const sso_str_t*)(uintptr_t)data;
}

static inline bool sso_ops_valid_data(deque_data_t data)
{
    const sso_str_t* s = sso_arg(data);

    return NULL != s && s->len != 0;
}

static inline bool sso_ops_eq(deque_data_t left, deque_data_t right)
{
    return sso_str_eq(sso_arg(left), sso_arg(right));
}

static inline bool sso_ops_copy_data(deque_data_t in, deque_data_t* out)
{
    /* out 就是 node 里那 16 字节的槽。内联串整块搬（结构没有自引用），
       堆串再把堆上那份复制一份。 */
    const sso_str_t* src = sso_arg(in);
    sso_str_t*       dst = (sso_str_t*)out;

    if (src->len >= 0) {
        memcpy(dst, src, sizeof(sso_str_t));
        return true;
    }

    {
        size_t len = (size_t)(-(int64_t)src->len);
        char*  p   = (char*)malloc(len + 1);

        if (NULL == p)
            return false;
        memcpy(p, sso_str_c_str(src), len + 1);
        sso_str_set_heap_ptr(dst, p);
        dst->len = src->len;
    }
    return true;
}

static inline void sso_ops_free_data(deque_data_t* data)
{
    sso_str_free((sso_str_t*)data);
}

static inline const class_deque_ops_t* sso_ops_ins(void)
{
    static const class_deque_ops_t ins = {
        .valid_data = sso_ops_valid_data,
        .__eq       = sso_ops_eq,
        .copy_data  = sso_ops_copy_data,
        .free_data  = sso_ops_free_data,
    };
    return &ins;
}

#define g_class_deque_ops_sso()  sso_ops_ins()

#endif /* __J_TEST_SSO_STR_H */
