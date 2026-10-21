/*
  Queue correctness test across deque/list bases.
*/

#include <queue/queue.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
        failures++; \
    } \
} while (0)

#define TEST_INT_BASE(_base) do { \
    enum { N = 1000 }; \
    queue_t* q = QUEUE_NEW_T(queue_data_t, _base); \
    int i; \
    CHECK(NULL != q, "queue constructor " #_base); \
    if (NULL == q) break; \
    for (i = 0; i < N; ++i) \
        CHECK(cqueue_push(q, (queue_data_t)i), "push " #_base); \
    CHECK(cqueue_size(q) == N, "size " #_base); \
    CHECK(!__cqueue_empty(q), "not empty " #_base); \
    for (i = 0; i < N; ++i) { \
        queue_value_t front = cqueue_front(q); \
        CHECK(NULL != front.u8, "front " #_base); \
        CHECK(NULL != front.u8 && *(queue_data_t*)front.d == i, "FIFO " #_base); \
        cqueue_pop(q); \
    } \
    CHECK(__cqueue_empty(q), "empty " #_base); \
    CHECK(cqueue_size(q) == 0, "size zero " #_base); \
    for (i = 0; i < N; ++i) \
        CHECK(cqueue_push(q, (queue_data_t)i), "push again " #_base); \
    CHECK(cqueue_clear(q) == N, "clear " #_base); \
    CHECK(__cqueue_empty(q), "empty after clear " #_base); \
    QUEUE_DELETE(&q); \
} while (0)

static void test_int_bases(void)
{
    TEST_INT_BASE(DEQUE);
    TEST_INT_BASE(LIST);
}

static void test_default_token(void)
{
    enum { N = 100 };
    queue_t* q = QUEUE_NEW();
    int i;

    CHECK(NULL != q, "default deque constructor");
    if (NULL == q)
        return;

    for (i = 0; i < N; ++i)
        CHECK(cqueue_push(q, (queue_data_t)i), "default push");

    CHECK(cqueue_size(q) == N, "default size");
    CHECK(*(queue_data_t*)cqueue_front(q).d == 0, "default front");
    CHECK(*(queue_data_t*)cqueue_back(q).d == N - 1, "default back");

    CHECK(cqueue_clear(q) == N, "default clear");
    CHECK(__cqueue_empty(q), "default empty");
    QUEUE_DELETE(&q);
}

static void test_sso(void)
{
    queue_t* q = QUEUE_NEW_SSO();
    queue_value_t front;

    CHECK(NULL != q, "sso constructor");
    if (NULL == q)
        return;

    CHECK(cqueue_push(q, (queue_data_t)"hello"), "sso push hello");
    CHECK(cqueue_push(q, (queue_data_t)"world"), "sso push world");

    front = cqueue_front(q);
    CHECK(NULL != front.ssd, "sso front ptr");
    CHECK(front.ssd && front.ssd->len == 5 && 0 == memcmp(front.ssd->p, "hello", 5), "sso front value");

    cqueue_pop(q);
    front = cqueue_front(q);
    CHECK(front.ssd && front.ssd->len == 5 && 0 == memcmp(front.ssd->p, "world", 5), "sso after pop");

    QUEUE_DELETE(&q);
}

static void test_string_ops(void)
{
    queue_t* q = QUEUE_NEW_CHAR();
    queue_value_t front;

    CHECK(NULL != q, "string ops constructor");
    if (NULL == q)
        return;

    CHECK(cqueue_push(q, (queue_data_t)"alpha"), "string ops push");
    front = cqueue_front(q);
    CHECK(NULL != front.sd, "string ops front ptr");
    CHECK(front.sd && 0 == strcmp(*front.sd, "alpha"), "string ops front value");

    QUEUE_DELETE(&q);
}

static void test_swap(void)
{
    queue_t* a = QUEUE_NEW_T(queue_data_t, DEQUE);
    queue_t* b = QUEUE_NEW_T(queue_data_t, DEQUE);

    CHECK(NULL != a && NULL != b, "swap constructors");
    if (!a || !b)
        goto out;

    CHECK(cqueue_push(a, (queue_data_t)1), "swap push a");
    CHECK(cqueue_push(b, (queue_data_t)2), "swap push b");
    cqueue_swap(a, b);
    CHECK(*(queue_data_t*)cqueue_front(a).d == 2, "swap front a");
    CHECK(*(queue_data_t*)cqueue_front(b).d == 1, "swap front b");

out:
    if (a) QUEUE_DELETE(&a);
    if (b) QUEUE_DELETE(&b);
}

static void test_null(void)
{
    CHECK(cqueue_size(NULL) == -1, "null size");
    CHECK(!cqueue_push(NULL, 0), "null push");
    CHECK(NULL == cqueue_front(NULL).u8, "null front");
    CHECK(NULL == cqueue_back(NULL).u8, "null back");
    cqueue_pop(NULL);
    CHECK(cqueue_clear(NULL) == -1, "null clear");
}

int main(void)
{
    test_int_bases();
    test_default_token();
    test_sso();
    test_string_ops();
    test_swap();
    test_null();

    if (failures != 0) {
        printf("%d failure(s)\n", failures);
        return 1;
    }

    printf("all queue tests passed\n");
    return 0;
}
