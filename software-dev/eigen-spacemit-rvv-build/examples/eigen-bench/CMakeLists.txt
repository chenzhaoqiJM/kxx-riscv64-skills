cmake_minimum_required(VERSION 3.16)
project(eigen_bench LANGUAGES CXX)

# OpenMP 让 Eigen 支持的单个算子（主要是大矩阵乘法）可以使用多线程。
option(EIGEN_BENCH_ENABLE_OPENMP "Enable OpenMP so Eigen can parallelize supported operators" ON)

# RVV 编译参数。
set(RVV_MARCH "rv64gcv_zvl256b" CACHE STRING "RISC-V architecture flags for RVV builds")
set(RVV_MABI  "lp64d"           CACHE STRING "RISC-V ABI flags for RVV builds")

# 系统 Eigen 对照组保持无 RVV，需显式指定标量目标架构和 ABI。
set(SCALAR_MARCH "rv64gc" CACHE STRING "RISC-V architecture flags for scalar non-RVV builds")
set(SCALAR_MABI  "lp64d"  CACHE STRING "RISC-V ABI flags for scalar non-RVV builds")

if(EIGEN_BENCH_ENABLE_OPENMP)
  find_package(OpenMP)
endif()


# 统一创建 benchmark 目标：传入不同 Eigen include 路径即可对比不同实现。
function(add_bench target eigen_include)
  add_executable(${target} bench.cpp)
  target_include_directories(${target} PRIVATE "${eigen_include}")
  target_compile_options(${target} PRIVATE -O3 -DNDEBUG -std=c++17)
  if(EIGEN_BENCH_ENABLE_OPENMP AND OpenMP_CXX_FOUND)
    target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
  endif()
endfunction()

# SpacemiT Eigen（RVV 加速）
add_bench(bench_spacemit /opt/eigen-spacemit/include/eigen3)
target_compile_options(bench_spacemit PRIVATE -march=${RVV_MARCH} -mabi=${RVV_MABI} -mrvv-vector-bits=zvl)
target_compile_definitions(bench_spacemit PRIVATE EIGEN_RISCV64_USE_RVV10)

# 系统 Eigen（无 RVV）
add_bench(bench_system /usr/include/eigen3)
target_compile_options(bench_system PRIVATE -march=${SCALAR_MARCH} -mabi=${SCALAR_MABI})
