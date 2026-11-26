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
| vector          \| STL |   0.035 \|   0.101          |   0.000   \|   0.000      |   1.163   \|   0.513        |   0.000 \|   0.000         |
| deque           \| STL |   0.039 \|   0.042          |   0.000   \|   0.000      |   1.350   \|   0.584        |   0.006 \|   0.002         |
| list            \| STL |   0.178 \|   0.185          |   0.000   \|   0.000      |   2.940   \|   3.077        |   0.059 \|   0.057         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.039 \|   0.082        |   0.062   \|   0.022           |   0.007   \|   0.002     |   0.003 \|   0.002         |
| stack deque     \| STL |   0.049 \|   0.049        |   0.072   \|   0.026           |   0.013   \|   0.005     |   0.007 \|   0.008         |
| stack list      \| STL |   0.202 \|   0.183        |   0.036   \|   0.024           |   0.058   \|   0.058     |   0.062 \|   0.076         |
| queue deque     \| STL |   0.044 \|   0.041        |   0.028   \|   0.020           |   0.012   \|   0.007     |   0.009 \|   0.004         |
| queue list      \| STL |   0.184 \|   0.183        |   0.033   \|   0.024           |   0.074   \|   0.062     |   0.071 \|   0.064         |
| pqueue vector   \| STL |   0.182 \|   0.260        |   0.027   \|   0.021           |   0.475   \|   0.404     |   0.002 \|   0.002         |
| pqueue deque    \| STL |   0.305 \|   0.488        |   0.037   \|   0.022           |   0.654   \|   0.822     |   0.006 \|   0.011         |

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
| vector          \| STL |   0.139 \|   0.192          |   0.743   \|   0.490      |   1.237   \|   0.520        |   0.000 \|   0.000         |
| deque           \| STL |   0.143 \|   0.139          |   0.972   \|   0.578      |   1.294   \|   0.563        |   0.002 \|   0.006         |
| list            \| STL |   0.266 \|   0.255          |   2.747   \|   2.541      |   2.389   \|   2.810        |   1.281 \|   1.236         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.153 \|   0.204        |   0.053   \|   0.020           |   0.007   \|   0.001     |   0.002 \|   0.002         |
| stack deque     \| STL |   0.145 \|   0.172        |   0.073   \|   0.025           |   0.014   \|   0.006     |   0.007 \|   0.006         |
| stack list      \| STL |   0.279 \|   0.295        |   0.034   \|   0.021           |   0.057   \|   0.065     |   0.068 \|   0.062         |
| queue deque     \| STL |   0.153 \|   0.148        |   0.029   \|   0.022           |   0.012   \|   0.009     |   0.006 \|   0.006         |
| queue list      \| STL |   0.288 \|   0.261        |   0.038   \|   0.020           |   0.061   \|   0.067     |   0.071 \|   0.065         |
| pqueue vector   \| STL |   0.270 \|   0.269        |   0.029   \|   0.025           |   1.823   \|   1.719     |   0.002 \|   0.002         |
| pqueue deque    \| STL |   0.351 \|   0.285        |   0.035   \|   0.020           |   2.631   \|   2.923     |   0.002 \|   0.010         |

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
| vector          \| STL |   0.498 \|   0.560          |   1.781   \|   1.554      |   3.306   \|   3.060        |   0.018 \|   0.014         |
| deque           \| STL |   0.429 \|   0.381          |   1.816   \|   1.926      |   3.973   \|   3.525        |   0.034 \|   0.033         |
| list            \| STL |   0.556 \|   0.561          |   3.973   \|   4.493      |   5.336   \|   5.806        |   1.325 \|   1.297         |

</div>

<div align="center">

| Model                  | Push Elements(s)          | Top/Front Times(s)             | Pop Times(s)             | Clear Elements(s)          |
|------------------------|---------------------------|--------------------------------|--------------------------|----------------------------|
|                        | **Push 1\*10^7 Times(s)** | **Top/Front 1\*10^8 Times(s)** | **Pop 1\*10^7 Times(s)** | **Clear 1\*10^7 Times(s)** |
| stack vector    \| STL |   0.561 \|   0.704        |   0.048   \|   0.021           |   0.026   \|   0.016     |   0.031 \|   0.022         |
| stack deque     \| STL |   0.538 \|   0.538        |   0.076   \|   0.026           |   0.030   \|   0.024     |   0.033 \|   0.027         |
| stack list      \| STL |   0.733 \|   0.682        |   0.033   \|   0.021           |   0.079   \|   0.083     |   0.097 \|   0.078         |
| queue deque     \| STL |   0.578 \|   0.475        |   0.030   \|   0.020           |   0.029   \|   0.028     |   0.034 \|   0.026         |
| queue list      \| STL |   0.772 \|   0.736        |   0.032   \|   0.022           |   0.117   \|   0.084     |   0.098 \|   0.097         |
| pqueue vector   \| STL |   1.030 \|   1.263        |   0.032   \|   0.021           |   7.745   \|  11.363     |   0.034 \|   0.026         |
| pqueue deque    \| STL |   1.046 \|   1.235        |   0.038   \|   0.022           |   9.118   \|  14.067     |   0.026 \|   0.025         |

</div>
