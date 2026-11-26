/*
  libj_ds permanent tests deterministic RNG
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

#include <stddef.h>
#include <stdint.h>

#include "test.h"

uint64_t test_rng_seed = 0x9e3779b97f4a7c15ULL;

void test_rng_init(uint64_t seed)
{
    test_rng_seed = seed != 0 ? seed : 0x9e3779b97f4a7c15ULL;
}

uint64_t test_rng_u64(void)
{
    uint64_t z = (test_rng_seed += 0x9e3779b97f4a7c15ULL);

    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

uint32_t test_rng_u32(void)
{
    return (uint32_t)(test_rng_u64() >> 32);
}

size_t test_rng_bounded(size_t bound)
{
    if (bound == 0)
        return 0;
    return (size_t)(test_rng_u64() % (uint64_t)bound);
}

size_t test_rng_range(size_t lo, size_t hi)
{
    if (lo >= hi)
        return lo;
    return lo + test_rng_bounded(hi - lo + 1);
}

bool test_rng_percent(unsigned percent)
{
    if (percent == 0)
        return false;
    if (percent >= 100)
        return true;
    return test_rng_u32() % 100u < percent;
}
