---
name: eigen-spacemit-rvv-build
description: "Use when: 为 Spacemit riscv64、k1、k3、Bianbu、开发板上的 C++ 程序配置 Eigen、系统 Eigen、eigen-spacemit、RVV 加速、EIGEN_RISCV64_USE_RVV10、CMake 编译或嵌入式调试。"
argument-hint: "说明要编译的 Eigen 程序路径、目标芯片 k1/k3、是否使用系统 Eigen 或 eigen-spacemit RVV"
---

# 目标

指导后续 Agent 在 Spacemit RISC-V 开发板上，为使用 Eigen 的 C++ 程序正确选择并编译以下两类构建：

- 系统 Eigen：通常位于 `/usr/include/eigen3`，用于无 RVV 的基线或兼容性验证。
- eigen-spacemit RVV 加速库：通常位于 `/opt/eigen-spacemit/include/eigen3`，用于启用 Spacemit RVV 优化。

参考本 Skill 自带示例：`./examples/eigen-bench`。

# 适用场景

- 在 Spacemit(R) X60 / `k1` 或 Spacemit(R) X100 / `k3` 开发板上编译 Eigen 程序。
- 对比系统 Eigen 与 eigen-spacemit 的矩阵计算性能或正确性。
- 为 CMake 项目添加 Eigen include 路径、RVV 编译参数、OpenMP 线程配置。
- 排查 Eigen 程序没有启用 RVV、误用了系统 Eigen、include 路径混乱、编译参数不匹配等问题。
- 未来新增 Spacemit RISC-V 芯片时，也可沿用本流程，但必须先确认芯片别名、支持的 `-march`、ABI 和 eigen-spacemit 安装路径。

# 开始前确认

在修改或生成编译配置前，先收集这些信息：

1. 目标芯片型号：
   - `k1` / Spacemit(R) X60
   - `k3` / Spacemit(R) X100
   - 其他 Spacemit RISC-V 芯片：要求用户给出型号、别名和文档来源
2. 目标程序路径：例如某个 `CMakeLists.txt`、单个 `.cpp`，或本 Skill 自带示例 `./examples/eigen-bench`。
3. 期望使用哪种 Eigen：
   - 系统 Eigen：`/usr/include/eigen3`
   - eigen-spacemit RVV：`/opt/eigen-spacemit/include/eigen3`
   - 两者都构建，用于对照验证
4. 是否需要 OpenMP：大矩阵乘法可启用；单线程基准或排查问题时可关闭。
5. 当前源码是否通过 `sshfs` 挂载：如果是，不要在本机编译、测试或跑基准。
6. 构建方式：CMake、Makefile、裸 `g++` 命令，或已有项目构建系统。

# 板卡环境要求

本 Skill 必须配合真实 Spacemit RISC-V 开发板使用。后续 Agent 必须按以下规则处理远程环境：

1. 读取环境变量 `RISCV-REMOTE`。
2. `RISCV-REMOTE` 必须匹配 `用户名@主机名或IP` 格式，例如：

   ```bash
   bianbu@10.0.91.146
   ```

3. 如果 `RISCV-REMOTE` 缺失或格式明显错误，先询问用户提供可 SSH 登录的开发板地址，不要猜测地址。
4. 允许通过 SSH 查看开发板环境，但命令应尽量只读或最小化：

   ```bash
   ssh "$RISCV-REMOTE" 'uname -m; cat /etc/os-release | head; ls -d /usr/include/eigen3 /opt/eigen-spacemit/include/eigen3 2>/dev/null'
   ```

5. 如果 SSH 提示密码、密钥或交互确认，停止并要求用户在终端中处理。
6. 如果源码目录是 sshfs 挂载，本地只读写文件；编译、测试、基准都应通过 SSH 在开发板或远端 riscv64 环境执行。

# 工作流程

## 1. 检查 Eigen 安装与目标架构

在开发板上确认基础信息：

```bash
ssh "$RISCV-REMOTE" 'set -eu
uname -m
printf "system eigen: "; test -d /usr/include/eigen3 && echo yes || echo no
printf "spacemit eigen: "; test -d /opt/eigen-spacemit/include/eigen3 && echo yes || echo no
'
```

判断规则：

- `uname -m` 应为 `riscv64`。
- 使用系统 Eigen 时，确认 `/usr/include/eigen3/Eigen` 存在。
- 使用 eigen-spacemit 时，确认 `/opt/eigen-spacemit/include/eigen3/Eigen` 存在。
- 如果 eigen-spacemit 不存在，先提示用户按 K1/K3 官方文档安装，不要回退后仍声称启用了 RVV。

## 2. 系统 Eigen 构建规则

系统 Eigen 用作基线或兼容路径。建议显式指定标量目标，避免意外启用 RVV：

```cmake
target_include_directories(app PRIVATE /usr/include/eigen3)
target_compile_options(app PRIVATE -O3 -DNDEBUG -march=rv64gc -mabi=lp64d)
```

裸编译示例：

```bash
g++ -O3 -DNDEBUG -std=c++17 -I/usr/include/eigen3 -march=rv64gc -mabi=lp64d main.cpp -o app_system
```

注意：系统 Eigen 路径不要同时混入 `/opt/eigen-spacemit/include/eigen3`，否则无法判断实际使用的是哪个实现。

## 3. eigen-spacemit RVV 构建规则

eigen-spacemit RVV 构建必须同时满足三类条件：

1. include 路径指向 `/opt/eigen-spacemit/include/eigen3`。
2. 编译参数启用 RVV，例如本仓库示例使用：
   - `-march=rv64gcv_zvl256b`
   - `-mabi=lp64d`
   - `-mrvv-vector-bits=zvl`
3. 定义 Eigen RVV 宏：
   - `EIGEN_RISCV64_USE_RVV10`

CMake 示例：

```cmake
target_include_directories(app PRIVATE /opt/eigen-spacemit/include/eigen3)
target_compile_options(app PRIVATE -O3 -DNDEBUG -march=rv64gcv_zvl256b -mabi=lp64d -mrvv-vector-bits=zvl)
target_compile_definitions(app PRIVATE EIGEN_RISCV64_USE_RVV10)
```

裸编译示例：

```bash
g++ -O3 -DNDEBUG -std=c++17 \
  -I/opt/eigen-spacemit/include/eigen3 \
  -march=rv64gcv_zvl256b -mabi=lp64d -mrvv-vector-bits=zvl \
  -DEIGEN_RISCV64_USE_RVV10 \
  main.cpp -o app_spacemit
```

如果目标芯片或工具链文档要求不同的 `-march`，以对应 K1/K3 文档为准，并在输出中说明依据。

## 4. 同一项目内构建双目标对照

优先使用双目标方式，避免通过全局 include 目录切换导致误用：

```cmake
function(add_eigen_app target eigen_include)
  add_executable(${target} main.cpp)
  target_include_directories(${target} PRIVATE "${eigen_include}")
  target_compile_options(${target} PRIVATE -O3 -DNDEBUG -std=c++17)
endfunction()

add_eigen_app(app_system /usr/include/eigen3)
target_compile_options(app_system PRIVATE -march=rv64gc -mabi=lp64d)

add_eigen_app(app_spacemit /opt/eigen-spacemit/include/eigen3)
target_compile_options(app_spacemit PRIVATE -march=rv64gcv_zvl256b -mabi=lp64d -mrvv-vector-bits=zvl)
target_compile_definitions(app_spacemit PRIVATE EIGEN_RISCV64_USE_RVV10)
```

可直接参考本 Skill 自带示例：`./examples/eigen-bench/CMakeLists.txt`。

## 5. OpenMP 与线程配置

如需让 Eigen 的大矩阵乘法使用多线程，可启用 OpenMP：

```cmake
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(app PRIVATE OpenMP::OpenMP_CXX)
endif()
```

运行时建议明确线程数，避免性能结果不可复现：

```bash
OMP_NUM_THREADS=8 ./app_spacemit
```

如果程序内部调用 `Eigen::setNbThreads(n)`，同时输出 `Eigen::nbThreads()` 和 `OMP_NUM_THREADS`，便于排查线程配置。

## 6. 编译与验证

如果项目位于远程开发板路径，直接在开发板上执行。若当前工作区是 sshfs 挂载，必须通过 SSH 执行远端命令：

```bash
ssh "$RISCV-REMOTE" 'cd /path/to/project && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)'
```

基于本 Skill 自带示例时，先将 `./examples/eigen-bench` 放到开发板可访问的工作目录，再在开发板上执行：

```bash
ssh "$RISCV-REMOTE" 'cd /path/to/eigen-bench && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)'
```

验证程序应尽量输出：

- Eigen 版本：`EIGEN_WORLD_VERSION.EIGEN_MAJOR_VERSION.EIGEN_MINOR_VERSION`
- SIMD 指令集：`Eigen::SimdInstructionSetsInUse()`
- 是否定义 OpenMP：`EIGEN_HAS_OPENMP`
- include 路径或目标名称：明确是 `system` 还是 `spacemit`
- 线程数：`Eigen::nbThreads()`、`OMP_NUM_THREADS`

可参考本 Skill 自带示例：`./examples/eigen-bench/bench.cpp`。

## 7. 常见问题判断

| 现象 | 优先检查 |
| --- | --- |
| RVV 目标性能没有提升 | 是否使用 `/opt/eigen-spacemit/include/eigen3`、是否定义 `EIGEN_RISCV64_USE_RVV10`、是否带 `-march=rv64gcv...` |
| 编译器不认识 `-mrvv-vector-bits=zvl` | 工具链版本或目标板系统编译器不支持；查看 K1/K3 文档要求 |
| 运行结果线程数不符合预期 | `OMP_NUM_THREADS`、`Eigen::setNbThreads()`、是否链接 OpenMP |
| 明明想用系统 Eigen 却输出 RVV | include 路径顺序污染，检查全局 `include_directories()`、环境变量、CMake cache |
| 编译在本机失败 | 检查是否误在 sshfs 本机环境编译；应在 riscv64 开发板或远端环境执行 |

# 需要外部资料时

优先参考官方文档，并在回答中标注文档来源：

- K3 Eigen RVV 文档：<https://www.spacemit.com/community/document/info?lang=zh&nodepath=software/SDK/ros/k3/06-%E7%B3%BB%E7%BB%9F%E4%B8%8E%E5%B9%B3%E5%8F%B0/6.5-%E9%AB%98%E6%80%A7%E8%83%BD%E8%AE%A1%E7%AE%97/6.5.2-Eigen-RVV.md>
- K1 Eigen RVV 文档：<https://www.spacemit.com/community/document/info?lang=zh&nodepath=software/SDK/ros/k1/03_Basic_applications/3.5_High_Performance_Computing_Library/3.5.2_eigen-rvv.md>

如用户提供新的 SDK、Bianbu 版本或编译器说明，应优先以用户提供的资料为准。

# 输出格式

完成配置或排查后，向用户汇报：

1. 目标芯片和远程开发板地址是否已确认。
2. 使用的是系统 Eigen、eigen-spacemit RVV，还是双目标对照。
3. 修改了哪些文件和关键编译参数。
4. 实际执行的远程验证命令。
5. 验证输出中的关键证据，例如 include 路径、SIMD 指令集、线程数或 benchmark 结果。
6. 未确认或需要用户继续提供的信息。

# 注意事项

- 不要把 `/usr/include/eigen3` 和 `/opt/eigen-spacemit/include/eigen3` 同时无序加入同一目标。
- 不要只切换 include 路径却忘记 RVV 编译参数和 `EIGEN_RISCV64_USE_RVV10`。
- 不要在 sshfs 挂载目录的本机环境中编译、测试或跑基准。
- 不要假设所有 Spacemit 芯片都使用相同 `-march`；新增芯片必须重新确认。
- 不要在无法验证远程环境时声称 RVV 已启用。
