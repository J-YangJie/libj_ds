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

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   0.898 /   1.032       |   1.406   /   3.561    |   6.561   /  12.788      |   0.180 /   0.092      |
| hashmap         / STL |   1.330 /   1.411       |   1.708   /   3.824    |   9.623   /  14.674      |   0.172 /   0.088      |
| map             / STL |   2.460 /   4.830       |  28.616   /  44.105    |  22.244   /  31.554      |   0.157 /   0.151      |
| set             / STL |   2.584 /   3.848       |  27.990   /  37.244    |  22.967   /  27.605      |   0.157 /   0.171      |
| multimap        / STL |   2.653 /   4.305       |  31.174   /  46.325    |  22.952   /  31.960      |   0.157 /   0.147      |
| multiset        / STL |   2.564 /   3.782       |  31.149   /  34.771    |  22.305   /  27.733      |   0.150 /   0.169      |
| priority queue  / STL |   0.740 /   3.269       | unsupport / unsupport  | unsupport / unsupport    |   0.000 /   0.002      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.273 /   0.414       |   0.000   /   0.000    |   3.288   /   6.129      |   0.104 /   0.133      |
| vector          / STL |   0.136 /   0.146       |   0.000   /   0.000    |   1.888   /   4.174      |   0.000 /   0.000      |

</div>

### 4.2 Random numbers

<div align="center">

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   3.894 /   2.368       |   8.190   /  15.994    |   7.993   /  13.231      |   0.598 /   0.159      |
| hashmap         / STL |   6.121 /   6.106       |  12.859   /  21.428    |  12.562   /  16.694      |   0.666 /   0.326      |
| map             / STL |  11.670 /  13.187       | 127.437   / 131.651    | 122.742   / 129.590      |   0.287 /   0.505      |
| set             / STL |  11.225 /  13.023       | 124.656   / 140.155    | 120.986   / 132.437      |   0.287 /   0.504      |
| multimap        / STL |  11.682 /  12.479       | 130.812   / 133.133    | 126.183   / 130.337      |   0.335 /   0.529      |
| multiset        / STL |  11.395 /  12.712       | 126.894   / 135.203    | 121.581   / 132.533      |   0.307 /   0.517      |
| priority queue  / STL |   0.324 /   0.780       | unsupport / unsupport  | unsupport / unsupport    |   0.000 /   0.002      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.248 /   0.336       |   4.008   /   8.100    |   4.081   /   6.863      |   0.109 /   0.138      |
| vector          / STL |   0.183 /   0.218       |   1.076   /   3.656    |   1.830   /   4.030      |   0.000 /   0.000      |

</div>

### 4.3 Random strings of variable lengths

<div align="center">

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   2.183 /   2.159       |  12.884   /  17.657    |  11.087   /  13.818      |   1.002 /   0.250      |
| hashmap         / STL |   4.497 /   4.840       |  16.927   /  26.717    |  14.636   /  22.736      |   1.145 /   0.525      |
| map             / STL |  11.505 /  11.817       | 106.850   / 111.196    | 117.456   / 119.741      |   0.596 /   0.852      |
| set             / STL |  11.202 /  11.186       | 105.472   / 110.548    | 116.333   / 119.213      |   0.587 /   0.848      |
| multimap        / STL |  15.573 /  14.134       | 139.487   / 133.521    | 117.811   / 121.338      |   0.595 /   0.873      |
| multiset        / STL |  14.848 /  14.253       | 137.652   / 134.120    | 117.682   / 121.731      |   0.577 /   0.860      |
| priority queue  / STL |   1.011 /   1.693       | unsupport / unsupport  | unsupport / unsupport    |   0.139 /   0.034      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.799 /   0.612       |   7.108   /  11.143    |   9.263   /  10.402      |   0.186 /   0.149      |
| vector          / STL |   0.717 /   0.742       |   6.970   /   8.500    |  29.613   /  11.661      |   0.358 /   0.020      |

</div>


## 5. Performance -O2 - For reference only

### 5.1 Ordered numbers

<div align="center">

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   0.534 /   0.192       |   0.494   /   0.360    |   4.665   /   5.071      |   0.113 /   0.051      |
| hashmap         / STL |   0.650 /   0.327       |   0.558   /   0.403    |   5.935   /   5.436      |   0.129 /   0.045      |
| map             / STL |   1.784 /   2.211       |   9.348   /  19.895    |  15.047   /  14.026      |   0.089 /   0.092      |
| set             / STL |   1.836 /   1.968       |   9.372   /  13.905    |  14.975   /  15.164      |   0.091 /   0.106      |
| multimap        / STL |   2.020 /   2.197       |  18.603   /  14.677    |  15.464   /  14.460      |   0.087 /   0.111      |
| multiset        / STL |   2.054 /   1.944       |   9.751   /  13.808    |  15.148   /  13.355      |   0.086 /   0.095      |
| priority queue  / STL |   0.199 /   0.241       | unsupport / unsupport  | unsupport / unsupport    |   0.000 /   0.002      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.188 /   0.176       |   0.000   /   0.000    |   3.002   /   2.682      |   0.065 /   0.045      |
| vector          / STL |   0.050 /   0.074       |   0.010   /   0.093    |   0.730   /   0.461      |   0.000 /   0.000      |

</div>

### 5.2 Random numbers

<div align="center">

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   3.437 /   0.995       |   5.486   /   6.042    |   5.407   /   5.621      |   0.346 /   0.116      |
| hashmap         / STL |   4.456 /   3.987       |   8.369   /   8.453    |   7.525   /   7.538      |   0.394 /   0.305      |
| map             / STL |  10.569 /  10.237       | 115.167   / 115.592    | 104.637   / 102.267      |   0.227 /   0.263      |
| set             / STL |  10.547 /   9.420       | 114.832   / 114.546    | 111.513   / 116.645      |   0.211 /   0.261      |
| multimap        / STL |  10.920 /   8.759       | 122.287   / 114.831    | 117.124   / 109.647      |   0.225 /   0.279      |
| multiset        / STL |  10.576 /   9.983       | 124.287   / 118.258    | 119.157   / 117.872      |   0.222 /   0.271      |
| priority queue  / STL |   0.222 /   0.250       | unsupport / unsupport  | unsupport / unsupport    |   0.000 /   0.005      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.170 /   0.159       |   3.951   /   3.855    |   4.111   /   4.050      |   0.066 /   0.057      |
| vector          / STL |   0.139 /   0.176       |   0.560   /   0.467    |   0.774   /   0.483      |   0.000 /   0.000      |

</div>

### 5.3 Random strings of variable lengths

<div align="center">

| Model                 | Insert 10^7 Elements(s) | Find 10^8 Times(s)     | Remove 10^8 Times(s)     | Clear 10^7 Elements(s) |
|-----------------------|-------------------------|------------------------|--------------------------|------------------------|
| hashmap_reserve / STL |   1.551 /   1.036       |   9.317   /  10.425    |   7.998   /   9.602      |   0.612 /   0.162      |
| hashmap         / STL |   2.980 /   3.529       |  12.340   /  17.501    |  10.789   /  16.096      |   0.674 /   0.463      |
| map             / STL |  10.608 /   9.472       | 101.660   /  95.045    | 112.529   /  97.946      |   0.457 /   0.451      |
| set             / STL |  10.671 /   9.872       | 103.762   /  98.896    | 114.052   / 102.997      |   0.458 /   0.464      |
| multimap        / STL |  14.361 /  12.374       | 137.609   / 118.065    | 115.720   / 104.587      |   0.456 /   0.467      |
| multiset        / STL |  14.040 /  12.568       | 135.342   / 119.957    | 115.028   / 107.190      |   0.454 /   0.473      |
| priority queue  / STL |   0.814 /   0.665       | unsupport / unsupport  | unsupport / unsupport    |   0.129 /   0.024      |
|                       |                         | **Find 10^2 Times(s)** | **Remove 10^2 Times(s)** |                        |
| list            / STL |   0.615 /   0.332       |   5.880   /   6.632    |   9.286   /   5.838      |   0.113 /   0.066      |
| vector          / STL |   0.567 /   0.356       |   5.263   /   6.254    |  19.446   /   6.501      |   0.330 /   0.013      |

</div>
