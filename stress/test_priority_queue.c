/*
  Priority queue correctness / swap / string ops test.
*/

#include <priority_queue/priority_queue.h>
#include <stdio.h>
#include <string.h>

static int failures;
#define CHECK(c,m) do { if(!(c)){printf("FAIL: %s (%d)\n",m,__LINE__);failures++;} } while(0)

static void test_base(int deque_base)
{
    priority_queue_t* pq = deque_base
        ? PRIORITY_QUEUE_NEW_T(priority_queue_data_t, DEQUE)
        : PRIORITY_QUEUE_NEW();
    priority_queue_data_t vals[] = { 5, 3, 8, 1, 9, 2 };
    int i;
    CHECK(pq != NULL, "new");
    if (!pq) return;
    for (i = 0; i < (int)(sizeof(vals)/sizeof(vals[0])); ++i)
        CHECK(cpqueue_push(pq, vals[i]), "push");
    CHECK(cpqueue_size(pq) == 6, "size");
    CHECK(*(priority_queue_data_t*)cpqueue_top(pq).d == 9, "top 9");
    cpqueue_pop(pq);
    CHECK(*(priority_queue_data_t*)cpqueue_top(pq).d == 8, "top 8");
    cpqueue_pop(pq);
    CHECK(*(priority_queue_data_t*)cpqueue_top(pq).d == 5, "top 5");
    CHECK(cpqueue_clear(pq) == 4, "clear");
    CHECK(__cpqueue_empty(pq), "empty");
    PRIORITY_QUEUE_DELETE(&pq);
}

static void test_string(void)
{
    priority_queue_t* pq = PRIORITY_QUEUE_NEW_CHAR();
    priority_queue_value_t v;
    CHECK(pq != NULL, "string new");
    if (!pq) return;
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"apple"), "push apple");
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"cherry"), "push cherry");
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"banana"), "push banana");
    v = cpqueue_top(pq);
    CHECK(v.sd && 0 == strcmp(*v.sd, "cherry"), "string top cherry");
    cpqueue_pop(pq);
    v = cpqueue_top(pq);
    CHECK(v.sd && 0 == strcmp(*v.sd, "banana"), "string top banana");
    cpqueue_pop(pq);
    v = cpqueue_top(pq);
    CHECK(v.sd && 0 == strcmp(*v.sd, "apple"), "string top apple");
    PRIORITY_QUEUE_DELETE(&pq);
}

static void check_sso_top(priority_queue_t* pq, const char* expect)
{
    priority_queue_value_t v = cpqueue_top(pq);
    size_t len = strlen(expect);

    CHECK(v.ssd && v.ssd->len == len && 0 == memcmp(v.ssd->p, expect, len), expect);
}

static void test_string_sso(int deque_base)
{
    static const char* order[] = { "watermelon-long-string", "cherry", "banana", "apple" };
    priority_queue_t* pq = deque_base
        ? PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_sso(), ds_sso_t, DEQUE)
        : PRIORITY_QUEUE_NEW_SSO();
    int i;

    CHECK(pq != NULL, "sso new");
    if (!pq) return;

    /* Short strings stay inline; the long one spills to the heap. */
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"apple"), "sso push apple");
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"cherry"), "sso push cherry");
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"banana"), "sso push banana");
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"watermelon-long-string"), "sso push long");
    CHECK(cpqueue_size(pq) == 4, "sso size");

    for (i = 0; i < 4; ++i) {
        check_sso_top(pq, order[i]);
        cpqueue_pop(pq);
    }
    CHECK(__cpqueue_empty(pq), "sso empty");

    /* Reuse after clear, then a swap between two sso queues. */
    CHECK(cpqueue_push(pq, (priority_queue_data_t)"one"), "sso push one");
    CHECK(cpqueue_clear(pq) == 1, "sso clear");
    CHECK(__cpqueue_empty(pq), "sso empty after clear");

    /*
      Bulk test: enough pushes to force base growth (vector realloc /
      deque block growth), including strings past the inline capacity.
      Every pop must yield a non-increasing sequence.
    */
    {
        char buf[64];
        char prev[64] = "";
        int n = 1000;

        for (i = 0; i < n; ++i) {
            snprintf(buf, sizeof(buf), "key-%06d-%s", (i * 37) % n, (i % 3) ? "" : "long-suffix");
            CHECK(cpqueue_push(pq, (priority_queue_data_t)buf), "sso bulk push");
        }
        for (i = 0; i < n; ++i) {
            priority_queue_value_t v = cpqueue_top(pq);
            size_t len;

            CHECK(v.ssd != NULL, "sso bulk top");
            if (!v.ssd)
                break;
            len = v.ssd->len;
            CHECK(len < sizeof(buf), "sso bulk len");
            memcpy(buf, v.ssd->p, len);
            buf[len] = '\0';
            if (i > 0)
                CHECK(strcmp(prev, buf) >= 0, "sso bulk order");
            memcpy(prev, buf, len + 1);
            cpqueue_pop(pq);
        }
        CHECK(i == n, "sso bulk count");
        CHECK(__cpqueue_empty(pq), "sso bulk empty");
    }

    {
        priority_queue_t* other = deque_base
            ? PRIORITY_QUEUE_NEW_OPS_T(g_class_priority_queue_ops_sso(), ds_sso_t, DEQUE)
            : PRIORITY_QUEUE_NEW_SSO();
        CHECK(other != NULL, "sso other new");
        if (other) {
            CHECK(cpqueue_push(pq, (priority_queue_data_t)"aaa"), "sso push aaa");
            CHECK(cpqueue_push(other, (priority_queue_data_t)"zzz"), "sso push zzz");
            cpqueue_swap(pq, other);
            check_sso_top(pq, "zzz");
            check_sso_top(other, "aaa");
            PRIORITY_QUEUE_DELETE(&other);
        }
    }

    PRIORITY_QUEUE_DELETE(&pq);
}

static void test_swap(void)
{
    priority_queue_t* a = PRIORITY_QUEUE_NEW();
    priority_queue_t* b = PRIORITY_QUEUE_NEW();
    CHECK(a && b, "swap new");
    if (!a || !b) return;
    cpqueue_push(a, 1);
    cpqueue_push(b, 2);
    cpqueue_swap(a, b);
    CHECK(*(priority_queue_data_t*)cpqueue_top(a).d == 2, "swap a");
    CHECK(*(priority_queue_data_t*)cpqueue_top(b).d == 1, "swap b");
    PRIORITY_QUEUE_DELETE(&a);
    PRIORITY_QUEUE_DELETE(&b);
}

static void test_null(void)
{
    CHECK(cpqueue_size(NULL) == -1, "null size");
    CHECK(!cpqueue_push(NULL, 0), "null push");
    CHECK(NULL == cpqueue_top(NULL).u8, "null top");
    cpqueue_pop(NULL);
    CHECK(cpqueue_clear(NULL) == -1, "null clear");
}

int main(void)
{
    test_base(0);
    test_base(1);
    test_string();
    test_string_sso(0);
    test_string_sso(1);
    test_swap();
    test_null();
    if (failures) { printf("%d failures\n", failures); return 1; }
    printf("pq tests passed\n");
    return 0;
}
