/*
  Stack correctness test across vector/deque/list bases.
*/

#include <stack/stack.h>
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
    stack_t* s = STACK_NEW_T(int, _base); \
    int i; \
    CHECK(NULL != s, "stack constructor " #_base); \
    if (NULL == s) break; \
    for (i = 0; i < N; ++i) \
        CHECK(cstack_push(s, (ds_data_t)i), "push " #_base); \
    CHECK(cstack_size(s) == N, "size " #_base); \
    CHECK(!__cstack_empty(s), "not empty " #_base); \
    for (i = N - 1; i >= 0; --i) { \
        stack_value_t top = cstack_top(s); \
        CHECK(NULL != top.u8, "top " #_base); \
        CHECK(NULL != top.u8 && *(int*)top.u8 == i, "LIFO " #_base); \
        cstack_pop(s); \
    } \
    CHECK(__cstack_empty(s), "empty " #_base); \
    CHECK(cstack_size(s) == 0, "size zero " #_base); \
    STACK_DELETE(&s); \
} while (0)

static void test_int_bases(void)
{
    TEST_INT_BASE(VECTOR);
    TEST_INT_BASE(DEQUE);
    TEST_INT_BASE(LIST);
}

static void test_default_token(void)
{
    enum { N = 100 };
    stack_t* s = STACK_NEW();
    int i;

    CHECK(NULL != s, "default constructor");
    if (NULL == s)
        return;

    for (i = 0; i < N; ++i)
        CHECK(cstack_push(s, (ds_data_t)i), "default push");

    CHECK(cstack_size(s) == N, "default size");
    CHECK(*(ds_data_t*)cstack_top(s).u8 == N - 1, "default top");

    CHECK(cstack_clear(s) == N, "default clear");
    CHECK(__cstack_empty(s), "default empty");
    STACK_DELETE(&s);
}

static void test_sso(void)
{
    stack_t* s = STACK_NEW_OPS_T(g_class_stack_ops_sso(), ds_sso_t, LIST);
    stack_value_t top;

    CHECK(NULL != s, "sso constructor");
    if (NULL == s)
        return;

    CHECK(cstack_push(s, (ds_data_t)"hello"), "sso push hello");
    CHECK(cstack_push(s, (ds_data_t)"world"), "sso push world");

    top = cstack_top(s);
    CHECK(NULL != top.ssd, "sso top ptr");
    CHECK(top.ssd && top.ssd->len == 5 && 0 == memcmp(top.ssd->p, "world", 5), "sso top value");

    cstack_pop(s);
    top = cstack_top(s);
    CHECK(top.ssd && top.ssd->len == 5 && 0 == memcmp(top.ssd->p, "hello", 5), "sso after pop");

    STACK_DELETE(&s);
}

static void test_string_ops(void)
{
    stack_t* s = STACK_NEW_OPS_T(g_class_deque_ops_string(), stack_data_t, DEQUE);
    stack_value_t top;

    CHECK(NULL != s, "string ops constructor");
    if (NULL == s)
        return;

    CHECK(cstack_push(s, (ds_data_t)"alpha"), "string ops push");
    top = cstack_top(s);
    CHECK(NULL != top.sd, "string ops top ptr");
    CHECK(top.sd && 0 == strcmp(*top.sd, "alpha"), "string ops top value");

    STACK_DELETE(&s);
}

static void test_ops_t_bases(void)
{
    enum { N = 100 };
    stack_t* s;
    int i;

    s = STACK_NEW_OPS_T(NULL, int, DEQUE);
    CHECK(NULL != s, "ops_t deque constructor");
    if (s) {
        for (i = 0; i < N; ++i)
            CHECK(cstack_push(s, (ds_data_t)i), "ops_t deque push");
        CHECK(*(int*)cstack_top(s).u8 == N - 1, "ops_t deque top");
        STACK_DELETE(&s);
    }

    s = STACK_NEW_OPS_T(NULL, int, LIST);
    CHECK(NULL != s, "ops_t list constructor");
    if (s) {
        for (i = 0; i < N; ++i)
            CHECK(cstack_push(s, (ds_data_t)i), "ops_t list push");
        CHECK(*(int*)cstack_top(s).u8 == N - 1, "ops_t list top");
        STACK_DELETE(&s);
    }

    s = STACK_NEW_OPS_T(g_class_deque_ops_string(), char*, DEQUE);
    CHECK(NULL != s, "ops_t string constructor");
    if (s) {
        stack_value_t top;
        CHECK(cstack_push(s, (ds_data_t)"beta"), "ops_t string push");
        top = cstack_top(s);
        CHECK(top.sd && 0 == strcmp(*top.sd, "beta"), "ops_t string top");
        STACK_DELETE(&s);
    }
}

static void test_vector_base(void)
{
    enum { N = 1000 };
    stack_t* s = STACK_NEW_OPS_T(NULL, int, VECTOR);
    int i;

    CHECK(NULL != s, "vector base constructor");
    if (NULL == s)
        return;

    for (i = 0; i < N; ++i)
        CHECK(cstack_push(s, (ds_data_t)i), "vector base push");

    CHECK(cstack_size(s) == N, "vector base size");
    CHECK(*(int*)cstack_top(s).u8 == N - 1, "vector base top");

    STACK_DELETE(&s);
}

static void test_null(void)
{
    CHECK(cstack_size(NULL) == -1, "null size");
    CHECK(!cstack_push(NULL, 0), "null push");
    CHECK(NULL == cstack_top(NULL).u8, "null top");
    cstack_pop(NULL);
}

int main(void)
{
    test_int_bases();
    test_default_token();
    test_sso();
    test_string_ops();
    test_ops_t_bases();
    test_vector_base();
    test_null();

    if (failures != 0) {
        printf("%d failure(s)\n", failures);
        return 1;
    }

    printf("all stack tests passed\n");
    return 0;
}
