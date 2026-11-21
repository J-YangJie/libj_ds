# libj_ds 测试体系设计

> 状态：设计稿 / 逐步实现中  
> 更新日期：2026-11-19  
> 目标：让每次修改都能通过自动测试确认正确性，而不是靠肉眼看代码。

---

## 1. 总原则

1. **demo 不是测试**
   - `demo/` 只负责展示推荐用法，面向新手和首次使用者。
   - demo 可以编译、运行、打印预期输出，但不承担正确性证明。
   - 不在 demo 中加入 `assert`、`demo_check`、失效迭代器测试等测试逻辑。

2. **正确性检查属于 `tests/`**
   - 语义、返回值、边界、内存所有权、回调行为、迭代器失效等，都放在独立测试目录。
   - 每修复一个 bug，应在 `tests/` 中增加最小回归用例，再修复并跑完整测试。

3. **性能测试不是正确性测试**
   - `main.c` / `main_stl.cpp` 的 performance 对比只用于观察性能趋势。
   - 性能结果不作为正确性门禁，也不放在每次 PR 的必跑测试里。

4. **随机测试必须可复现**
   - 记录 seed 和操作轨迹。
   - 失败时打印 seed、操作序号、期望值/实际值，然后非零退出或 `abort()`。
   - 不使用 `while (true);` 这种无法定位的失败方式。

5. **测试代码分层，避免重复膨胀**
   - 通用契约只写一次，通过薄适配器复用到多个容器。
   - 随机测试复用操作生成器、tracked value、分配器等基础设施。

---

## 2. 测试分层

| 层级 | 名称 | 主要目的 | 运行频率 | 是否门禁 |
|---|---|---|---|---|
| L0 | 构建/API 冒烟 | 头文件、库、demo、测试目标能编译 | 每次提交 | 是 |
| L1 | 单元/语义测试 | 固定输入验证单个操作的语义和边界 | 每次 PR | 是 |
| L2 | 契约测试 | 验证同类容器共同遵守的公共契约 | 每次 PR | 是 |
| L3 | 模型随机测试 | 随机操作序列与参考模型逐步对比 | 每次 PR 短跑 + nightly 长跑 | 短跑是 |
| L4 | 长时间随机 stress | 全随机合法操作 + 不变量 + 内存检查 | nightly / release | nightly/release |
| L5 | 性能测试 | 测吞吐/耗时，观察趋势 | nightly / 手动 | 否 |

sanitizer、故障注入、32 位、对齐、SSO 等属于横切配置，适用于 L1~L4。

---

## 3. 各层职责与测试内容

### L0：构建/API 冒烟

**目的**

- 防止测试代码、demo、头文件在 API 变化后悄悄腐烂。
- 当前仓库已经出现过 demo 和 stress 因 API 变化而无法编译的情况。

**内容**

- 每个公开头文件单独包含并编译。
- `make` 构建库。
- 所有 `demo/*.c` 编译通过。
- `tests/` 与 `stress/` 目标编译通过。
- 可选：C99/C11、gcc/clang、32/64 位各编译一次。

**计划命令**

```sh
make check
```

`make check` 至少包含：

- `make test`
- `make demos`
- 测试目标和 stress 目标的编译检查

**不负责**

- 不验证行为正确性。
- 不运行长测试。

---

### L1：单元/语义测试

**目的**

- 用固定输入验证单个 API 的具体语义。
- 覆盖边界、返回值、空容器、单元素、容量阈值、字符串切换等。

**内容**

- 生命周期：`NEW/DELETE/clear`，空容器状态。
- 插入删除：`push_back/pop_back/insert/insert_n/insert_range/erase/erase_range`。
- 查找：`find/count/remove/remove_if`。
- 访问：`at/front/back`，返回值类型和边界。
- 迭代器：`begin/end/next/prev/rbegin/rend/rnext/rprev`，正向/反向遍历。
- 容量：`size/capacity/max_size/reserve/resize/shrink_to_fit`。
- 赋值/交换：`assign/assign_range/swap`。
- 字符串：`char*` 与 `ds_sso_t`，短串和长串。
- 自定义 ops：`valid_data/copy_data/free_data/__move`。
- 失败路径：分配失败、非法范围（在允许的调试模式下）。

**测试代码（计划）**

```text
tests/unit/test_vector.c
tests/unit/test_list.c
tests/unit/test_deque.c
tests/unit/test_stack.c
tests/unit/test_queue.c
tests/unit/test_priority_queue.c
tests/unit/test_associative.c
tests/unit/test_hashmap.c
tests/unit/test_string.c
tests/unit/test_ops.c
```

**输出要求**

- 断言失败时打印文件名、行号、期望值、实际值。
- 测试结束打印 pass/fail 数量，失败返回非零。

---

### L2：契约测试

**目的**

- 用一套测试验证同类容器的共同语义，减少重复代码。
- 通过公共 vtable 或薄适配器复用，而不是为每个容器重写一套。

**可复用的契约**

| 契约 | 适用容器 | 主要内容 |
|---|---|---|
| sequence contract | vector / list / deque | 顺序、首尾、正反遍历、插入删除、返回迭代器 |
| associative contract | map / set / multimap / multiset | 有序遍历、重复 key、lower/upper_bound、erase |
| hashmap contract | hashmap | hash/eq、重复 key、扩容、查找、删除 |
| adapter contract | stack / queue / priority_queue | push/pop/top/front/back/size/swap/clear |

**通用不变量示例**

- 正向遍历次数 == 反向遍历次数 == `size`。
- `begin` 到 `end` 的顺序与 `front/back` 一致。
- `insert` 返回的迭代器指向新元素。
- `erase` 返回的迭代器指向被删除元素的后继。
- `clear` 返回删除数量，之后 `size == 0`。
- 拷贝后两个容器内容独立，修改一个不影响另一个。
- `delete` 后所有元素都被释放。

**测试代码（计划）**

```text
tests/contract/test_sequence.c
tests/contract/test_associative.c
tests/contract/test_hashmap.c
tests/contract/test_adapter.c
```

**代码量控制**

- 每个契约只写一次。
- 每个容器只提供少量适配器，或者直接使用已有 `class_*_t`。
- 不为每个容器复制整套测试逻辑。

---

### L3：模型随机测试

**目的**

- 用独立参考模型作为 oracle，随机生成合法操作序列，逐步对比容器和模型。
- 这是“语义正确性”的主要随机验证层。

**参考模型**

- vector/deque：简单数组模型。
- list：简单双向链表模型。
- map/set：排序数组或独立树模型。
- hashmap：简单链式哈希模型。
- stack/queue/priority_queue：数组/堆模型。

参考模型应**独立实现**，不要复用被测容器的代码，避免共同 bug。

**每步检查**

- `size`。
- 完整遍历顺序。
- `front/back`。
- `find/count`。
- `remove` 返回值。
- `insert/erase` 后的内容和返回迭代器。
- 分配/释放计数。

**可复现性**

- 使用固定 seed；CI 使用固定 seed。
- nightly 使用随机 seed，但必须打印 seed。
- 支持 `--seed`、`--iters`。
- 失败时打印最后一个操作、期望值、实际值、seed，然后非零退出。
- 失败后把最小操作序列沉淀到 `tests/unit/` 作为回归用例。

**测试代码（计划）**

```text
tests/model/test_sequence_model.c
tests/model/test_associative_model.c
tests/model/test_hashmap_model.c
tests/model/test_adapter_model.c
```

**关于 C++ 参考模型**

- 可以把 `std::vector/std::list/std::map/std::unordered_map` 当模型。
- 当前公共头文件没有 `extern "C"` 保护，C++ 测试直接链接 C 库需要额外处理。
- 方案一：给公共头文件加 `extern "C"` 保护。
- 方案二：测试侧写一层 C 适配包装。
- 方案三：先用 C 参考模型，后续再引入 C++ 模型。

---

### L4：长时间随机 stress

**目的**

这是 stress 的明确定位：

1. **发现极难设计的组合 bug**
   - 长时间、全随机的合法操作序列，覆盖大量状态组合。
   - 重点覆盖扩容/缩容、哈希扩容、SSO 长短串切换、分配失败后的清理等。

2. **在完全随机情况下检查内存泄漏**
   - 配合 Valgrind / ASan / LSan，检查随机路径下是否泄漏、越界、double free。
   - 让第 N 次分配失败，遍历 N，检查错误路径是否清理干净。

**和 L3 的区别**

- L3 有明显 oracle，短跑、可解释，适合 PR 门禁。
- L4 跑得更久、更随机、操作范围更宽，适合 nightly/release。
- 两者可以共用操作生成器和 fixture。

**“全随机”的约束**

- 随机的是**合法操作序列**，不是随机非法调用或失效迭代器。
- 失效迭代器的使用属于 UB，应该用 debug iterator / 死亡测试单独验证。
- 每一步仍要检查不变量；否则 stress 只能发现崩溃，发现不了“结果错了”。
- 纯均匀随机效率不高，建议做结构化随机：
  - 给插入/删除/查找等操作加权；
  - 主动注入边界值：空容器、单元素、容量阈值、SSO 边界、哈希负载因子边界；
  - 随机字符串长度覆盖短串和长串；
  - 关联容器覆盖重复 key、lower/upper_bound、erase_range。

**操作范围**

- 生命周期：`new/delete/clear/copy/swap/assign/resize/reserve/shrink`。
- 序列容器：`push_back/pop_back/insert/erase/erase_range`。
- 适配器：`push/pop/top/front/back`。
- 关联容器：插入、删除、查找、上下界、重复 key。
- 字符串：`char*` 与 `ds_sso_t`。
- 自定义 ops：复制、释放、移动、深拷贝独立性。
- 故障注入：第 N 次分配失败。

**内存检查**

- 每个容器单独跑，便于定位泄漏来源；混合跑作为补充。
- 每次 `clear/delete` 后检查分配/释放计数是否归零。
- 进程结束时用 Valgrind/LSan 检查全局泄漏。
- 分配失败路径要额外跑一遍，确认失败后也能清理干净。

**运行频率**

| 场景 | 规模 | 说明 |
|---|---|---|
| PR 冒烟 | 固定 seed，几秒到几十秒 | 防止明显退化 |
| nightly | 随机 seed，几小时 | 长时间发现组合 bug |
| release | nightly + Valgrind 深检 | 确认无泄漏后发布 |

**Valgrind 示例**

```sh
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --errors-for-leak-kinds=all \
         --error-exitcode=1 \
         ./stress/stress_bin --seed=12345 --iters=1000000
```

Valgrind 较慢，长跑优先用 ASan/LSan；Valgrind 用于 nightly 或 release 前的深度检查。

**测试代码（计划）**

```text
stress/stress_main.c
stress/stress_ops.c
stress/stress_invariants.c
stress/stress_alloc.c
stress/stress_trace.c
```

现有 `stress/main.c`、`stress/main_slow.c` 的随机循环可以作为起点，但目前使用旧 API，需先修复。

---

### L5：性能测试

**目的**

- 只观察性能趋势，不证明正确性。
- 不进入 PR 正确性门禁。

**内容**

- 现有 `main.c`、`main_stl.cpp` 的 performance 对比。
- 建议整理到独立的 `bench/` 或 `performance/` 目录。
- 记录编译选项、CPU、seed、数据规模，保证可比性。

**运行频率**

- nightly 或手动。
- 可选做性能回归阈值，但不阻塞正确性测试。

---

## 4. 测试维度覆盖矩阵

| 测试方面 | L1 单元 | L2 契约 | L3 模型 | L4 stress |
|---|---|---|---|---|
| 生命周期/空容器 | 是 | 是 | 是 | 是 |
| 容量/reserve/resize | 是 | 部分 | 是 | 是 |
| 插入/删除/返回迭代器 | 是 | 是 | 是 | 是 |
| 查找/count/remove | 是 | 是 | 是 | 是 |
| 正/反向迭代器 | 是 | 是 | 是 | 是 |
| 迭代器失效 | 文档 + debug | debug | 有效性跟踪 | debug 模式 |
| 字符串/SSO | 是 | 是 | 是 | 是 |
| 自定义 ops/深拷贝 | 是 | 是 | 是 | 是 |
| 分配失败 | 是 | 部分 | 部分 | 是 |
| 内存泄漏 | ASan | ASan | ASan | Valgrind/ASan |
| 组合/长序列 | 少 | 少 | 是 | 是 |
| 性能趋势 | 否 | 否 | 否 | 否（L5） |

---

## 5. 迭代器失效怎么测

迭代器失效是语义契约的一部分，但普通 C 测试无法直接“断言 UB”。建议：

1. **文档化规则**
   - 在头文件注释中明确每种操作会使哪些迭代器失效。
   - 例如：vector 扩容后所有迭代器失效；list 删除只使被删节点迭代器失效；关联容器删除只使被删迭代器失效。

2. **测试合法模式**
   - 测试 `insert/erase/erase_range` 返回的迭代器可以继续使用。
   - 测试在明确不失效的操作后继续遍历。
   - 测试 `clear` 后只使用 `begin/end` 等新迭代器。

3. **Debug iterator 模式**
   - 迭代器携带容器标识和代际/版本号。
   - 容器发生失效操作时递增版本。
   - `next/prev/at` 等检查版本，失效则明确 abort 并打印信息。
   - release 构建零开销；debug 构建启用。
   - 用“死亡测试”（fork 子进程，期望 abort）验证失效行为。

4. **随机测试约束**
   - L3/L4 的随机操作只使用合法迭代器。
   - 不把随机非法使用当作 stress 的一部分；那测的是 UB，不是库的 bug。

---

## 6. 测试基础设施

### 6.1 轻量测试框架

建议先使用仓库内的轻量 `tests/test.h`：

- `TEST_ASSERT_TRUE`
- `TEST_ASSERT_EQ`
- `TEST_ASSERT_NULL`
- 测试注册和运行
- 失败时打印文件、行号、期望值、实际值

如果后续需要更完整功能，再考虑 Unity/CMocka 等单文件测试框架。

### 6.2 测试值 / 深拷贝

使用 `tracked_value` 之类的测试类型：

- 带唯一 ID。
- 记录 copy/free 次数。
- 复制后两个对象内容独立。
- 所有副本最终都被释放。
- 用于验证 `copy_data/free_data/__move` 的语义。

### 6.3 分配器故障注入

建议提供测试分配器：

- 统计 `malloc/calloc/realloc/free` 次数。
- 支持“第 N 次分配失败”。
- 遍历 N，检查容器在失败后仍然合法、无泄漏。
- 不依赖随机概率，便于复现。

### 6.4 随机数

- 使用自定义确定性 PRNG（如 xorshift/PCG），避免 `rand()` 跨平台差异。
- 打印 seed。
- 支持命令行 seed 和迭代次数。

### 6.5 死亡测试

用于验证：

- debug iterator 失效检查会 abort。
- 某些不可恢复错误路径。
- 子进程退出码/信号符合预期。

---

## 7. 目录规划

```text
tests/
  test.h
  test_main.c
  support/
    test_rng.c
    test_alloc.c
    test_value.c
  unit/
    test_vector.c
    test_list.c
    test_deque.c
    test_stack.c
    test_queue.c
    test_priority_queue.c
    test_associative.c
    test_hashmap.c
    test_string.c
    test_ops.c
  contract/
    test_sequence.c
    test_associative.c
    test_hashmap.c
    test_adapter.c
  model/
    test_sequence_model.c
    test_associative_model.c
    test_hashmap_model.c
    test_adapter_model.c

stress/
  stress_main.c
  stress_ops.c
  stress_invariants.c
  stress_alloc.c
  stress_trace.c

bench/            # 或沿用当前 performance 文件
demo/             # 只放教学示例
```

---

## 8. Makefile 目标规划

```text
make test              # L1 + L2 + L3 短跑
make test-asan         # ASan/LSan
make test-ubsan        # UBSan
make test-32           # 32 位构建
make test-align        # WITH_ALIGNMEM=1
make stress            # L4 短跑（固定 seed）
make stress-long       # L4 长跑（随机 seed，nightly）
make stress-valgrind   # L4 + Valgrind 深检
make demos             # 所有 demo 编译
make check             # test + demos + 测试目标编译
make bench             # 性能测试（不进门禁）
```

运行示例：

```sh
make test
make test-asan
make stress STRESS_SEED=12345 STRESS_ITERS=1000000
make stress-valgrind STRESS_SEED=12345 STRESS_ITERS=100000
```

---

## 9. CI 矩阵建议

### 每次 PR

- gcc + clang
- C99 / C11（至少一个）
- 64 位
- `WITH_ALIGNMEM=0`
- `make check`
- `make test`
- ASan + UBSan
- L4 短跑（固定 seed）

### nightly

- 32 位
- `WITH_ALIGNMEM=1`
- SSO 短/长边界
- 多个随机 seed 的长 stress
- Valgrind 深检
- 可选 coverage

### release 前

- 全矩阵
- 长时间 stress
- Valgrind/ASan 无泄漏
- 性能趋势记录

---

## 10. 当前仓库状态与已知问题

### 已完成

- `demo/demo_vector.c` 已回归纯教学风格，移除了 `demo_check`。
- `demo/demo_vector_bin` 已加入 `DEMO_BINS`，可以参与构建。

### 待处理

- `stress/main.c`、`stress/main_slow.c` 当前不能编译：
  - `stress/test_vector.c` 仍使用已删除的 `first/last/push_front/pop_front/sort` 和旧的双参数 `next/prev`。
  - `stress/main.c:160` 仍使用旧的 `cpriority_queue->pop(ds, 0)`。
- `stress` 目前主要是容器之间互相比对，没有独立参考模型，不能作为唯一正确性 oracle。
- `stress` 失败使用 `while (true);`，没有 seed/trace，不可复现。
- 仓库目前没有 `tests/` 目录、`make test`、sanitizer 目标。
- `demo/demo_list_bin`、`demo/demo_deque_bin` 仍是注释状态。

### 处理顺序

1. 修复或暂时隔离无法编译的 stress 目标，避免继续腐烂。
2. 建立 `tests/` 骨架和 `make test`。
3. 先做 vector 的单元测试和模型测试。
4. 再做 sequence 契约，覆盖 vector/list/deque。
5. 再做 associative/hashmap/adapter 契约和模型。
6. 最后增强 stress：seed、trace、invariant、故障注入、Valgrind。

---

## 11. 分阶段实施计划

### Phase 0：构建门禁

- `make check` 编译库、demo、测试目标。
- 修复或隔离当前无法编译的 stress。
- 确保所有 demo 都能编译。

**验收**：`make check` 通过。

### Phase 1：测试骨架 + vector 单元测试

- 新增 `tests/test.h`、`tests/test_main.c`。
- 新增 `tests/unit/test_vector.c`。
- 覆盖生命周期、插入删除、查找、容量、迭代器、字符串。
- 新增 `make test`。

**验收**：`make test` 运行通过，失败时能打印具体断言位置。

### Phase 2：vector 模型测试

- 新增 `tests/model/test_sequence_model.c`。
- 用数组模型跑 vector 随机操作。
- 支持 seed、iters、失败 trace。

**验收**：固定 seed 和随机 seed 都能通过；失败可复现。

### Phase 3：sequence 契约

- 新增 `tests/contract/test_sequence.c`。
- 覆盖 vector/list/deque 共同契约。
- 使用薄适配器或公共 vtable。

**验收**：三个容器跑同一套契约。

### Phase 4：associative / hashmap / adapter 契约

- 覆盖 map/set/multimap/multiset/hashmap。
- 覆盖 stack/queue/priority_queue。
- 加入 lower/upper_bound、重复 key、哈希扩容等。

**验收**：所有容器都有契约测试。

### Phase 5：stress 增强

- 修复 `stress/main.c`、`stress/main_slow.c` 的旧 API。
- 加入 seed、iters、操作 trace。
- 加入不变量检查。
- 加入确定性分配失败。
- 加入 Valgrind/ASan 目标。

**验收**：短 stress 进 PR；长 stress 可在 nightly 跑几小时。

### Phase 6：CI 矩阵

- gcc/clang、32/64、align、ASan/UBSan。
- demo 编译检查。
- nightly 长 stress + Valgrind。

**验收**：每次 PR 自动跑 `make check`，nightly 自动跑长测试。

---

## 12. 其他库的参考

- **libstdc++ / libc++**
  - 每个容器有独立 testsuite。
  - `_GLIBCXX_DEBUG` 调试迭代器检查失效。
  - CI 跑 sanitizer。
- **Abseil**
  - GoogleTest 单元测试。
  - 随机测试、fuzz、sanitizer、benchmark 分层。
- **Boost.Container**
  - Boost.Test。
  - 重点测试 allocator、异常安全、深拷贝。
- **GLib**
  - `g_test` 框架，`make check` 统一运行。
- **Google Guava**
  - `CollectionTestSuite`：一套通用集合契约复用于多个实现。
  - 和本仓库通过 `class_*_t` 写通用契约的思路最接近。
- **Rust std / Go std**
  - 单元测试 + Miri/fuzz；表驱动测试。

这些库的共同点：

- 单元、契约、模型、stress、性能分层。
- 随机测试可复现。
- 迭代器失效/UB 用 debug 模式或 sanitizer 检查。
- 测试和 demo 分离。
- CI 强制编译并运行。

---

## 13. 工作约定

以后修改代码时：

- **修 bug**
  - 先在 `tests/` 加最小回归用例。
  - 修复后跑 `make test` 和 sanitizer。
- **加功能**
  - 补单元测试，并至少补一个契约或模型用例。
- **改 demo**
  - 只保证编译、运行不崩、输出符合预期。
  - 不把断言和正确性检查塞进 demo。
- **改容器/API**
  - 跑 `make check`，确保所有 demo、测试、stress 目标至少能编译。
- **stress 发现 bug**
  - 保存 seed 和操作轨迹。
  - 最小化成 `tests/unit/` 回归用例。
  - 修复后同时跑短 stress 和长 stress。

---

## 14. 实施状态

- [x] 记录测试体系设计
- [x] `demo_vector` 移除 `demo_check`，回归教学示例
- [ ] `make check` / 构建门禁
- [ ] `tests/test.h` 测试骨架
- [ ] vector 单元测试
- [ ] vector 模型测试
- [ ] sequence 契约测试
- [ ] associative/hashmap/adapter 契约测试
- [ ] stress 修复与增强
- [ ] ASan/UBSan/Valgrind 目标
- [ ] CI 矩阵
- [ ] 性能测试目录整理
