# J's Data Structures Library

## Table of Contents

1. [Introduction](#1-introduction)
2. [Features](#2-features)
3. [How to use](#3-how-to-use)
4. [Performance -O0 - For reference only](#4-performance-\-o0-\--for-reference-only)
5. [Performance -O2 - For reference only](#5-performance-\-o2-\--for-reference-only)


## 1. Introduction

`JDSL(J's Data Structures Library)` is a data structures library written in `C`, featuring interfaces that closely resemble those of the `STL(Standard Template Library)`.


## 2. Features

1. **Object Oriented**
2. **Have the concept of iterator**
3. **Be able to interface with the memory pool(malloc、calloc、realloc)**


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
WITH_LIST=y
WITH_VECTOR=y
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


## 4. Performance -O0 - For reference only

### 4.1 Ordered numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   1.116 \|   1.841          |   1.144   \|   3.981      |   9.536   \|  17.235        |   0.319 \|   0.186         |
| hashmap         \| STL |   1.640 \|   2.549          |   1.105   \|   3.711      |  10.980   \|  16.286        |   0.308 \|   0.185         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |   2.460 \|   4.830          |  28.616   \|  44.105      |  22.244   \|  31.554        |   0.157 \|   0.151         |
| set             \| STL |   2.584 \|   3.848          |  27.990   \|  37.244      |  22.967   \|  27.605        |   0.157 \|   0.171         |
| multimap        \| STL |   2.653 \|   4.305          |  31.174   \|  46.325      |  22.952   \|  31.960        |   0.157 \|   0.147         |
| multiset        \| STL |   2.564 \|   3.782          |  31.149   \|  34.771      |  22.305   \|  27.733        |   0.150 \|   0.169         |
| priority queue  \| STL |   0.740 \|   3.269          | unsupport \| unsupport    | unsupport \| unsupport      |   0.000 \|   0.002         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.292 \|   0.432          |   0.000   \|   0.000      |   2.962   \|   6.429        |   0.115 \|   0.138         |
| vector          \| STL |   0.137 \|   0.157          |   0.000   \|   0.000      |   1.929   \|   4.093        |   0.000 \|   0.000         |

</div>

### 4.2 Random numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   4.491 \|   5.615          |  11.068   \|  20.772      |  10.098   \|  16.294        |   0.768 \|   0.323         |
| hashmap         \| STL |   8.065 \|  11.955          |  11.205   \|  22.740      |  10.048   \|  18.667        |   0.762 \|   0.672         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |  11.670 \|  13.187          | 127.437   \| 131.651      | 122.742   \| 129.590        |   0.287 \|   0.505         |
| set             \| STL |  11.225 \|  13.023          | 124.656   \| 140.155      | 120.986   \| 132.437        |   0.287 \|   0.504         |
| multimap        \| STL |  11.682 \|  12.479          | 130.812   \| 133.133      | 126.183   \| 130.337        |   0.335 \|   0.529         |
| multiset        \| STL |  11.395 \|  12.712          | 126.894   \| 135.203      | 121.581   \| 132.533        |   0.307 \|   0.517         |
| priority queue  \| STL |   0.324 \|   0.780          | unsupport \| unsupport    | unsupport \| unsupport      |   0.000 \|   0.002         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.351 \|   0.485          |   2.734   \|   7.491      |   3.232   \|   6.394        |   0.120 \|   0.144         |
| vector          \| STL |   0.195 \|   0.241          |   1.172   \|   3.407      |   2.001   \|   3.913        |   0.000 \|   0.000         |

</div>

### 4.3 Random strings of variable lengths

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   4.883 \|   5.431          |  15.451   \|  20.506      |  13.400   \|  16.718        |   2.004 \|   0.571         |
| hashmap         \| STL |   6.968 \|   9.083          |  19.984   \|  28.018      |  18.108   \|  23.784        |   1.904 \|   1.109         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |  11.505 \|  11.817          | 106.850   \| 111.196      | 117.456   \| 119.741        |   0.596 \|   0.852         |
| set             \| STL |  11.202 \|  11.186          | 105.472   \| 110.548      | 116.333   \| 119.213        |   0.587 \|   0.848         |
| multimap        \| STL |  15.573 \|  14.134          | 139.487   \| 133.521      | 117.811   \| 121.338        |   0.595 \|   0.873         |
| multiset        \| STL |  14.848 \|  14.253          | 137.652   \| 134.120      | 117.682   \| 121.731        |   0.577 \|   0.860         |
| priority queue  \| STL |   1.011 \|   1.693          | unsupport \| unsupport    | unsupport \| unsupport      |   0.139 \|   0.034         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.936 \|   0.667          |   3.735   \|   9.884      |   6.104   \|  10.016        |   0.197 \|   0.156         |
| vector          \| STL |   0.754 \|   0.753          |   2.996   \|   8.499      |  24.350   \|  11.211        |   0.371 \|   0.025         |

</div>

## 5. Performance -O2 - For reference only

### 5.1 Ordered numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   0.680 \|   0.371          |   0.351   \|   0.384      |   6.559   \|   6.067        |   0.183 \|   0.099         |
| hashmap         \| STL |   0.857 \|   0.672          |   0.341   \|   0.369      |   6.970   \|   6.312        |   0.181 \|   0.108         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |   1.784 \|   2.211          |   9.348   \|  19.895      |  15.047   \|  14.026        |   0.089 \|   0.092         |
| set             \| STL |   1.836 \|   1.968          |   9.372   \|  13.905      |  14.975   \|  15.164        |   0.091 \|   0.106         |
| multimap        \| STL |   2.020 \|   2.197          |  18.603   \|  14.677      |  15.464   \|  14.460        |   0.087 \|   0.111         |
| multiset        \| STL |   2.054 \|   1.944          |   9.751   \|  13.808      |  15.148   \|  13.355        |   0.086 \|   0.095         |
| priority queue  \| STL |   0.199 \|   0.241          | unsupport \| unsupport    | unsupport \| unsupport      |   0.000 \|   0.002         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.182 \|   0.179          |   0.000   \|   0.000      |   2.623   \|   2.963        |   0.063 \|   0.063         |
| vector          \| STL |   0.041 \|   0.079          |   0.005   \|   0.092      |   0.738   \|   0.513        |   0.000 \|   0.000         |

</div>

### 5.2 Random numbers

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   2.693 \|   2.508          |   7.084   \|   7.460      |   6.945   \|   6.901        |   0.456 \|   0.254         |
| hashmap         \| STL |   4.032 \|   6.981          |   7.078   \|   8.828      |   6.861   \|   8.403        |   0.437 \|   0.645         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |  10.569 \|  10.237          | 115.167   \| 115.592      | 104.637   \| 102.267        |   0.227 \|   0.263         |
| set             \| STL |  10.547 \|   9.420          | 114.832   \| 114.546      | 111.513   \| 116.645        |   0.211 \|   0.261         |
| multimap        \| STL |  10.920 \|   8.759          | 122.287   \| 114.831      | 117.124   \| 109.647        |   0.225 \|   0.279         |
| multiset        \| STL |  10.576 \|   9.983          | 124.287   \| 118.258      | 119.157   \| 117.872        |   0.222 \|   0.271         |
| priority queue  \| STL |   0.222 \|   0.250          | unsupport \| unsupport    | unsupport \| unsupport      |   0.000 \|   0.005         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.251 \|   0.261          |   2.540   \|   2.733      |   2.692   \|   2.838        |   0.066 \|   0.051         |
| vector          \| STL |   0.143 \|   0.174          |   0.491   \|   0.449      |   0.684   \|   0.442        |   0.000 \|   0.000         |

</div>

### 5.3 Random strings of variable lengths

<div align="center">

| Model                  | Insert Elements(s)          | Find Times(s)             | Remove Times(s)             | Clear Elements(s)          |
|------------------------|-----------------------------|---------------------------|-----------------------------|----------------------------|
|                        | **Insert 2\*10^7 Times(s)** | **Find 1\*10^8 Times(s)** | **Remove 1\*10^8 Times(s)** | **Clear 2\*10^7 Times(s)** |
| hashmap_reserve \| STL |   3.795 \|   3.154          |  11.844   \|  12.701      |  10.161   \|  11.803        |   1.108 \|   0.385         |
| hashmap         \| STL |   4.852 \|   6.482          |  14.249   \|  18.699      |  12.977   \|  18.128        |   1.187 \|   1.065         |
|                        | **Insert    10^7 Times(s)** | **Find    10^8 Times(s)** | **Remove    10^8 Times(s)** | **Clear    10^7 Times(s)** |
| map             \| STL |  10.608 \|   9.472          | 101.660   \|  95.045      | 112.529   \|  97.946        |   0.457 \|   0.451         |
| set             \| STL |  10.671 \|   9.872          | 103.762   \|  98.896      | 114.052   \| 102.997        |   0.458 \|   0.464         |
| multimap        \| STL |  14.361 \|  12.374          | 137.609   \| 118.065      | 115.720   \| 104.587        |   0.456 \|   0.467         |
| multiset        \| STL |  14.040 \|  12.568          | 135.342   \| 119.957      | 115.028   \| 107.190        |   0.454 \|   0.473         |
| priority queue  \| STL |   0.814 \|   0.665          | unsupport \| unsupport    | unsupport \| unsupport      |   0.129 \|   0.024         |
|                        | **Insert    10^7 Times(s)** | **Find    10^2 Times(s)** | **Remove    10^2 Times(s)** | **Clear    10^7 Times(s)** |
| list            \| STL |   0.706 \|   0.352          |   2.967   \|   6.027      |   5.704   \|   6.031        |   0.123 \|   0.077         |
| vector          \| STL |   0.568 \|   0.337          |   1.789   \|   5.654      |  14.785   \|   6.549        |   0.340 \|   0.014         |

</div>
