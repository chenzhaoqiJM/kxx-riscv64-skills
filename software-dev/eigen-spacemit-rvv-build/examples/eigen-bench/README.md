# Eigen Benchmark 示例

该目录用于对比 Eigen 矩阵乘法在不同 include 路径和编译参数下的性能：

- `bench_spacemit`：使用 `/opt/eigen-spacemit/include/eigen3`，启用 RVV 编译参数和 `EIGEN_RISCV64_USE_RVV10`。
- `bench_system`：使用 `/usr/include/eigen3`，使用标量 `rv64gc` 编译参数作为无 RVV 对照组。

## 编译

请在 riscv64 目标机器上编译，不要在 sshfs 挂载目录对应的本机环境中编译。

从本示例目录执行：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

## 运行

可执行文件生成在 `build/` 目录下：

```bash
./build/bench_spacemit [matrix_size] [iterations] [threads]
./build/bench_system [matrix_size] [iterations] [threads]
```

参数说明：

- `matrix_size`：矩阵大小 $N$，程序测试 $N \times N$ 的 `float` 矩阵乘法，默认 `512`。
- `iterations`：计时循环次数，默认 `10`。
- `threads`：Eigen 请求线程数，默认优先使用第三个命令行参数，其次使用环境变量 `EIGEN_BENCH_THREADS`，都未设置时为 `8`。

示例：

```bash
./build/bench_spacemit 512 20 8
./build/bench_system 512 20 8
```

也可以通过环境变量指定线程数：

```bash
EIGEN_BENCH_THREADS=4 ./build/bench_spacemit 512 20
```

如果启用了 OpenMP，也可以同步设置 `OMP_NUM_THREADS`：

```bash
OMP_NUM_THREADS=8 EIGEN_BENCH_THREADS=8 ./build/bench_spacemit 512 20
```

程序会输出 Eigen 版本、实际 SIMD 指令集、OpenMP 状态、线程配置以及平均矩阵乘法耗时。
