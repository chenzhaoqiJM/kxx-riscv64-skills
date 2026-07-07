---
name: ros2-jazzy-riscv64-build-debug
description: "Use when: 排查 Spacemit riscv64、k1、k3、Bianbu、开发板、嵌入式调试环境中的 ROS 2 Jazzy/Humble 包构建、Debian 打包、sbuild/buildd、CMake、Rust/Cargo、架构适配或依赖失败。"
---

# 目标

把 ROS 2 Jazzy/Humble 软件包在 Spacemit RISC-V / Bianbu / riscv64 环境中的构建失败，整理成可复用的排查流程和补丁判断方法。

本 Skill 来源于同目录 `ros2_fix.md` 的问题记录。需要包级补丁细节时，先读该文件，不要凭记忆重写大段补丁。

# 适用场景

- 在 Spacemit(R) X60 / `k1`、Spacemit(R) X100 / `k3` 或未来 Spacemit riscv64 芯片上构建 ROS 2 包。
- 排查 Bianbu / Ubuntu 24.04 / noble 上的 ROS 2 Jazzy Debian 包构建失败。
- 处理 `sbuild`、`buildd`、`dpkg-buildpackage`、`dh_auto_build`、`ament_cmake`、`CMake`、`cargo` 相关错误。
- 判断失败属于环境问题、上游源码问题、riscv64 架构适配问题、二进制库不支持，还是依赖关系需要裁剪。
- 未来新增 Spacemit 芯片时，也可复用本流程，但必须先确认芯片别名、系统版本、工具链和构建环境。

# 开始前确认

先收集这些信息：

1. 目标芯片型号：`k1`、`k3`、其他 Spacemit RISC-V 芯片，或多型号通用。
2. ROS 发行版：`jazzy`、`humble`，以及 Bianbu / Ubuntu / Debian 版本。
3. 失败包名和版本，例如 `ros-jazzy-librealsense2-2.55.1`。
4. 构建方式：`sbuild`、`buildd`、`dpkg-buildpackage`、`colcon`、本地原生构建或交叉构建。
5. 构建日志路径、源码路径、`debian/` 补丁路径。
6. 是否能在 x86 Ubuntu 24.04 复现同样失败。
7. 是否需要真实开发板或远端 riscv64 构建机。
8. 是否有外部资料：上游 commit、Bianbu 文档、SDK 文档、内部 GitLab、构建说明或已知 FAQ。

# 板卡环境要求

本 Skill 默认不强制连接真实开发板；它也适用于 CI 日志、buildd 日志、riscv64 chroot、sbuild 环境和源码包维护。

只有当用户要求在远端 riscv64 机器或开发板上复现、验证、编译时，才读取 `RISCV-REMOTE`：

```bash
test "${RISCV-REMOTE:-}" && printf '%s\n' "$RISCV-REMOTE"
```

`RISCV-REMOTE` 应匹配 `用户名@主机名或IP`，例如：

```bash
bianbu@10.0.91.146
```

如果缺失或格式错误，先询问用户提供可 SSH 登录的地址，不要猜测。若源码目录来自 `sshfs` 挂载，不要在本机执行编译或检查命令，优先通过 SSH 在远端 riscv64 环境执行。

# 工作流程

## 1. 先给失败分类

| 类别 | 典型信号 | 处理原则 |
| --- | --- | --- |
| 环境或权限问题 | `/sbuild-nonexistent`、`Permission denied`、找不到 `git`、网络 clone 失败 | 先修构建环境，不改源码 |
| 系统识别问题 | `OsNotDetected`、Bianbu 未被 `rospkg` 识别 | 临时用 `ROS_OS_OVERRIDE`，长期改 `python3-rospkg-modules` |
| riscv64 架构适配 | `Unknown architecture`、`-mssse3`、`-mtune=generic`、SIMD 报错 | 删除 x86 专用 flag，补 riscv64 分支 |
| 上游源码问题 | x86 同样失败，或上游已有修复 commit | 优先切上游修复版本或最小补丁 |
| ROS API 变化 | Jazzy 与 Humble 函数签名不同、typesupport 链接缺失 | 按 ROS 发行版做最小兼容 |
| 二进制库不支持 | 只提供 amd64/x86 动态库或闭源 SDK | 标记 delay，不伪造 riscv64 支持 |
| 依赖裁剪 | connextdds、gazebo、vendor 源码包缺失 | 去掉无人依赖的可选后端或记录未构建原因 |

## 2. 再决定是否是 riscv64 专属

- 如果 x86 Ubuntu 24.04 也失败，优先按上游源码或 ROS Jazzy 包问题处理。
- 如果只有 riscv64 失败，优先搜索 x86-only flag、SIMD、汇编、指针宽度、Rust target、CMake 架构判断。
- 如果失败只出现在 buildd/CI，本地 riscv64 能过，优先检查网络、权限、`HOME`、依赖安装和构建并发。

## 3. 常见修复模式

| 错误特征 | 常见修复 |
| --- | --- |
| `Project requires C++14 or higher` 但工具链实际支持 | 检查项目自定义 C++ 检测，必要时绕过错误检测 |
| `Could NOT find Threads` | 显式设置 `THREADS_PREFER_PTHREAD_FLAG` 或链接 `pthread` |
| `unrecognized command-line option '-mssse3'` | 在 CMake 架构判断里为 `riscv64` 跳过 x86 flag |
| `Unknown architecture` | 补 `riscv64` 分支；若依赖闭源动态库，标记不支持 |
| `OsNotDetected` | 临时 `export ROS_OS_OVERRIDE=Linux:6.6.36:noble`，长期补 rospkg |
| `Unsupported compiler or architecture` | 查上游是否已有 riscv64 commit，优先 cherry-pick |
| `ring` / Rust target unwrap panic | 参考上游 riscv64 支持补丁，避免直接改 cargo registry 当最终方案 |
| `_FORTIFY_SOURCE redefined [-Werror]` | 去掉重复定义，保留通用安全编译参数 |
| `overloaded-virtual` 被 `-Werror` 放大 | 对 ecl 系列包加最小 `-Wno-overloaded-virtual` |
| `cast from void* to uint32_t loses precision` | 64 位架构使用 `uint64_t`，不要按 32 位截断指针 |
| 找不到 `PCLConfig.cmake` | 补构建依赖，如 `libpcl-dev` |

## 4. 包级速查

| 包或包族 | 原始记录中的方向 |
| --- | --- |
| `python3-bloom` / `dh-python` | `rosdep` 映射从 `python3-rosdep2` 调整为 `python3-rosdep` |
| `ros-jazzy-control-box-rst` | `EdgeCache::pushJacobian()` 返回 `_jacobians.back()` |
| `ros-jazzy-librealsense2` | 处理 C++14 检测、pthread 检测和 `-mssse3` |
| `ros-jazzy-rc-genicam-api` | 多处 `Unknown architecture`，且有直接动态库依赖，谨慎标记未支持 |
| `ros-jazzy-lanelet2-*` / `depthai` | `rospkg` OS 识别；临时 `ROS_OS_OVERRIDE`，长期改模块包 |
| `ros-jazzy-lely-core-libraries` | 上游已有 riscv64 `mkjmp` 支持，优先用上游修复 |
| `ros-jazzy-zenoh-bridge-dds` | 区分 cargo 权限/网络问题和 `ring` riscv64 支持问题 |
| `gz-* vendor` / `aws-sdk-cpp-vendor` | 常见为缺 `git`、网络或 vendor 下载问题 |
| `ros-jazzy-gz-ogre-next-vendor` | 补 `__riscv64__`、禁用不适用 SIMD、补 shlibdeps 搜索路径 |
| `ros-jazzy-mimick-vendor` | 切换到支持目标架构的 Mimick 源 |
| `ecl-*`、`kobuki-core` | 同类 `overloaded-virtual` 警告转错误 |
| `rclcpp` 依赖链 | 裁剪无人依赖的 connextdds 路径 |
| `ros-jazzy-canopen-core` | x86 同样失败，优先更新到上游 `ros2_canopen` |
| `ros-jazzy-nav2-mppi-controller` | 移除 riscv64 不支持的 `-mtune=generic`，关闭不适用 xsimd 路径 |
| `ros-jazzy-nmea-hardware-interface` | 更新上游文件，并适配 Jazzy `ControllerInterface::init()` 签名 |
| `ros-jazzy-scenario-execution-rviz` | 删除重复 `_FORTIFY_SOURCE` 定义 |
| `ros-jazzy-avt-vimba-camera` / `spinnaker` | 依赖非 riscv64 SDK 或 amd64 deb，标记 delay |
| Humble 记录 | 关注 `mimick`、`ortools`、`rmw-implementation`、`mavros`、二进制库架构限制 |

## 5. 修改策略

1. 优先保留 `debian/`，只替换上游源码部分；替换前记录上游仓库、分支和 commit。
2. 能用上游 commit 解决时，不手写长期补丁。
3. 对 vendor 包，不要把个人本地绝对路径当最终方案；本地路径只能作为临时验证。
4. 对 buildd 问题，不要把网络、权限、OOM 误判成源码 bug。
5. 对二进制库缺 riscv64 的包，输出“暂缓/不可构建原因”，不要绕过检查后声称支持。

## 6. 验证

至少做一项能证明修复有效的验证：

```bash
dpkg-buildpackage -b -uc -us
```

或在 sbuild 环境验证：

```bash
sbuild -A -s -v
```

验证时记录：

- 包名、版本、ROS 发行版、目标架构。
- 实际执行的构建命令。
- 失败日志中原始错误是否消失。
- 生成的 `.deb`、构建产物或新的失败点。
- 如果只做了静态补丁整理，要明确说明尚未重新构建。

# 需要外部资料时

优先要求用户提供最小资料：

- 完整构建日志或失败片段。
- 源码包目录或补丁目录。
- 上游仓库、commit、release tag。
- Bianbu / Spacemit SDK / 内部 GitLab 链接。
- buildd 页面、CI job 链接或 sbuild 配置。

如果用户暂时没有资料，也继续按错误特征分类和同目录 `ros2_fix.md` 记录给出通用排查路径。

# 输出格式

完成排查或补丁整理后，按这个格式回复：

1. 目标芯片、系统版本、ROS 发行版和构建方式。
2. 错误分类：环境、依赖、riscv64 架构适配、上游源码、二进制库不支持或其他。
3. 修改了哪些文件，以及为什么是最小修改。
4. 实际验证命令和关键输出。
5. 仍未确认的信息或不能构建的原因。

# 注意事项

- 不要强制 SSH 或强制读取 `RISCV-REMOTE`，除非用户明确需要远端开发板或 riscv64 构建机。
- 不要把 x86 专用 SIMD、汇编或 `-m*` 参数照搬到 riscv64。
- 不要在 `sshfs` 本机挂载目录里直接编译远端源码。
- 不要把 `~/.cargo/registry`、本地 clone 路径或临时环境变量当成最终 Debian 源码包方案。
- 不要硬编码单一芯片；新增 Spacemit 芯片时先确认系统、工具链和架构参数。
