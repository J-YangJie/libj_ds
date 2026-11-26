# J's Data Structures Library

## Table of Contents

1. [Introduction](#1-introduction)
2. [Features](#2-features)
3. [How to use](#3-how-to-use)
4. [Performance -O2 - For reference only](#4-performance-\-o2-\--for-reference-only)


## 1. Introduction

`JDSL(J's Data Structures Library)` is a data structures library written in `C`, featuring interfaces that closely resemble those of the `STL(Standard Template Library)`.


## 2. Features

1. **Object Oriented**
2. **Have the concept of iterator**
3. **Be able to interface with the memory pool(malloc, calloc, realloc)**


## 3. How to use

1. **Performance testing**: Enable option `WITH_PERFORMANCE` in the `Makefile`

```pip-requirements
WITH_PERFORMANCE=y
```

2. **Demo testing**: Enable option `WITH_DEMO` in the `Makefile`

```pip-requirements
WITH_DEMO=y
```

3. **Select and compile**: Select the data structures you need in the `Makefile` and compile them.

```pip-requirements
WITH_VECTOR=y
WITH_DEQUE=y
WITH_LIST=y
WITH_STACK=y
WITH_QUEUE=y
WITH_PRIORITY_QUEUE=y
WITH_HASHMAP=y
WITH_MAP=y
WITH_MULTIMAP=y
WITH_SET=y
WITH_MULTISET=y
```

4. **Code**: Write code by referring to the `demo`.

```pip-requirements
demo/demo_hashmap.c
demo/demo_list.c
demo/...
```


## 4. Performance -O2 - For reference only

### 4.1 Ordered numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   0.670 \|   0.379          |   0.345   \|   0.372      |   5.979   \|   5.990        |   0.196 \|   0.095         |
| hashmap         \| STL |   0.863 \|   0.682          |   0.309   \|   0.347      |   6.393   \|   6.188        |   0.180 \|   0.100         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |   2.484 \|   3.639          |  15.428   \|  40.164      |  17.440   \|  23.025        |   0.090 \|   0.109         |
| set             \| STL |   2.587 \|   3.296          |  17.257   \|  31.160      |  15.070   \|  17.396        |   0.091 \|   0.103         |
| multimap        \| STL |   2.808 \|   3.882          |  17.681   \|  36.288      |  18.529   \|  19.587        |   0.092 \|   0.113         |
| multiset        \| STL |   2.884 \|   3.358          |  14.958   \|  31.276      |  14.913   \|  19.677        |   0.093 \|   0.101         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| vector          \| STL |   0.031 \|   0.086          |   0.000   \|   0.000      |   1.160   \|   0.468        |   0.000 \|   0.000         |
| deque           \| STL |   0.048 \|   0.038          |   0.000   \|   0.000      |   1.375   \|   0.641        |   0.006 \|   0.006         |
| list            \| STL |   0.167 \|   0.168          |   0.000   \|   0.000      |   2.757   \|   2.680        |   0.060 \|   0.059         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.045 \|   0.080        |   0.049   \|   0.021           |   0.007   \|   0.002     |   0.004 \|   0.002         |
| stack deque     \| STL |   0.052 \|   0.045        |   0.079   \|   0.031           |   0.013   \|   0.006     |   0.007 \|   0.006         |
| stack list      \| STL |   0.183 \|   0.168        |   0.036   \|   0.020           |   0.065   \|   0.067     |   0.055 \|   0.063         |
| queue deque     \| STL |   0.046 \|   0.045        |   0.030   \|   0.021           |   0.014   \|   0.011     |   0.011 \|   0.002         |
| queue list      \| STL |   0.180 \|   0.168        |   0.038   \|   0.022           |   0.059   \|   0.061     |   0.062 \|   0.058         |
| pqueue vector   \| STL |   0.179 \|   0.243        |   0.029   \|   0.021           |   0.484   \|   0.398     |   0.002 \|   0.002         |
| pqueue deque    \| STL |   0.289 \|   0.448        |   0.035   \|   0.020           |   0.547   \|   0.826     |   0.006 \|   0.002         |

</div>

### 4.2 Random numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   2.640 \|   2.407          |   6.617   \|   7.255      |   6.371   \|   6.753        |   0.413 \|   0.254         |
| hashmap         \| STL |   3.902 \|   6.827          |   6.560   \|   8.597      |   6.246   \|   8.147        |   0.422 \|   0.624         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |   9.957 \|   9.540          | 112.004   \| 115.749      | 110.767   \| 108.614        |   0.221 \|   0.267         |
| set             \| STL |   9.989 \|   9.316          | 116.323   \| 115.275      | 117.202   \| 113.990        |   0.216 \|   0.266         |
| multimap        \| STL |   9.956 \|   9.643          | 117.726   \| 117.705      | 115.948   \| 108.554        |   0.220 \|   0.286         |
| multiset        \| STL |   9.960 \|   9.803          | 119.583   \| 119.024      | 118.666   \| 118.858        |   0.223 \|   0.289         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| vector          \| STL |   0.142 \|   0.193          |   0.723   \|   0.451      |   1.176   \|   0.459        |   0.000 \|   0.000         |
| deque           \| STL |   0.146 \|   0.139          |   0.965   \|   0.563      |   1.343   \|   0.606        |   0.003 \|   0.002         |
| list            \| STL |   0.274 \|   0.264          |   2.559   \|   2.584      |   2.660   \|   2.762        |   0.059 \|   0.056         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.145 \|   0.186        |   0.047   \|   0.022           |   0.007   \|   0.002     |   0.002 \|   0.002         |
| stack deque     \| STL |   0.148 \|   0.149        |   0.075   \|   0.033           |   0.014   \|   0.013     |   0.002 \|   0.002         |
| stack list      \| STL |   0.268 \|   0.259        |   0.034   \|   0.023           |   0.060   \|   0.065     |   0.064 \|   0.052         |
| queue deque     \| STL |   0.167 \|   0.151        |   0.030   \|   0.022           |   0.016   \|   0.005     |   0.009 \|   0.002         |
| queue list      \| STL |   0.262 \|   0.257        |   0.042   \|   0.025           |   0.063   \|   0.062     |   0.061 \|   0.052         |
| pqueue vector   \| STL |   0.249 \|   0.242        |   0.028   \|   0.022           |   1.600   \|   1.617     |   0.002 \|   0.002         |
| pqueue deque    \| STL |   0.325 \|   0.261        |   0.044   \|   0.022           |   2.231   \|   2.711     |   0.004 \|   0.002         |

</div>

### 4.3 Random strings of variable lengths

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   5.431 \|   4.851          |  18.934   \|  22.546      |  16.124   \|  20.857        |   0.694 \|   0.384         |
| hashmap         \| STL |   7.109 \|   8.841          |  19.242   \|  31.888      |  15.921   \|  28.551        |   0.708 \|   0.702         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |  12.257 \|  12.276          | 135.199   \| 140.054      | 142.005   \| 136.878        |   0.277 \|   0.305         |
| set             \| STL |  12.727 \|  12.315          | 136.853   \| 136.745      | 145.338   \| 138.491        |   0.282 \|   0.325         |
| multimap        \| STL |  14.520 \|  13.754          | 160.581   \| 154.593      | 147.626   \| 138.924        |   0.285 \|   0.329         |
| multiset        \| STL |  14.702 \|  13.725          | 163.531   \| 158.298      | 151.533   \| 141.142        |   0.286 \|   0.350         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| vector          \| STL |   0.448 \|   0.646          |   1.715   \|   1.605      |   2.587   \|   2.970        |   0.022 \|   0.016         |
| deque           \| STL |   0.418 \|   0.403          |   1.849   \|   1.865      |   2.940   \|   3.493        |   0.028 \|   0.034         |
| list            \| STL |   0.590 \|   0.564          |   4.239   \|   4.724      |   5.630   \|   6.091        |   0.086 \|   0.091         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.456 \|   0.582        |   0.046   \|   0.022           |   0.027   \|   0.021     |   0.027 \|   0.024         |
| stack deque     \| STL |   0.434 \|   0.387        |   0.074   \|   0.027           |   0.033   \|   0.027     |   0.032 \|   0.044         |
| stack list      \| STL |   0.575 \|   0.584        |   0.035   \|   0.020           |   0.083   \|   0.089     |   0.084 \|   0.084         |
| queue deque     \| STL |   0.440 \|   0.404        |   0.031   \|   0.022           |   0.034   \|   0.029     |   0.039 \|   0.027         |
| queue list      \| STL |   0.585 \|   0.564        |   0.039   \|   0.023           |   0.088   \|   0.086     |   0.096 \|   0.088         |
| pqueue vector   \| STL |   0.937 \|   1.044        |   0.032   \|   0.024           |   6.921   \|  10.442     |   0.027 \|   0.022         |
| pqueue deque    \| STL |   0.933 \|   0.948        |   0.041   \|   0.021           |   8.167   \|  12.235     |   0.025 \|   0.032         |

</div>
