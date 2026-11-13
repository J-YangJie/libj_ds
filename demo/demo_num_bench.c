/*
  Numeric Data Benchmark
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

#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map/map.h>
#include <vector/vector.h>

/*
 * Same source, 32-bit and 64-bit:
 *   map<uint64_t, double> pays two malloc boxes per node on 32-bit and
 *   none on 64-bit. map<uint32_t, uint32_t> is the platform control.
 * Run both builds and compare; absolute numbers depend on the machine.
 */

static volatile uint64_t sink;

static double now_sec(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void bench_map_u64_double(size_t n)
{
    uint64_t* keys = malloc(n * sizeof(*keys));
    map_t* m = MAP_NEW_KV(uint64_t, double);
    double t0, t1;
    size_t i;

    for (i = 0; i < n; ++i)
        keys[i] = (uint64_t)i * UINT64_C(0x9E3779B97F4A7C15);

    t0 = now_sec();
    for (i = 0; i < n; ++i)
        cmap->insert(m, ds_u64key(keys[i]), ds_lfvalue((double)i));
    t1 = now_sec();
    printf("map<u64,double> insert %zu: %.3f s\n", n, t1 - t0);

    t0 = now_sec();
    for (i = 0; i < n; ++i) {
        map_iterator_t it = cmap->find(m, ds_u64key(keys[i]));
        if (it_ne(cmap->end(m), it))
            sink += (uint64_t)it_lfvalue(it);
    }
    t1 = now_sec();
    printf("map<u64,double> find   %zu: %.3f s\n", n, t1 - t0);

    t0 = now_sec();
    for (i = 0; i < n; ++i)
        sink += (uint64_t)cmap->remove(m, ds_u64key(keys[i]));
    t1 = now_sec();
    printf("map<u64,double> remove %zu: %.3f s\n", n, t1 - t0);

    MAP_DELETE(&m);
    free(keys);
}

static void bench_map_u32_u32(size_t n)
{
    uint32_t* keys = malloc(n * sizeof(*keys));
    map_t* m = MAP_NEW_KV(uint32_t, uint32_t);
    double t0, t1;
    size_t i;

    for (i = 0; i < n; ++i)
        keys[i] = (uint32_t)(i * UINT32_C(2654435761));

    t0 = now_sec();
    for (i = 0; i < n; ++i)
        cmap->insert(m, ds_u32key(keys[i]), ds_u32value((uint32_t)i));
    t1 = now_sec();
    printf("map<u32,u32>    insert %zu: %.3f s\n", n, t1 - t0);

    t0 = now_sec();
    for (i = 0; i < n; ++i) {
        map_iterator_t it = cmap->find(m, ds_u32key(keys[i]));
        if (it_ne(cmap->end(m), it))
            sink += it_u32value(it);
    }
    t1 = now_sec();
    printf("map<u32,u32>    find   %zu: %.3f s\n", n, t1 - t0);

    t0 = now_sec();
    for (i = 0; i < n; ++i)
        sink += (uint64_t)cmap->remove(m, ds_u32key(keys[i]));
    t1 = now_sec();
    printf("map<u32,u32>    remove %zu: %.3f s\n", n, t1 - t0);

    MAP_DELETE(&m);
    free(keys);
}

static void bench_vector_double(size_t n)
{
    vector_t* v = VECTOR_NEW_T(double);
    vector_iterator_t it;
    double t0, t1;
    size_t i;

    t0 = now_sec();
    for (i = 0; i < n; ++i)
        cvector->push_back(v, ds_lfdata((double)i));
    t1 = now_sec();
    printf("vector<double>  push   %zu: %.3f s\n", n, t1 - t0);

    t0 = now_sec();
    for (it = cvector->begin(v); it_ne(cvector->end(v), it); it = cvector->next(it))
        sink += (uint64_t)it_lfdata(it);
    t1 = now_sec();
    printf("vector<double>  iter   %zu: %.3f s\n", n, t1 - t0);

    VECTOR_DELETE(&v);
}

int main(int argc, char** argv)
{
    size_t n = argc > 1 ? (size_t)strtoull(argv[1], NULL, 10) : 1000000;

    printf("pointer=%zu bytes n=%zu\n", sizeof(void*), n);
    bench_map_u64_double(n);
    bench_map_u32_u32(n);
    bench_vector_double(n);
    printf("sink=%" PRIu64 "\n", sink);
    return 0;
}
