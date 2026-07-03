---
name: eigen-benchmark-framework
description: "Use when: 为 Spacemit riscv64、k1、k3、Bianbu、真实开发板、嵌入式调试场景编写或重构 Eigen C++ 性能测试程序、benchmark、CSV 输出、RVV/标量对比、EIGEN_RISCV64_USE_RVV10 验证。"
argument-hint: "说明要新增或修改的 Eigen 性能测试文件、目标算子、芯片型号和远端源码路径"
---

# 目标

在真实 Spacemit RISC-V 开发板上编写 Eigen 性能测试程序时，约束后续 Agent 参考用户指定的现有 benchmark 示例或当前文件中的框架格式，生成可复现、可对比、便于 CSV 汇总的 benchmark 代码。

本 Skill 面向多型号通用场景，包括但不限于：

- Spacemit(R) X60 / `k1`
- Spacemit(R) X100 / `k3`
- 后续新增 Spacemit RISC-V 芯片

新增芯片时必须先确认芯片别名、Bianbu 版本、编译器能力、RVV 参数和 Eigen 安装路径，不要硬编码单一芯片假设。

# 适用场景

- 新增 Eigen 算子性能测试程序。
- 扩展已有 Eigen benchmark 程序。
- 对比系统 Eigen 与 eigen-spacemit RVV 性能。
- 统一 benchmark CSV 字段、输入构造、预热、计时、GFLOPS 统计和环境信息输出。
- 排查 Eigen benchmark 没有正确启用 RVV、线程配置不稳定、输出格式不可汇总等问题。

# 开始前确认

编写或修改代码前，先收集以下信息：

1. 目标芯片型号：
   - `k1` / Spacemit(R) X60
   - `k3` / Spacemit(R) X100
   - 其他 Spacemit RISC-V 芯片：要求用户给出型号、别名和文档来源
   - 多型号通用：保持参数可配置，不写死芯片专属路径
2. 远程开发板地址：必须通过 `RISCV-REMOTE` 获取。
3. 源码路径：区分本地 sshfs 路径和开发板上的真实执行路径。
4. 构建方式：CMake、Makefile、裸 `g++`，或已有脚本。
5. Eigen 来源：系统 Eigen、eigen-spacemit RVV，或双目标对照。
6. 目标算子类别：array、matrix、geometry、decomposition、reduction、block、custom 等。
7. 输出消费方式：是否需要兼容现有 CSV 后处理脚本。

# 板卡环境要求

本 Skill 必须配合真实 Spacemit RISC-V 开发板使用。后续 Agent 必须遵守以下规则：

1. 读取环境变量 `RISCV-REMOTE`。
2. `RISCV-REMOTE` 必须匹配 `用户名@主机名或IP` 格式，例如：

   ```bash
   bianbu@10.0.91.146
   ```

3. 如果 `RISCV-REMOTE` 缺失或格式明显错误，先询问用户提供可 SSH 登录的开发板地址，不要猜测地址。
4. 允许通过 SSH 查看开发板环境，但优先使用只读命令，例如：

   ```bash
   ssh "$RISCV-REMOTE" 'uname -m; cat /etc/os-release | head; g++ --version | head -1'
   ```

5. 如果 SSH 提示密码、密钥或交互确认，停止并要求用户在终端中处理。
6. 如果当前源码是 sshfs 挂载，本地只读写文件；不要在本机编译、测试或跑 benchmark，必须通过 SSH 在开发板或远端 riscv64 环境执行验证。

# 工作流程

## 1. 先复用现有框架，不另起炉灶

优先参考用户指定的 Eigen benchmark 示例文件结构；若用户没有指定示例文件，则按以下结构组织：

- `BenchmarkCase`：描述单个测试项。
- `TimingStats`：保存 `min_ms`、`max_ms`、`avg_ms`、`median_ms`。
- `BenchmarkInputs<Scalar>`：集中保存输入数据。
- `makeInputs<Scalar>()`：一次性构造确定性输入。
- `buildCases<Scalar>()`：表驱动注册 benchmark case。
- `measure()`：统一预热、计时、排序和统计。
- `gflopsPerSecond()`：统一 GFLOPS 计算。
- `csvEscape()`：统一 CSV 转义。
- `main()`：解析参数、构造 dtype、输出环境信息、循环执行 case。

除非用户明确要求，不要为每个算子单独写一套 main、计时器或输出格式。

## 2. BenchmarkCase 字段约束

每个测试项至少包含以下字段，并保持 CSV 输出顺序稳定：

```text
module,function,api,dtype,input_size,output_size,flop_count,min_ms,median_ms,avg_ms,max_ms,median_gflops,avg_gflops
```

字段含义：

- `module`：算子大类，例如 `array`、`matrix`、`geometry`、`decomposition`。
- `function`：稳定、脚本友好的函数名，使用小写和下划线，例如 `matrix_matmul`。
- `api`：人类可读的 Eigen API 表达式，例如 `A * B`、`colwise().sum()`。
- `dtype`：必须明确，例如 `float32`、`float64`。
- `input_size` / `output_size`：使用 `rows x cols` 字符串，例如 `512x512`、`262144x1`。
- `flop_count`：可以估算；不适合计算 GFLOPS 的访存、切片或构造类操作填 `0.0`。
- 时间字段单位固定为毫秒。
- GFLOPS 字段基于 `median_ms` 和 `avg_ms` 计算。

不要随意改字段名、字段顺序或单位；如果必须新增字段，应追加到末尾，并说明兼容性影响。

## 3. 输入数据约束

输入必须确定性、可复现：

- 使用固定 seed，例如 `std::srand(123456)`。
- 向量可用 `LinSpaced` 构造稳定区间。
- 矩阵可用 `Random`，但 seed 必须固定。
- 分解类算子必须使用可保证成功的输入，例如正定矩阵。
- 避免在计时循环内构造大输入，除非测试目标就是构造成本。
- 输入数据集中放入 `BenchmarkInputs<Scalar>`，不要散落在每个 lambda 里重复初始化。

如果算子会改变输入，例如 `normalize()`、原地分解、原地排序，应在 lambda 内复制局部变量，避免污染后续迭代。

## 4. dtype 覆盖约束

默认至少覆盖：

- `float` → `float32`
- `double` → `float64`

通过模板函数组织 dtype：

- `BenchmarkInputs<Scalar>` 保存输入。
- `dtypeName<Scalar>()` 输出 dtype 字符串。
- `buildCases<Scalar>()` 注册同一组算子。

如果新增 `int32`、`half`、`bfloat16` 或其他类型，必须说明：

- 是否被 Eigen 当前路径支持。
- 是否适合对应算子的数学语义。
- CSV 中的 dtype 命名。
- RVV 路径是否可验证。

## 5. 计时约束

统一使用 `std::chrono::steady_clock`，并保留预热和多次迭代：

- 默认参数可参考：`vector_size=262144`、`matrix_size=512`、`iterations=50`、`warmup=10`。
- 命令行参数必须允许覆盖规模、迭代次数和预热次数。
- `measure()` 先执行 warmup，再记录 samples。
- samples 排序后计算 min、max、avg、median。
- 主要比较优先看 `median_ms`，避免偶发抖动。

不要只跑一次就输出性能结论。

## 6. 防止编译器优化掉计算

每个 case 必须消费结果，避免 dead-code elimination：

- 标量结果调用 `consumeScalar(value)`。
- 向量或矩阵结果用 `.sum()`、`.coeffs().sum()` 等转为标量再消费。
- `consumeScalar` 可使用 `static volatile double sink`。

不要只写表达式而不使用结果。

## 7. 环境信息输出约束

CSV 数据行前必须输出足够的环境证据，至少包括：

- Eigen 版本：`EIGEN_WORLD_VERSION.EIGEN_MAJOR_VERSION.EIGEN_MINOR_VERSION`
- `Eigen::SimdInstructionSetsInUse()`
- `EIGEN_VECTORIZE`
- `EIGEN_VECTORIZE_RVV10`
- `EIGEN_RISCV64_USE_RVV10`
- `EIGEN_DONT_VECTORIZE`
- `EIGEN_USE_BLAS`
- `EIGEN_HAS_OPENMP`
- `vector_size`
- `matrix_size`
- `iterations`
- `warmup`
- 请求线程数和 `Eigen::nbThreads()`
- `OMP_NUM_THREADS`
- `EIGEN_BENCH_THREADS`
- `OPENBLAS_NUM_THREADS`

这些信息用于判断结果是否真的来自 RVV、是否被 OpenMP 或 BLAS 影响。不要为了输出更短而删除。

## 8. 线程约束

线程数必须显式可控：

- 优先支持命令行参数指定线程数。
- 其次读取 `EIGEN_BENCH_THREADS`。
- 默认值建议为 `1`，便于单线程 RVV 对比。
- 程序内调用 `Eigen::setNbThreads(threads)`。
- 输出 `requested_threads` 和 `Eigen nbThreads`。

对比 RVV 与 scalar 时，应保持线程数一致。

## 9. 算子注册约束

使用表驱动方式注册 case：

- 在 `buildCases<Scalar>()` 内定义 `add_case` lambda。
- 每个 case 的 lambda 捕获共享输入，不重复分配大输入。
- 按 `module` 分组排列，便于 diff 和 CSV 阅读。
- 新增 case 时同步检查 `cases.reserve()` 容量是否合理。
- `function` 名称一旦被外部脚本消费，避免改名。

新增算子时优先补齐：

1. `module`
2. `function`
3. `api`
4. 输入输出 shape
5. flop 估算
6. 结果消费
7. dtype 覆盖

## 10. FLOP 估算规则

FLOP 估算可以是近似值，但必须一致：

- 向量逐元素加减乘除：约 `N`。
- FMA：约 `2N`。
- dot：约 `2N - 1`。
- GEMV：约 `2MN - M`。
- GEMM：约 `2MNP - MP`，方阵可写为 `2N^3 - N^2`。
- reduce sum：约 `N - 1`。
- determinant/inverse/分解类可使用常见近似并在代码中保持清晰。
- block、transpose、Identity、Zero 等主要不是浮点运算的 case 可填 `0.0`。

当 `flop_count <= 0` 时，GFLOPS 输出应为 `0.0`。

## 11. 构建与远端验证

修改 benchmark 后，必须在真实 Spacemit RISC-V 开发板上做最小验证。

如果当前仓库通过 sshfs 挂载，本地只编辑，不本地构建。通过 SSH 在远端执行，例如：

```bash
ssh "$RISCV-REMOTE" 'cd <远端项目根目录> && cmake --build <构建目录> --target <target> -j$(nproc)'
```

运行 benchmark 时，先用小规模参数冒烟，避免长时间占用开发板：

```bash
ssh "$RISCV-REMOTE" 'cd <远端benchmark目录> && ./<benchmark_binary> 1024 32 3 1 1 | head -40'
```

验证重点：

- 程序能编译并退出码为 0。
- 输出包含环境信息和 CSV header。
- 至少有目标算子的 `float32` 和 `float64` 数据行。
- RVV 构建输出中 `EIGEN_RISCV64_USE_RVV10` 为 `defined`，或按目标场景明确说明不是 RVV 构建。
- 标量和 RVV 对照时，输入规模、迭代次数、线程数一致。

# 需要外部资料时

用户已说明当前没有外部参考文档。后续如涉及新芯片、新 Bianbu 版本、新工具链或非默认 RVV 参数，应要求用户补充：

- 芯片型号和别名。
- Bianbu 或 SDK 版本。
- 编译器版本。
- Eigen 或 eigen-spacemit 安装路径。
- 推荐的 `-march`、`-mabi` 和 RVV 编译参数。

没有资料时，也可以按本 Skill 的通用框架编写 benchmark，但不要声称适配了某个未验证的新芯片。

# 输出格式

完成代码修改或生成后，向用户汇报：

1. 修改或新增的 benchmark 文件路径。
2. 新增了哪些 module/function/API。
3. 是否保持了既有 CSV header 和字段顺序。
4. 覆盖了哪些 dtype。
5. 使用的输入规模、迭代次数、warmup 和线程配置。
6. 实际远端验证命令。
7. 验证输出中的关键证据。
8. 仍未确认或需要用户补充的信息。

# 注意事项

- 不要在 sshfs 本机环境中编译、测试或跑 benchmark。
- 不要写一次性、不可汇总的自由格式输出。
- 不要删除环境信息输出。
- 不要只支持单一 dtype，除非算子本身不支持模板化，并且要说明原因。
- 不要在计时循环中做无关的大量内存分配。
- 不要把 correctness check 和 benchmark 混在同一个热循环里；正确性可在计时前后单独做轻量校验。
- 不要把 RVV 是否启用只当作编译参数猜测，必须通过输出宏、SIMD 字符串或远端构建证据确认。
