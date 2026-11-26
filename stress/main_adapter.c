/*
  Sorting Algorithms Implementations
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

#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <_types.h>
#include <_log.h>
#include <stack/stack.h>
#include <queue/queue.h>

#include "demo_ops.h"
#include "stress_adapter.h"
#include "test_adapter_stack.h"
#include "test_adapter_queue.h"

#define TAG "[stress_adapter]"

#define STRESS_NEW(_ptr, _expr) \
    do { while (NULL == ((_ptr) = (_expr))) ; } while (0)

void* _p_malloc(size_t size)                { int num = rand(); if (123 == num % 4321) { pr_test("Memory [ malloc  ] [ %d ] failed!", num); return NULL; } return malloc(size); }
void* _p_calloc(size_t nmemb, size_t size)  { int num = rand(); if (123 == num % 4321) { pr_test("Memory [ calloc  ] [ %d ] failed!", num); return NULL; } return calloc(nmemb, size); }
void* _p_realloc(void* ptr, size_t size)    { int num = rand(); if (0 == num % 5)      { pr_test("Memory [ realloc ] [ %d ] failed!", num); return NULL; } return realloc(ptr, size); }
void  _p_free(void** ptr)                   { if (NULL != ptr && NULL != *ptr) { free(*ptr); *ptr = NULL; } }

static void string_rand(char* buf, int len)
{
    const char tstr[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \
                         ~!@#$%^&*()_+{}|:<>?~@#￥%……&*（）——+{}|：”《》？杨杰";

    if (len <= 0)
        return ;

    --len;
    for (int i = 0; i < len; ++i)
        buf[i] = tstr[rand() % (int)(sizeof(tstr) - 1)];
    buf[len] = '\0';
}

static class_container_adaptor_ops_t g_adapter_ops = {
    .valid_data = demo_valid_key,
    .__eq       = __demo_cmp_eq,
    .copy_data  = demo_copy_key,
    .free_data  = demo_free_key,
};

typedef struct {
    void* this;
    const class_stress_adapter_stack_interface_t* interface;
    const char* name;
} stress_adapter_stack_t;

typedef struct {
    void* this;
    const class_stress_adapter_queue_interface_t* interface;
    const char* name;
} stress_adapter_queue_t;

typedef struct {
    test_data_t** data;
    ds_size_t size;
    ds_size_t capacity;
    ds_size_t head;
} adapter_model_t;

static long g_adapter_times = 0;
static int g_adapter_op = 0;
static const char* g_adapter_group = "?";

static void adapter_fail(int line)
{
    pr_test("Adapter check failed [ %s ] at line [ %d ], times [ %ld ], op [ %d ]!",
            g_adapter_group, line, g_adapter_times, g_adapter_op);
    abort();
}

static bool model_grow(adapter_model_t* model)
{
    ds_size_t capacity = model->capacity ? model->capacity * 2 : 16;
    test_data_t** data = (test_data_t**)malloc(sizeof(test_data_t*) * (size_t)capacity);

    if (NULL == data)
        return false;

    for (ds_size_t i = 0; i < model->size; ++i)
        data[i] = model->data[(model->head + i) % model->capacity];

    free(model->data);
    model->data = data;
    model->capacity = capacity;
    model->head = 0;
    return true;
}

static test_data_t* model_at(adapter_model_t* model, ds_size_t index)
{
    return model->data[(model->head + index) % model->capacity];
}

static bool model_push(adapter_model_t* model, ds_data_t data)
{
    test_data_t* copy = NULL;

    if (model->size == model->capacity && !model_grow(model))
        return false;
    if (!demo_copy_key((ds_key_t)data, (ds_key_t*)&copy))
        return false;

    model->data[(model->head + model->size) % model->capacity] = copy;
    model->size++;
    return true;
}

static void model_pop_back(adapter_model_t* model)
{
    if (0 == model->size)
        return ;

    model->size--;
    demo_free_key((ds_key_t*)&model->data[(model->head + model->size) % model->capacity]);
    model->data[(model->head + model->size) % model->capacity] = NULL;
    if (0 == model->size)
        model->head = 0;
}

static void model_pop_front(adapter_model_t* model)
{
    if (0 == model->size)
        return ;

    demo_free_key((ds_key_t*)&model->data[model->head]);
    model->data[model->head] = NULL;
    model->head = (model->head + 1) % model->capacity;
    model->size--;
    if (0 == model->size)
        model->head = 0;
}

static void model_clear(adapter_model_t* model)
{
    while (model->size > 0)
        model_pop_back(model);
}

static void model_swap(adapter_model_t* left, adapter_model_t* right)
{
    adapter_model_t tmp = *left;
    *left = *right;
    *right = tmp;
}

static void model_destroy(adapter_model_t* model)
{
    model_clear(model);
    free(model->data);
    model->data = NULL;
    model->capacity = 0;
}

static void stack_verify(stress_adapter_stack_t* stack, adapter_model_t* model)
{
    for (int i = 0; stack[i].this; ++i) {
        void* this = stack[i].this;
        const class_stress_adapter_stack_interface_t* interface = stack[i].interface;

        if (interface->size(this) != model->size) {
            pr_test("Stack [ %s ] size [ %zd ] != model [ %zd ]", stack[i].name, interface->size(this), model->size);
            adapter_fail(__LINE__);
        }
        if (interface->empty(this) != (0 == model->size)) {
            pr_test("Stack [ %s ] empty mismatch", stack[i].name);
            adapter_fail(__LINE__);
        }

        if (model->size > 0) {
            ds_data_t top = interface->top(this, 0);
            if (0 == top || !__demo_cmp_eq(top, (ds_data_t)model_at(model, model->size - 1))) {
                pr_test("Stack [ %s ] top mismatch", stack[i].name);
                adapter_fail(__LINE__);
            }
        }
    }
}

static bool stack_push(stress_adapter_stack_t* stack, adapter_model_t* model, ds_data_t data)
{
    if (!model_push(model, data))
        return false;

    for (int i = 0; stack[i].this; ++i) {
        if (!stack[i].interface->push(stack[i].this, data)) {
            for (int j = 0; j < i; ++j)
                stack[j].interface->pop(stack[j].this);

            model_pop_back(model);
            stack_verify(stack, model);
            return false;
        }
    }

    stack_verify(stack, model);
    return true;
}

static bool stack_pop(stress_adapter_stack_t* stack, adapter_model_t* model)
{
    if (0 == model->size)
        return true;

    stack_verify(stack, model);
    ds_data_t expected = (ds_data_t)model_at(model, model->size - 1);

    for (int i = 0; stack[i].this; ++i) {
        void* this = stack[i].this;
        const class_stress_adapter_stack_interface_t* interface = stack[i].interface;
        ds_data_t top = interface->top(this, 0);

        if (0 == top || !__demo_cmp_eq(top, expected)) {
            pr_test("Stack [ %s ] pop top mismatch", stack[i].name);
            adapter_fail(__LINE__);
        }

        interface->pop(this);
        if (interface->size(this) != model->size - 1) {
            pr_test("Stack [ %s ] pop size mismatch", stack[i].name);
            adapter_fail(__LINE__);
        }
    }

    model_pop_back(model);
    stack_verify(stack, model);
    return true;
}

static void stack_clear(stress_adapter_stack_t* stack, adapter_model_t* model)
{
    ds_size_t size = model->size;

    for (int i = 0; stack[i].this; ++i) {
        void* this = stack[i].this;
        const class_stress_adapter_stack_interface_t* interface = stack[i].interface;

        if (interface->size(this) != size) {
            pr_test("Stack [ %s ] clear size mismatch", stack[i].name);
            adapter_fail(__LINE__);
        }
        if (interface->clear(this) != size || interface->size(this) != 0 || !interface->empty(this)) {
            pr_test("Stack [ %s ] clear result mismatch", stack[i].name);
            adapter_fail(__LINE__);
        }
    }

    model_clear(model);
}

static void stack_drain(stress_adapter_stack_t* stack, adapter_model_t* model)
{
    while (model->size > 0)
        stack_pop(stack, model);

    stack_verify(stack, model);
}

static void stack_swap(stress_adapter_stack_t* left, adapter_model_t* left_model,
                       stress_adapter_stack_t* right, adapter_model_t* right_model)
{
    if (left_model->size > 4096 || right_model->size > 4096)
        return ;

    stack_verify(left, left_model);
    stack_verify(right, right_model);

    for (int i = 0; left[i].this; ++i)
        left[i].interface->swap(left[i].this, right[i].this);
    model_swap(left_model, right_model);

    stack_verify(left, left_model);
    stack_verify(right, right_model);

    for (int i = 0; left[i].this; ++i)
        left[i].interface->swap(left[i].this, right[i].this);
    model_swap(left_model, right_model);

    stack_verify(left, left_model);
    stack_verify(right, right_model);

    stack_drain(left, left_model);
    stack_drain(right, right_model);
    pr_test("Stack swap successfully!");
}

static void stack_stress(long limit)
{
    stack_t* left_vector = NULL;
    stack_t* left_deque = NULL;
    stack_t* left_list = NULL;
    stack_t* right_vector = NULL;
    stack_t* right_deque = NULL;
    stack_t* right_list = NULL;
    STRESS_NEW(left_vector, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, VECTOR));
    STRESS_NEW(left_deque, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, DEQUE));
    STRESS_NEW(left_list, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, LIST));
    STRESS_NEW(right_vector, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, VECTOR));
    STRESS_NEW(right_deque, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, DEQUE));
    STRESS_NEW(right_list, STACK_NEW_OPS_T(&g_adapter_ops, intptr_t, LIST));

    stress_adapter_stack_t left[4] = {
        { .this = left_vector, .interface = cs_adapter_stack, .name = "vector", },
        { .this = left_deque,  .interface = cs_adapter_stack, .name = "deque",  },
        { .this = left_list,   .interface = cs_adapter_stack, .name = "list",   },
        { .this = NULL,        .interface = NULL,             .name = NULL,     },
    };
    stress_adapter_stack_t right[4] = {
        { .this = right_vector, .interface = cs_adapter_stack, .name = "vector", },
        { .this = right_deque,  .interface = cs_adapter_stack, .name = "deque",  },
        { .this = right_list,   .interface = cs_adapter_stack, .name = "list",   },
        { .this = NULL,         .interface = NULL,             .name = NULL,     },
    };

    adapter_model_t left_model = { 0 };
    adapter_model_t right_model = { 0 };
    char buffer[128];

    g_adapter_group = "stack";
    for (long times = 0; times < limit; ++times) {
        g_adapter_times = times;
        if (0 == times % 10000)
            pr_test("Stack times [ %ld ], left [ %zd ], right [ %zd ]", times, left_model.size, right_model.size);

        if (left_model.size + right_model.size > 3000000) {
            pr_test("Stack memory warning, clear!");
            stack_clear(left, &left_model);
            stack_clear(right, &right_model);
        }

        {
            int len = rand() % (sizeof(buffer) + 1);
            if (len < 5) len = 5;
            string_rand(buffer, len);
        }
        test_data_t tmp = { .name = buffer, .id = rand(), };
        ds_data_t data = (ds_data_t)(&tmp);

        int op = rand() % 10000;
        g_adapter_op = op;
        if (op < 3000)
            stack_push(left, &left_model, data);
        else if (op < 6000)
            stack_push(right, &right_model, data);
        else if (op < 7500)
            stack_pop(left, &left_model);
        else if (op < 9000)
            stack_pop(right, &right_model);
        else if (op < 9001)
            stack_clear(left, &left_model);
        else if (op < 9002)
            stack_clear(right, &right_model);
        else if (op < 9003)
            stack_swap(left, &left_model, right, &right_model);
        else {
            stack_verify(left, &left_model);
            stack_verify(right, &right_model);
        }
    }

    STACK_DELETE(&left_vector);
    STACK_DELETE(&left_deque);
    STACK_DELETE(&left_list);
    STACK_DELETE(&right_vector);
    STACK_DELETE(&right_deque);
    STACK_DELETE(&right_list);
    model_destroy(&left_model);
    model_destroy(&right_model);
}

static void queue_verify(stress_adapter_queue_t* queue, adapter_model_t* model)
{
    for (int i = 0; queue[i].this; ++i) {
        void* this = queue[i].this;
        const class_stress_adapter_queue_interface_t* interface = queue[i].interface;

        if (interface->size(this) != model->size) {
            pr_test("Queue [ %s ] size [ %zd ] != model [ %zd ]", queue[i].name, interface->size(this), model->size);
            adapter_fail(__LINE__);
        }
        if (interface->empty(this) != (0 == model->size)) {
            pr_test("Queue [ %s ] empty mismatch", queue[i].name);
            adapter_fail(__LINE__);
        }

        if (model->size > 0) {
            ds_data_t front = interface->front(this, 0);
            ds_data_t back = interface->back(this, 0);

            if (0 == front || !__demo_cmp_eq(front, (ds_data_t)model_at(model, 0))) {
                pr_test("Queue [ %s ] front mismatch", queue[i].name);
                adapter_fail(__LINE__);
            }
            if (0 == back || !__demo_cmp_eq(back, (ds_data_t)model_at(model, model->size - 1))) {
                pr_test("Queue [ %s ] back mismatch", queue[i].name);
                adapter_fail(__LINE__);
            }
        }
    }
}

static void queue_clear(stress_adapter_queue_t* queue, adapter_model_t* model)
{
    ds_size_t size = model->size;

    for (int i = 0; queue[i].this; ++i) {
        void* this = queue[i].this;
        const class_stress_adapter_queue_interface_t* interface = queue[i].interface;

        if (interface->size(this) != size) {
            pr_test("Queue [ %s ] clear size mismatch", queue[i].name);
            adapter_fail(__LINE__);
        }
        if (interface->clear(this) != size || interface->size(this) != 0 || !interface->empty(this)) {
            pr_test("Queue [ %s ] clear result mismatch", queue[i].name);
            adapter_fail(__LINE__);
        }
    }

    model_clear(model);
}

static void queue_force_clear(stress_adapter_queue_t* queue, adapter_model_t* model)
{
    for (int i = 0; queue[i].this; ++i)
        queue[i].interface->clear(queue[i].this);

    model_clear(model);
    queue_verify(queue, model);
}

static bool queue_push(stress_adapter_queue_t* queue, adapter_model_t* model, ds_data_t data)
{
    if (!model_push(model, data))
        return false;

    for (int i = 0; queue[i].this; ++i) {
        if (!queue[i].interface->push(queue[i].this, data)) {
            queue_force_clear(queue, model);
            return false;
        }
    }

    queue_verify(queue, model);
    return true;
}

static bool queue_pop(stress_adapter_queue_t* queue, adapter_model_t* model)
{
    if (0 == model->size)
        return true;

    queue_verify(queue, model);
    ds_data_t expected = (ds_data_t)model_at(model, 0);

    for (int i = 0; queue[i].this; ++i) {
        void* this = queue[i].this;
        const class_stress_adapter_queue_interface_t* interface = queue[i].interface;
        ds_data_t front = interface->front(this, 0);

        if (0 == front || !__demo_cmp_eq(front, expected)) {
            pr_test("Queue [ %s ] pop front mismatch", queue[i].name);
            adapter_fail(__LINE__);
        }

        interface->pop(this);
        if (interface->size(this) != model->size - 1) {
            pr_test("Queue [ %s ] pop size mismatch", queue[i].name);
            adapter_fail(__LINE__);
        }
    }

    model_pop_front(model);
    queue_verify(queue, model);
    return true;
}

static void queue_drain(stress_adapter_queue_t* queue, adapter_model_t* model)
{
    while (model->size > 0)
        queue_pop(queue, model);

    queue_verify(queue, model);
}

static void queue_swap(stress_adapter_queue_t* left, adapter_model_t* left_model,
                       stress_adapter_queue_t* right, adapter_model_t* right_model)
{
    if (left_model->size > 4096 || right_model->size > 4096)
        return ;

    queue_verify(left, left_model);
    queue_verify(right, right_model);

    for (int i = 0; left[i].this; ++i)
        left[i].interface->swap(left[i].this, right[i].this);
    model_swap(left_model, right_model);

    queue_verify(left, left_model);
    queue_verify(right, right_model);

    for (int i = 0; left[i].this; ++i)
        left[i].interface->swap(left[i].this, right[i].this);
    model_swap(left_model, right_model);

    queue_verify(left, left_model);
    queue_verify(right, right_model);

    queue_drain(left, left_model);
    queue_drain(right, right_model);
    pr_test("Queue swap successfully!");
}

static void queue_stress(long limit)
{
    queue_t* left_deque = NULL;
    queue_t* left_list = NULL;
    queue_t* right_deque = NULL;
    queue_t* right_list = NULL;
    STRESS_NEW(left_deque, QUEUE_NEW_OPS_T(&g_adapter_ops, intptr_t, DEQUE));
    STRESS_NEW(left_list, QUEUE_NEW_OPS_T(&g_adapter_ops, intptr_t, LIST));
    STRESS_NEW(right_deque, QUEUE_NEW_OPS_T(&g_adapter_ops, intptr_t, DEQUE));
    STRESS_NEW(right_list, QUEUE_NEW_OPS_T(&g_adapter_ops, intptr_t, LIST));

    stress_adapter_queue_t left[3] = {
        { .this = left_deque, .interface = cs_adapter_queue, .name = "deque", },
        { .this = left_list,  .interface = cs_adapter_queue, .name = "list",  },
        { .this = NULL,       .interface = NULL,              .name = NULL,    },
    };
    stress_adapter_queue_t right[3] = {
        { .this = right_deque, .interface = cs_adapter_queue, .name = "deque", },
        { .this = right_list,  .interface = cs_adapter_queue, .name = "list",  },
        { .this = NULL,        .interface = NULL,              .name = NULL,    },
    };

    adapter_model_t left_model = { 0 };
    adapter_model_t right_model = { 0 };
    char buffer[128];

    g_adapter_group = "queue";
    for (long times = 0; times < limit; ++times) {
        g_adapter_times = times;
        if (0 == times % 10000)
            pr_test("Queue times [ %ld ], left [ %zd ], right [ %zd ]", times, left_model.size, right_model.size);

        if (left_model.size + right_model.size > 3000000) {
            pr_test("Queue memory warning, clear!");
            queue_clear(left, &left_model);
            queue_clear(right, &right_model);
        }

        {
            int len = rand() % (sizeof(buffer) + 1);
            if (len < 5) len = 5;
            string_rand(buffer, len);
        }
        test_data_t tmp = { .name = buffer, .id = rand(), };
        ds_data_t data = (ds_data_t)(&tmp);

        int op = rand() % 10000;
        g_adapter_op = op;
        if (op < 3000)
            queue_push(left, &left_model, data);
        else if (op < 6000)
            queue_push(right, &right_model, data);
        else if (op < 7500)
            queue_pop(left, &left_model);
        else if (op < 9000)
            queue_pop(right, &right_model);
        else if (op < 9001)
            queue_clear(left, &left_model);
        else if (op < 9002)
            queue_clear(right, &right_model);
        else if (op < 9003)
            queue_swap(left, &left_model, right, &right_model);
        else {
            queue_verify(left, &left_model);
            queue_verify(right, &right_model);
        }
    }

    QUEUE_DELETE(&left_deque);
    QUEUE_DELETE(&left_list);
    QUEUE_DELETE(&right_deque);
    QUEUE_DELETE(&right_list);
    model_destroy(&left_model);
    model_destroy(&right_model);
}

int main(int argc, char** argv)
{
    srand(time(0));
    long limit = argc > 1 ? atoi(argv[1]) : 1000000;

    stack_stress(limit);
    queue_stress(limit);

    return 0;
}
