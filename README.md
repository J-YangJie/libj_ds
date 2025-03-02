# J's Data Structures Library

## Table of Contents

1. [Introduction](#1-introduction)
2. [Features](#2-features)
3. [How to use](#3-how-to-use)
4. [Performance - For reference only](#4-performance-\--for-reference-only)


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


## 4. Performance - For reference only

### 4.1 Ordered numbers

<div align="center">

| Performance            | Insert 10^7 Elements(s)  | Find 10^8 Times(s)  | Clear 10^7 Elements(s)  |
|------------------------|--------------------------|---------------------|-------------------------|
| hashmap(Init 2 * 10^7) |   0.498                  |   0.533             |   0.160                 |
| hashmap                |   0.775                  |   0.667             |   0.151                 |
| map                    |   1.888                  |   8.884             |   0.190                 |
| set                    |   1.701                  |   8.598             |   0.199                 |
| multimap               |   2.617                  |  12.264             |   0.195                 |
| multiset               |   2.652                  |   8.777             |   0.201                 |
| priority queue         |   0.198                  | unsupport           |   0.001                 |
|                        |                          | Find 10^2 Times(s)  |                         |
| list                   |   0.164                  |   0.000             |   0.066                 |
| vector                 |   0.051                  |   0.000(sort 0.020) |   0.001                 |

</div>

### 4.2 Random numbers

<div align="center">

| Model                  | Insert 10^7 Elements(s)  | Find 10^8 Times(s)  | Clear 10^7 Elements(s)  |
|------------------------|--------------------------|---------------------|-------------------------|
| hashmap(Init 2 * 10^7) |   1.794                  |   5.226             |   3.035                 |
| hashmap                |   3.430                  |   8.465             |   2.948                 |
| map                    |   9.340                  | 105.861             |   0.713                 |
| set                    |   9.300                  | 105.319             |   0.712                 |
| multimap               |   8.978                  | 111.242             |   0.747                 |
| multiset               |   8.629                  | 112.542             |   0.758                 |
| priority queue         |   0.211                  | unsupport           |   0.001                 |
|                        |                          | Find 10^2 Times(s)  |                         |
| list                   |   0.125                  |   2.999             |   0.070                 |
| vector                 |   0.094                  |   0.533(sort 0.866) |   0.001                 |

</div>

### 4.3 Random strings of variable lengths

<div align="center">

| Model                  | Insert 10^7 Elements(s)  | Find 10^8 Times(s)  | Clear 10^7 Elements(s)  |
|------------------------|--------------------------|---------------------|-------------------------|
| hashmap(Init 2 * 10^7) |   1.475                  |   9.464             |   0.531                 |
| hashmap                |   2.597                  |  14.369             |   0.523                 |
| map                    |   8.656                  |  86.873             |   0.376                 |
| set                    |   8.444                  |  85.522             |   0.389                 |
| multimap               |  13.033                  | 126.741             |   0.693                 |
| multiset               |  12.857                  | 128.514             |   0.718                 |
| priority queue         |   0.746                  | unsupport           |   0.139                 |
|                        |                          | Find 10^2 Times(s)  |                         |
| list                   |   0.599                  |   4.333             |   0.138                 |
| vector                 |   0.552                  |   3.941(sort 3.095) |   0.359                 |

</div>
