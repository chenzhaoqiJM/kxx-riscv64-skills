
### 问题记录

#### 1、python3-bloom依赖冲突问题

dh-python工具解析rosdep python依赖时将其对应包设置为了python-rosdep2

```Shell
sudo vim /usr/share/dh-python/dist/cpython3_fallback

# 定位到rosdep
rosdep python3-rosdep2 改为 rosdep python3-rosdep
```

#### 2、ros-jazzy-control-box-rst

X86 ubuntu24.04编译报一样的问题

报错信息：

```Shell
/home/zq/Desktop/deb/ros-jazzy-control-box-rst/ros-jazzy-control-box-rst-0.0.7/src/optimization/include/corbo-optimization/hyper_graph/edge_cache.h: In member function ‘Eigen::MatrixXd& corbo::EdgeCache::pushJacobian(int, int)’:
/home/zq/Desktop/deb/ros-jazzy-control-box-rst/ros-jazzy-control-box-rst-0.0.7/src/optimization/include/corbo-optimization/hyper_graph/edge_cache.h:78:36: error: cannot bind non-const lvalue reference of type ‘Eigen::MatrixXd&’ {aka ‘Eigen::Matrix<double, -1, -1>&’} to an rvalue of type ‘Eigen::MatrixXd’ {aka ‘Eigen::Matrix<double, -1, -1>’}
   78 |         return _values.emplace_back();
      |                ~~~~~~~~~~~~~~~~~~~~^~
```

解决：src/optimization/include/corbo-optimization/hyper_graph/edge_cache.h如下修改：

```C++
    Eigen::MatrixXd& pushJacobian(int value_dim, int param_dim)
    {
        PRINT_DEBUG_COND_ONCE(_jacobians.size() >= _values.capacity(),
                              "EdgeCache::pushJacobian(): cache capacity reached; you might better reserve more space in advance.");
#if __cplusplus > 201402L
        // return _values.emplace_back();
        _jacobians.emplace_back(value_dim, param_dim);
        return _jacobians.back();
#else
        _jacobians.emplace_back(value_dim, param_dim);
        return _jacobians.back();
#endif
    }
```

#### 3、ros-jazzy-librealsense2

X86下编译无报错

报错信息a：

```Shell
CMake Error at CMake/lrs_macros.cmake:17 (message):
  Project 'realsense2' requires C++14 or higher
Call Stack (most recent call first):
  CMakeLists.txt:51 (config_cxx_flags)

-- Configuring incomplete, errors occurred!
```

解决a：

注释掉ros-jazzy-librealsense2-2.55.1/CMakeList.txt文件中51行的config_cxx_flags()

报错信息b:

```Shell
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Check if compiler accepts -pthread
-- Check if compiler accepts -pthread - no
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
CMake Error at /usr/share/cmake-3.28/Modules/FindPackageHandleStandardArgs.cmake:230 (message):
  Could NOT find Threads (missing: Threads_FOUND)
Call Stack (most recent call first):
  /usr/share/cmake-3.28/Modules/FindPackageHandleStandardArgs.cmake:600 (_FPHSA_FAILURE_MESSAGE)
  /usr/share/cmake-3.28/Modules/FindThreads.cmake:226 (FIND_PACKAGE_HANDLE_STANDARD_ARGS)
  examples/measure/CMakeLists.txt:9 (find_package)
```

解决b:

指定使用系统的pthread，ros-jazzy-librealsense2-2.55.1/CMakeList.txt文件第四行起添加

```Shell
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
```

报错信息c:

```Shell
CMake Error at /usr/share/cmake-3.28/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

    "/usr/bin/cc"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: '/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim'

    Run Build Command(s): /usr/bin/cmake -E env VERBOSE=1 /usr/bin/gmake -f Makefile cmTC_46c3b/fast
    gmake[5]: 进入目录“/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim”
    /usr/bin/gmake  -f CMakeFiles/cmTC_46c3b.dir/build.make CMakeFiles/cmTC_46c3b.dir/build
    gmake[6]: 进入目录“/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim”
    Building C object CMakeFiles/cmTC_46c3b.dir/testCCompiler.c.o
    /usr/bin/cc   -g -O2 -fno-omit-frame-pointer -ffile-prefix-map=/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1=. -fstack-protector-strong -Wformat -Werror=format-security -fno-stack-clash-protection -fdebug-prefix-map=/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1=/usr/src/ros-jazzy-librealsense2-2.55.1-1noble -Wdate-time -D_FORTIFY_SOURCE=3   -pedantic -D_DEFAULT_SOURCE   -mssse3 -pthread  -o CMakeFiles/cmTC_46c3b.dir/testCCompiler.c.o -c /home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim/testCCompiler.c
    cc: error: unrecognized command-line option ‘-mssse3’
    gmake[6]: *** [CMakeFiles/cmTC_46c3b.dir/build.make:78：CMakeFiles/cmTC_46c3b.dir/testCCompiler.c.o] 错误 1
    gmake[6]: 离开目录“/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim”
    gmake[5]: *** [Makefile:127：cmTC_46c3b/fast] 错误 2
    gmake[5]: 离开目录“/home/zq-ros2/tmp/ros-jazzy-librealsense2/ros-jazzy-librealsense2-2.55.1/.obj-riscv64-linux-gnu/libcurl/src/libcurl-build/CMakeFiles/CMakeScratch/TryCompile-Xt7sim”
```

应用了`-mssse3`标志（特定于 x86/x86_64）并导致构建失败

解决c:

定位到：ros-jazzy-librealsense2-2.55.1/CMake/unix_config.cmake

从31行如下增加：

```Shell
    # RISC-V architecture flags
    elseif(${MACHINE} MATCHES "riscv64-*")
        # No x86 specific flags like -mssse3; you can add RISC-V specific flags here if needed
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
        # Default case for x86 and others (fallback)
    else()
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mssse3")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mssse3")
        set(LRS_TRY_USE_AVX true)
    endif(${MACHINE} MATCHES "arm-*")
```

#### 4、ros-jazzy-rc-genicam-api

报错a:

```Shell
-- Found Curses: /usr/lib/riscv64-linux-gnu/libncurses.so
-- Detected architecture riscv64
CMake Error at genicam/GenicamConfig.cmake:82 (message):
  Unknown architecture
Call Stack (most recent call first):
  CMakeLists.txt:75 (find_package)
```

报错b:

```Shell
-- Detected architecture riscv64
-- Adding imported: genicam::libGCBase_gcc_xxx
-- Adding imported: genicam::libGenApi_gcc_xxx
-- Adding imported: genicam::liblog4cpp_gcc_xxx
-- Adding imported: genicam::libLog_gcc_xxx
-- Adding imported: genicam::libMathParser_gcc_xxx
-- Adding imported: genicam::libNodeMapData_gcc_xxx
-- Adding imported: genicam::libXmlParser_gcc_xxx
-- Detected architecture riscv64
CMake Error at baumer/CMakeLists.txt:86 (message):
  Unknown architecture
```

报错c:

```Shell
- RC_PROJECT_VERSION: 2.6.5
-- RC_PACKAGE_VERSION: 2.6.5
-- Detected architecture riscv64
-- Adding imported: genicam::libGCBase_gcc_xxx
-- Adding imported: genicam::libGenApi_gcc_xxx
-- Adding imported: genicam::liblog4cpp_gcc_xxx
-- Adding imported: genicam::libLog_gcc_xxx
-- Adding imported: genicam::libMathParser_gcc_xxx
-- Adding imported: genicam::libNodeMapData_gcc_xxx
-- Adding imported: genicam::libXmlParser_gcc_xxx
-- Detected architecture riscv64
-- Detected architecture riscv64
CMake Error at genicam/CMakeLists.txt:90 (message):
  Unknown architecture
```

含有直接依赖的动态库，还未找到其源码。

#### 5、ros-jazzy-lanelet2-maps

问题：

```Shell
CMake Error at /opt/ros/jazzy/share/mrt_cmake_modules/cmake/Modules/GatherDeps.cmake:24 (message):
  Gather depenencies failed: Traceback (most recent call last):

    File "/opt/ros/jazzy/share/mrt_cmake_modules/cmake/../scripts/generate_cmake_dependency_file.py", line 366, in <module>
      main(sys.argv[1], sys.argv[2], sys.argv[3])
    File "/opt/ros/jazzy/share/mrt_cmake_modules/cmake/../scripts/generate_cmake_dependency_file.py", line 296, in main
      cmakeVarData = readPackageCMakeData(rosDepYamlFileName)
                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    File "/opt/ros/jazzy/share/mrt_cmake_modules/cmake/../scripts/generate_cmake_dependency_file.py", line 142, in readPackageCMakeData
      distribution = OsDetect().detect_os()[2]
                     ^^^^^^^^^^^^^^^^^^^^^^
    File "/usr/lib/python3/dist-packages/rospkg/os_detect.py", line 609, in detect_os
      raise OsNotDetected("Could not detect OS, tried %s" % attempted)

  rospkg.os_detect.OsNotDetected: Could not detect OS, tried ['zorin',
  'windows', 'nixos', 'clearlinux', 'ubuntu', 'slackware', 'rocky', 'rhel',
  'raspbian', 'qnx', 'pop', 'osx', 'sailfishos', 'tizen', 'conda', 'oracle',
  'opensuse', 'opensuse', 'opensuse', 'opensuse', 'opensuse', 'openembedded',
  'neon', 'mx', 'mint', 'linaro', 'gentoo', 'funtoo', 'freebsd',
  'fedora-asahi', 'fedora', 'elementary', 'elementary', 'debian', 'cygwin',
  'euleros', 'centos', 'manjaro', 'buildroot', 'arch', 'amazon', 'alpine',
  'almalinux']

Call Stack (most recent call first):
  CMakeLists.txt:11 (include)
```

解决：

```Shell
export ROS_OS_OVERRIDE=Linux:6.6.36:noble
```

python3-rospkg-modules包需要修改以支持我们的系统。

#### 6、python3-rospkg-modules的更改

触发的编译失败：

```Shell
ros-jazzy-lanelet2-core、ros-jazzy-depthai
```

解决：见https://gitlab.dc.com:8443/bianbu/ros/python3-rospkg-modules-bianbu/-/tree/packaging/bianbu-24.04/debian

#### 7、ros-jazzy-lely-core-libraries

报错a:

```Shell
/home/zq-ros2/ros2deb-try/ros-jazzy-lely-core-libraries/ros-jazzy-lely-core-libraries-0.2.9/.obj-riscv64-linux-gnu/upstream/src/util/mkjmp.c:61:2: error: #error Unsupported compiler or architecture for mkjmp()/sigmkjmp().
   61 | #error Unsupported compiler or architecture for mkjmp()/sigmkjmp().
      |  ^~~~~
/home/zq-ros2/ros2deb-try/ros-jazzy-lely-core-libraries/ros-jazzy-lely-core-libraries-0.2.9/.obj-riscv64-linux-gnu/upstream/src/util/mkjmp.c: In function 'ctx_init':
/home/zq-ros2/ros2deb-try/ros-jazzy-lely-core-libraries/ros-jazzy-lely-core-libraries-0.2.9/.obj-riscv64-linux-gnu/upstream/src/util/mkjmp.c:135:9: error: implicit declaration of function 'MKJMP_SET_SP' [-Werror=implicit-function-declaration]
  135 |         MKJMP_SET_SP(sp, size);
```

解决a：

修改ros-jazzy-lely-core-libraries-0.2.9/.obj-riscv64-linux-gnu/upstream/src/util/mkjmp.c

如下添加riscv的支持，暂时在编译时添加，源代码来自上游项目。

```C++
#endif
#include <lely/util/mkjmp.h>

#include <stdlib.h>

#if defined(__clang__) || defined(__GNUC__)
#if defined(__arm__) || defined(__aarch64__)
#define MKJMP_SET_SP(sp, size) \
    __asm__("mov     sp, %0" ::"r"((char *)(sp) + (size)))
#elif defined(__i386__)
#define MKJMP_SET_SP(sp, size) \
    __asm__("movl    %0, %%esp" ::"r"((char *)(sp) + (size)))
#elif defined(__x86_64__)
#define MKJMP_SET_SP(sp, size) \
    __asm__("movq    %0, %%rsp" ::"r"((char *)(sp) + (size)))
#elif defined(__riscv)#define MKJMP_SET_SP(sp, size) \
    __asm__("move sp, %0" ::"r"((char *)(sp) + (size)))
```

上游源码已经修改：https://gitlab.com/lely_industries/lely-core/-/commit/637de61625c9e8af2b9598109d6fb0e3a2829ce6

解决：

ros-jazzy-lely-core-libraries/CMakeLists.txt如下修改：

```Shell
ExternalProject_Add(upstr_lely_core_libraries    # Name for custom target
  #--Download step--------------
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/upstream
  INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"  # Installation prefix
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build
  GIT_REPOSITORY https://gitlab.com/lely_industries/lely-core.git
  # GIT_TAG b63a0b6f79d3ea91dc221724b42dae49894449fc
  GIT_TAG master # 改为master分支
  TIMEOUT 60
  #UPDATE step apply patch to fix dcf-tools install
  UPDATE_COMMAND
  COMMAND git reset --hard
  COMMAND git apply --whitespace=fix --reject ${CMAKE_CURRENT_SOURCE_DIR}/patches/0001-Fix-dcf-tools.patch
  #CONFIGURE step execute autoreconf and configure
  CONFIGURE_COMMAND autoreconf -i <SOURCE_DIR>
  COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-cython --disable-doc --disable-tests --disable-static --disable-diag
  #BUILD STEP execute make
  BUILD_COMMAND $(MAKE) -C ${CMAKE_CURRENT_BINARY_DIR}/build
  #INSTALL STEP do nothing as we install in main
  INSTALL_COMMAND ""
)
```

#### 8、ros-jazzy-zenoh-bridge-dds

报错a:

```Bash
make[4]: Entering directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
cd /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /<<PKGBUILDDIR>> /<<PKGBUILDDIR>> /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/CMakeFiles/build_crate.dir/DependInfo.cmake "--color="
make[4]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make  -f CMakeFiles/build_crate.dir/build.make CMakeFiles/build_crate.dir/build
make[4]: Entering directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
[100%] Generating target/release/zenoh-bridge-dds
cargo build --release --manifest-path /<<PKGBUILDDIR>>/zenoh-bridge-dds/Cargo.toml --target-dir /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/target
warning: file `/<<PKGBUILDDIR>>/zenoh-bridge-dds/src/main.rs` found to be present in multiple build targets:
  * `bin` target `zenoh-bridge-dds`
  * `bin` target `dzd`
    Updating crates.io index
    Updating git repository `https://github.com/eclipse-zenoh/zenoh`
error: failed to get `zenoh` as a dependency of package `zenoh-bridge-dds v0.5.0-dev (/<<PKGBUILDDIR>>/zenoh-bridge-dds)`

Caused by:
  failed to load source for dependency `zenoh`

Caused by:
  Unable to update https://github.com/eclipse-zenoh/zenoh#000a2ba4

Caused by:
  failed to create directory `/sbuild-nonexistent/.cargo/git/db/zenoh-cc237f2570fab813`

Caused by:
  Permission denied (os error 13)
make[4]: *** [CMakeFiles/build_crate.dir/build.make:81: target/release/zenoh-bridge-dds] Error 101
make[4]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make[3]: *** [CMakeFiles/Makefile2:140: CMakeFiles/build_crate.dir/all] Error 2
make[3]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make[2]: *** [Makefile:149: all] Error 2
make[2]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
dh_auto_build: error: cd .obj-riscv64-linux-gnu && make -j1 VERBOSE=1 returned exit code 2
make[1]: *** [debian/rules:44: override_dh_auto_build] Error 255
make[1]: Leaving directory '/<<PKGBUILDDIR>>'
make: *** [debian/rules:27: build-arch] Error 2
dpkg-buildpackage: error: debian/rules build-arch subprocess returned exit status 2
--------------------------------------------------------------------------------
Build finished at 2024-10-15T23:14:07Z
```

网络或用户权限问题。

报错b:

```Shell
error: failed to run custom build command for `ring v0.16.20`

Caused by:
  process didn't exit successfully: `/home/zq-ros2/ros2deb-try/ros-jazzy-zenoh-bridge-dds/ros-jazzy-zenoh-bridge-dds-0.5.0/.obj-riscv64-linux-gnu/target/release/build/ring-a14ba72d10f69858/build-script-build` (exit status: 101)
  --- stderr
  thread 'main' panicked at /home/zq-ros2/.cargo/registry/src/index.crates.io-6f17d22bba15001f/ring-0.16.20/build.rs:358:10:
  called `Option::unwrap()` on a `None` value
  note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace
warning: build failed, waiting for other jobs to finish...
```

解决b:

~/.cargo/registry/src/index.crates.io-6f17d22bba15001f/ring-0.16.20/build.rs 35行后修改如下

```JavaScript
const ARM: &str = "arm";
const RISCV64: &str = "riscv64";
const RISCV32: &str = "riscv32";

#[rustfmt::skip]
const RING_SRCS: &[(&[&str], &str)] = &[
    (&[], "crypto/fipsmodule/aes/aes_nohw.c"),
    (&[], "crypto/fipsmodule/bn/montgomery.c"),
    (&[], "crypto/fipsmodule/bn/montgomery_inv.c"),
    (&[], "crypto/limbs/limbs.c"),
    (&[], "crypto/mem.c"),
    (&[], "crypto/poly1305/poly1305.c"),

    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/crypto.c"),
    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/curve25519/curve25519.c"),
    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/fipsmodule/ec/ecp_nistz.c"),
    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/fipsmodule/ec/ecp_nistz256.c"),
    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/fipsmodule/ec/gfp_p256.c"),
    (&[AARCH64, ARM, X86_64, X86, RISCV64, RISCV32], "crypto/fipsmodule/ec/gfp_p384.c"),
```

218行：

```JavaScript
const ASM_TARGETS: &[(&str, Option<&str>, Option<&str>)] = &[
    ("x86_64", Some("ios"), Some("macosx")),
    ("x86_64", Some("macos"), Some("macosx")),
    ("x86_64", Some(WINDOWS), Some("nasm")),
    ("x86_64", None, Some("elf")),
    ("aarch64", Some("ios"), Some("ios64")),
    ("aarch64", Some("macos"), Some("ios64")),
    ("aarch64", None, Some("linux64")),
    ("x86", Some(WINDOWS), Some("win32n")),
    ("x86", Some("ios"), Some("macosx")),
    ("x86", None, Some("elf")),
    ("arm", Some("ios"), Some("ios32")),
    ("arm", None, Some("linux32")),
    ("wasm32", None, None),
    ("riscv64", Some("linux"), None)
];
```

其它部分修改参考：

https://github.com/briansmith/ring/pull/1436/commits/1042da52736376c1f1f6bd222ef5f98e79f6740b

~/.cargo/registry/src/index.crates.io-6f17d22bba15001f/ring-0.16.20/src/rand.rs 219行附近修改如下：

```JavaScript
        #[cfg(target_arch = "x86_64")]
        const SYS_GETRANDOM: c_long = 318;

        #[cfg(target_arch = "riscv64")]
        const SYS_GETRANDOM: c_long = 257;
```

解决方案2：

```Shell
git clone --branch main --single-branch https://github.com/eclipse-zenoh/zenoh-plugin-dds.git
```

然后替换除debian文件夹外的所有文件

#### 000、非源码问题，本地编译直接通过

常见报错1：

```Shell
/usr/bin/cmake -E touch /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/gz_cmake_vendor-prefix/src/gz_cmake_vendor-stamp/gz_cmake_vendor-mkdir
[ 25%] Performing download step for 'gz_cmake_vendor'
cd /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/gz_cmake_vendor-prefix/src && /usr/bin/cmake -E rm -rf /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/gz_cmake_vendor-prefix/src/gz_cmake_vendor && /usr/bin/cmake -E make_directory /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/gz_cmake_vendor-prefix/src/gz_cmake_vendor && /usr/bin/vcs import . --input /<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/gz_cmake_vendor.repos --shallow --recursive --force
=== ./gz_cmake_vendor (git) ===
Invocation of command 'import' on client 'git' failed: AssertionError: Could not find 'git' executable (/usr/lib/python3/dist-packages/vcstool/clients/git.py:771)
make[4]: *** [CMakeFiles/gz_cmake_vendor.dir/build.make:104: gz_cmake_vendor-prefix/src/gz_cmake_vendor-stamp/gz_cmake_vendor-download] Error 1
make[4]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make[3]: *** [CMakeFiles/Makefile2:140: CMakeFiles/gz_cmake_vendor.dir/all] Error 2
make[3]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make[2]: *** [Makefile:149: all] Error 2
ros-jazzy-pybind11-json-vendor、、ros-jazzy-acado-vendor
ros-jazzy-gz-cmake-vendor、ros-jazzy-gz-dartsim-vendor、ros-jazzy-lttngpy
ros-jazzy-aws-sdk-cpp-vendor、ros-jazzy-gz-utils-vendor、ros-jazzy-rviz-ogre-vendor、
ros-jazzy-gz-tools-vendor
```

#### 9、ros-jazzy-gz-ogre-next-vendor

git clone --branch v2.3.3 --single-branch https://github.com/OGRECave/ogre-next.git

ros-jazzy-gz-ogre-next-vendor-0.0.5/CMakeLists.txt如下修改：

```Shell
ament_vendor(${PROJECT_NAME}
  # VCS_URL https://github.com/OGRECave/ogre-next.git
  VCS_URL /home/zq-ros2/gitup/ogre-next
  # VCS_VERSION v2.3.3
  VCS_VERSION bianbu
  CMAKE_ARGS
    -DOGRE_STATIC:BOOL=OFF
    -DOGRE_BUILD_COMPONENT_OVERLAY:BOOL=ON
    -DOGRE_BUILD_COMPONENT_PLANAR_REFLECTIONS:BOOL=TRUE
    -DOGRE_BUILD_PLUGIN_PFX:BOOL=ON
    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS:BOOL=TRUE
    -DOGRE_BUILD_RENDERSYSTEM_GLES2:BOOL=FALSE
    -DOGRE_BUILD_RENDERSYSTEM_VULKAN:BOOL=TRUE
    -DOGRE_BUILD_SAMPLES2:BOOL=FALSE
    -DOGRE_CONFIG_UNIX_NO_X11:BOOL=FALSE
    -DOGRE_FULL_RPATH:BOOL=FALSE
    -DOGRE_GLSUPPORT_USE_EGL_HEADLESS:BOOL=TRUE
    -DOGRE_GLSUPPORT_USE_GLX:BOOL=TRUE
    -DOGRE_INSTALL_SAMPLES:BOOL=FALSE
    -DOGRE_USE_NEW_PROJECT_NAME:BOOL=ON
    -DOGRE_VULKAN_WINDOW_NULL:BOOL=TRUE
    -DOGRE_VULKAN_WINDOW_XCB:BOOL=ON
    ${EXTRA_CMAKE_FLAGS}
  GLOBAL_HOOK
  PATCHES
    patches/0001-Fix-incomplete-vulkan-linkage.patch
)
```

报错a：

```Shell
/home/zq-ros2/ros2deb-try/ros-jazzy-gz-ogre-next-vendor/ros-jazzy-gz-ogre-next-vendor-0.0.5/.obj-riscv64-linux-gnu/gz_ogre_next_vendor-prefix/src/gz_ogre_next_vendor/OgreMain/include/OgreStringConverter.h: At global scope: /home/zq-ros2/ros2deb-try/ros-jazzy-gz-ogre-next-vendor/ros-jazzy-gz-ogre-next-vendor-0.0.5/.obj-riscv64-linux-gnu/gz_ogre_next_vendor-prefix/src/gz_ogre_next_vendor/OgreMain/include/OgreStringConverter.h:115:23: error: ‘static Ogre::String Ogre::StringConverter::toString(size_t, short unsigned int, char, std::ios_base::fmtflags)’ cannot be overloaded with ‘static Ogre::String Ogre::StringConverter::toString(long unsigned int, short unsigned int, char, std::ios_base::fmtflags)’   115 |         static String toString(size_t val,       |                       ^~~~~~~~ /home/zq-ros2/ros2deb-try/ros-jazzy-gz-ogre-next-vendor/ros-jazzy-gz-ogre-next-vendor-0.0.5/.obj-riscv64-linux-gnu/gz_ogre_next_vendor-prefix/src/gz_ogre_next_vendor/OgreMain/include/OgreStringConverter.h:110:23: note: previous declaration ‘static Ogre::String Ogre::StringConverter::toString(long unsigned int, short unsigned int, char, std::ios_base::fmtflags)’   110 |         static String toString(unsigned long va
```

解决a:

ogre-next/OgreMain/include/OgrePlatform.h 77 行

```Shell
/* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_X64) || defined(_M_AMD64) \
 || defined(__ppc64__) \
 || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) \
 || defined(__mips64) || defined(__mips64_) \
 || defined(__alpha__) || defined(__ia64__) || defined(__e2k__) || defined(__s390__) || defined(__s390x__) || defined(__riscv64__)
#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_64
#else
#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_64
#endif
```

报错b:

```Shell
ity.h:34,
                 from /home/zq-ros2/ros2deb-try/ros-jazzy-gz-ogre-next-vendor/ros-jazzy-gz-ogre-next-vendor-0.0.5/.obj-riscv64-linux-gnu/gz_ogre_next_vendor-prefix/src/gz_ogre_next_vendor/OgreMain/src/OgreAnimation.cpp:32:
/home/zq-ros2/ros2deb-try/ros-jazzy-gz-ogre-next-vendor/ros-jazzy-gz-ogre-next-vendor-0.0.5/.obj-riscv64-linux-gnu/gz_ogre_next_vendor-prefix/src/gz_ogre_next_vendor/OgreMain/include/Math/Array/OgreArrayConfig.h:188:10: error: #error "Unknown platform or platform not supported for SIMD. Build Ogre without OGRE_USE_SIMD"
  188 |         #error "Unknown platform or platform not supported for SIMD. Build Ogre without OGRE_USE_SIMD"
      |          ^~~~~
```

解决b

ros-jazzy-gz-ogre-next-vendor-0.0.5/CMakeLists.txt，增加riscv的选项

```Shell
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64.*|aarch64.*|AARCH64.*|arm64.*|ARM64.*|arm.*|ARM.*)")
  # Flags for non SIMD architectures
  # https://github.com/gazebo-forks/ogre-2.3-release/blob/960bb19664879282b979e4ed8cb3ce278c875bdb/debian/rules#L21-L28
  set(EXTRA_CMAKE_FLAGS -DOGRE_SIMD_NEON:BOOL=FALSE  -DOGRE_SIMD_SSE2:BOOL=FALSE)
endif()
```

ros-jazzy-gz-ogre-next-vendor-0.0.5/debian/rules，如下修改

```Shell
override_dh_shlibdeps:
    # In case we're installing to a non-standard location, look for a setup.sh
    # in the install tree and source it.  It will set things like
    # CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
    if [ -f "/opt/ros/jazzy/setup.sh" ]; then . "/opt/ros/jazzy/setup.sh"; fi && \
    dh_shlibdeps -l$(CURDIR)/debian/ros-jazzy-gz-ogre-next-vendor//opt/ros/jazzy/lib/:$(CURDIR)/debian/ros-jazzy-gz-ogre-next-vendor//opt/ros/jazzy/opt/gz_ogre_next_vendor/lib/:/usr/lib/riscv64-linux-gnu/
```

#### 10、ros-jazzy-mimick-vendor

报错：不支持的架构

解决：

定位ros-jazzy-mimick-vendor/ros-jazzy-mimick-vendor-0.6.2/CMakeLists.txt

修改为：

```Shell
cmake_minimum_required(VERSION 3.10)
project(mimick_vendor)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_vendor_package REQUIRED)

ament_vendor(mimick_vendor
  VCS_URL https://github.com/ziyao233/Mimick.git
  VCS_VERSION 90d02296025f38da2e33c67b02b7fa0c7c7d460c
)
```

#### 11、ros-jazzy-ecl-io-1.2.0

报错a：

```Shell
e/zq-muse2/ros2deb/ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/src/lib/poll.cpp
In file included from /home/zq-muse2/ros2deb/ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/src/lib/../../include/ecl/io/sockets.hpp:28,
                 from /home/zq-muse2/ros2deb/ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/src/lib/../../include/ecl/io/poll.hpp:19,
                 from /home/zq-muse2/ros2deb/ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/src/lib/poll.cpp:13:
/opt/ros/jazzy/include/ecl/errors/handlers.hpp:73:22: error: ‘virtual void ecl::Error::operator=(const ecl::ErrorFlag&)’ was hidden [-Werror=overloaded-virtual=]
   73 |         virtual void operator=(const ErrorFlag &error) { error_flag = error; }
      |                      ^~~~~~~~
/home/zq-muse2/ros2deb/ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/src/lib/../../include/ecl/io/sockets.hpp:68:21: note:   by ‘ecl::SocketError::operator=’
   68 | class ecl_io_PUBLIC SocketError : public Error
      |                     ^~~~~~~~~~~
cc1plus: all warnings being treated as errors
```

重载虚函数的警告，这个警告被视为错误

解决a: 关闭警告

ros-jazzy-ecl-io/ros-jazzy-ecl-io-1.2.0/CMakeLists.txt

添加：

```Shell
cmake_minimum_required(VERSION 3.5)
project(ecl_io)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual") # add
```

#### 12、ros-jazzy-ecl-time-lite

与11报错相同

解决：

ros-jazzy-ecl-time-lite/ros-jazzy-ecl-time-lite-1.2.0/CMakeLists.txt

如下添加：

```Shell
cmake_minimum_required(VERSION 3.5)
project(ecl_time_lite)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")
```

#### 13、ros-jazzy-canopen-tests

报错:

```Shell
-- Found ament_cmake: 2.5.2 (/opt/ros/jazzy/share/ament_cmake/cmake)
Traceback (most recent call last):
  File "/opt/ros/jazzy/share/ament_cmake_core/cmake/package_templates/templates_2_cmake.py", line 21, in <module>
    from ament_package.templates import get_environment_hook_template_path
ModuleNotFoundError: No module named 'ament_package'
CMake Error at /opt/ros/jazzy/share/ament_cmake_core/cmake/ament_cmake_package_templates-extras.cmake:41 (message):
  execute_process(/usr/bin/python3
  /opt/ros/jazzy/share/ament_cmake_core/cmake/package_templates/templates_2_cmake.py
  /home/zq-muse2/ros2deb/ros-jazzy-canopen-tests/ros-jazzy-canopen-tests-0.2.9/.obj-riscv64-linux-gnu/ament_cmake_package_templates/templates.cmake)
  returned error code 1
Call Stack (most recent call first):
  /opt/ros/jazzy/share/ament_cmake_core/cmake/ament_cmake_coreConfig.cmake:41 (include)
  /opt/ros/jazzy/share/ament_cmake/cmake/ament_cmake_export_dependencies-extras.cmake:15 (find_package)
  /opt/ros/jazzy/share/ament_cmake/cmake/ament_cmakeConfig.cmake:41 (include)
  CMakeLists.txt:9 (find_package)
```

jazzy-lely-core-libraries的问题，fixed

#### 14、ros-jazzy-ortools-vendor

报错：

```Shell
/home/zq-muse2/ros2deb/ros-jazzy-ortools-vendor/ros-jazzy-ortools-vendor-9.9.0/.obj-riscv64-linux-gnu/ortools_vendor-prefix/src/ortools_vendor/ortools/constraint_solver/constraint_solveri.h:251:16: error: cast from ‘void*’ to ‘uint32_t’ {aka ‘unsigned int’} loses precision [-fpermissive]
  251 |   return Hash1(reinterpret_cast<uint32_t>(ptr));
```

解决：

https://github.com/google/or-tools/tree/v9.9

or-tools/ortools/constraint_solver/constraint_solveri.h

```Shell
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || \
    defined(__aarch64__) || (defined(_MIPS_SZPTR) && (_MIPS_SZPTR == 64))
  return Hash1(reinterpret_cast<uint64_t>(ptr));
#else
  return Hash1(reinterpret_cast<uint64_t>(ptr));
  // return Hash1(reinterpret_cast<uint32_t>(ptr));
#endif
}
```

#### 15、ros-jazzy-ecl-time

报错及解决方法与12相同

#### 16、ros-jazzy-ecl-ipc

报错：

```Shell
/opt/ros/jazzy/include/ecl/errors/handlers.hpp:73:22: error: ‘virtual void ecl::Error::operator=(const ecl::ErrorFlag&)’ was hidden [-Werror=overloaded-virtual=]
   73 |         virtual void operator=(const ErrorFlag &error) { error_flag = error; }
      |                      ^~~~~~~~
```

同12，CMakelist文件添加

```Shell
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual")
```

#### 17、ros-jazzy-ecl-streams

```Shell
/opt/ros/jazzy/include/ecl/errors/handlers.hpp:73:22: error: ‘virtual void ecl::Error::operator=(const ecl::ErrorFlag&)’ was hidden [-Werror=overloaded-virtual=]
   73 |         virtual void operator=(const ErrorFlag &error) { error_flag = error; }
      |                      ^~~~~~~~
```

同16

未加gitlab

#### 18、ros-jazzy-kobuki-core

```Shell
               from /home/zq-muse2/ros2deb/ros-jazzy-kobuki-core/ros-jazzy-kobuki-core-1.4.0/src/driver/command.cpp:14:
/opt/ros/jazzy/include/ecl/errors/handlers.hpp:73:22: error: ‘virtual void ecl::Error::operator=(const ecl::ErrorFlag&)’ was hidden [-Werror=overloaded-virtual=]
   73 |         virtual void operator=(const ErrorFlag &error) { error_flag = error; }
```

同16

未加gitlab

#### 001、rclcpp依赖不满足问题

有问题的依赖路径：

```Shell
rclcpp->ros-jazzy-libstatistics-collector->ros-jazzy-rcl -> ros-jazzy-rmw-implementation
-> ros-jazzy-rmw-connextdds -> ros-jazzy-rmw-connextdds-common ->
ros-jazzy-rti-connext-dds-cmake-module ->
rti-connext-dds-6.0.1
```

依赖ros-jazzy-rmw-connextdds的有：[domain_bridge](https://index.ros.org/p/domain_bridge) ，[domain_bridge](https://index.ros.org/p/domain_bridge)不被任何包依赖

解决：

去掉ros-jazzy-rmw-implementation依赖的ros-jazzy-rmw-connextdds

#### 19、ros-jazzy-performance-test

ros-jazzy-performance-test-2.0.0/CMakeLists.txt 124行如下增加

```Shell
  if($ENV{ROS_DISTRO} MATCHES "jazzy")
    set(PERFORMANCE_TEST_RCLCPP_WAITSET_ENABLED ON)
    set(PERFORMANCE_TEST_RCLCPP_SSTE_ENABLED ON)
  endif()
```

503行如下增加：

```Shell
  elseif($ENV{ROS_DISTRO} MATCHES "jazzy")
    rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
    target_link_libraries(${EXE_NAME} ${cpp_typesupport_target})
```

新版本无问题

#### 20、ros-jazzy-canopen-core

报错：X86报相同错误

```Shell
usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::end()'
/usr/bin/ld: libdevice_container.so: undefined reference to `vtable for YAML::Exception'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::set_null()'
/usr/bin/ld: libdevice_container.so: undefined reference to `vtable for YAML::BadConversion'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::memory::create_node()'
/usr/bin/ld: libdevice_container.so: undefined reference to `typeinfo for YAML::BadSubscript'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::convert_to_map(std::shared_ptr<YAML::detail::memory_holder> const&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::BadConversion::~BadConversion()'
/usr/bin/ld: libdevice_container.so: undefined reference to `vtable for YAML::RepresentationException'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::mark_defined()'
/usr/bin/ld: libdevice_container.so: undefined reference to `typeinfo for YAML::InvalidNode'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::set_scalar(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::BadSubscript::~BadSubscript()'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::insert_map_pair(YAML::detail::node&, YAML::detail::node&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::Dump[abi:cxx11](YAML::Node const&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::node_data::begin()'
/usr/bin/ld: libdevice_container.so: undefined reference to `vtable for YAML::BadSubscript'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::detail::memory_holder::merge(YAML::detail::memory_holder&)'
/usr/bin/ld: libnode_canopen_driver.so: undefined reference to `YAML::Load(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::LoadFile(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: libdevice_container.so: undefined reference to `YAML::InvalidNode::~InvalidNode()'
/usr/bin/ld: libdevice_container.so: undefined reference to `vtable for YAML::InvalidNode'
/usr/bin/ld: libdevice_container.so: undefined reference to `typeinfo for YAML::BadConversion'
collect2: error: ld returned 1 exit status
make[4]: *** [CMakeFiles/device_container_node.dir/build.make:210: device_container_node] Error 1
make[4]: Leaving directory '/home/zq/pkgs/ros-jazzy-canopen-core/ros-jazzy-canopen-core-0.2.9/.obj-x86_64-linux-gnu'
make[3]: *** [CMakeFiles/Makefile2:295: CMakeFiles/device_container_node.dir/all] Error 2
make[3]: Leaving directory '/home/zq/pkgs/ros-jazzy-canopen-core/ros-jazzy-canopen-core-0.2.9/.obj-x86_64-linux-gnu'
make[2]: *** [Makefile:149: all] Error 2
make[2]: Leaving directory '/home/zq/pkgs/ros-jazzy-canopen-core/ros-jazzy-canopen-core-0.2.9/.obj-x86_64-linux-gnu'
dh_auto_build: error: cd .obj-x86_64-linux-gnu && make -j1 VERBOSE=1 returned exit code 2
make[1]: *** [debian/rules:44: override_dh_auto_build] Error 255
make[1]: Leaving directory '/home/zq/pkgs/ros-jazzy-canopen-core/ros-jazzy-canopen-core-0.2.9'
make: *** [debian/rules:27: build-arch] Error 2
dpkg-buildpackage: error: debian/rules build-arch subprocess returned exit status 2
```

解决：

```Shell
git clone --recurse-submodules https://github.com/ros-industrial/ros2_canopen.git
```

使用相应的最新文件进行替换

TODO ，更新源码包

#### 21、ros-jazzy-rmf-traffic-editor-test-maps

本地编译直接通过

CI报错：

```Shell
calculating levels relative to L1
level L1 scale: 0.060789897520256944
ERROR::building_map_model_downloader.get_crowdsim_models():78: Could not get crowd_sim models, error: 'crowd_sim'. Ignore models in crowd_sim...
Cache directory does not exist! Creating it: /sbuild-nonexistent/.pit_crew
Traceback (most recent call last):
  File "/opt/ros/jazzy/lib/rmf_building_map_tools/building_map_model_downloader", line 33, in <module>
    sys.exit(load_entry_point('rmf-building-map-tools==1.9.1', 'console_scripts', 'building_map_model_downloader')())
             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/opt/ros/jazzy/lib/python3.12/site-packages/building_map_model_downloader/building_map_model_downloader.py", line 217, in main
    download_models(
  File "/opt/ros/jazzy/lib/python3.12/site-packages/building_map_model_downloader/building_map_model_downloader.py", line 134, in download_models
    missing_models = pit_crew.get_missing_models(
                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/opt/ros/jazzy/lib/python3.12/site-packages/pit_crew/pit_crew.py", line 153, in get_missing_models
    cache = build_and_update_cache(cache_file_path=cache_file_path,
            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/opt/ros/jazzy/lib/python3.12/site-packages/pit_crew/pit_crew.py", line 817, in build_and_update_cache
    os.makedirs(dir_name, exist_ok=True)
  File "<frozen os>", line 215, in makedirs
  File "<frozen os>", line 225, in makedirs
PermissionError: [Errno 13] Permission denied: '/sbuild-nonexistent'
[ros2run]: Process exited with failure 1
make[4]: *** [CMakeFiles/generate_door_madness.dir/build.make:77: maps/door_madness/door_madness.world] Error 1
make[4]: *** Deleting file 'maps/door_madness/door_madness.world'
make[4]: Leaving directory '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu'
make[3]: *** [CMakeFiles/Makefile2:140: CMakeFiles/generate_door_madness.dir/all] Error 2
```

#### 22、ros-jazzy-plotjuggler

ros-jazzy-plotjuggler-3.9.2/3rdparty/backward-cpp/backward.hpp 3948行修改为

```JavaScript
#pragma message ":/ sorry, ain't know no nothing none not of your architecture!"
```

#### 23、ros-jazzy-cmake-generate-parameter-module-example

报错：

```Shell
Traceback (most recent call last):
  File "/opt/ros/jazzy/bin/generate_parameter_library_python", line 33, in <module>
    sys.exit(load_entry_point('generate-parameter-library-py==0.3.8', 'console_scripts', 'generate_parameter_library_python')())
             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/opt/ros/jazzy/lib/python3.12/site-packages/generate_parameter_library_py/generate_python_module.py", line 66, in main
    run(output_file, yaml_file, validate_file)
  File "/opt/ros/jazzy/lib/python3.12/site-packages/generate_parameter_library_py/generate_python_module.py", line 44, in run
    os.makedirs(output_dir)
  File "<frozen os>", line 225, in makedirs
PermissionError: [Errno 13] Permission denied: '/opt/ros/jazzy/lib/python3.12/site-packages/cmake_generate_parameter_module_example'
make[4]: *** [CMakeFiles/admittance_parameters.dir/build.make:74：/opt/ros/jazzy/lib/python3.12/site-packages/cmake_generate_parameter_module_example/admittance_parameters.py] 错误 1
make[4]: 离开目录“/home/zq-ros2/ros2deb-try/ros-jazzy-cmake-generate-parameter-module-example/ros-jazzy-cmake-generate-parameter-module-example-0.3.8/.obj-riscv64-linux-gnu”
make[3]: *** [CMakeFiles/Makefile2:143：CMakeFiles/admittance_parameters.dir/all] 错误 2
make[3]: 离开目录“/home/zq-ros2/ros2deb-try/ros-jazzy-cmake-generate-parameter-module-example/ros-jazzy-cmake-generate-parameter-module-example-0.3.8/.obj-riscv64-linux-gnu”
make[2]: *** [Makefile:149：all] 错误 2
make[2]: 离开目录“/home/zq-ros2/ros2deb-try/ros-jazzy-cmake-generate-parameter-module-example/ros-jazzy-cmake-generate-parameter-module-example-0.3.8/.obj-riscv64-linux-gnu”
dh_auto_build: error: cd .obj-riscv64-linux-gnu && make -j1 VERBOSE=1 returned exit code 2
```

#### 24、ros-jazzy-avt-vimba-camera

```Shell
CMake Error at CMakeLists.txt:46 (message):
  [libvimba]: Architecture unknown not suported.  Exiting...
Call Stack (most recent call first):
  CMakeLists.txt:56 (add_dependencies_and_linkings)
```

动态库不支持，无依赖包，delay

25、ros-jazzy-spinnaker-camera-driver

报错：

```Shell
Traceback (most recent call last):
  File "/home/zq-ros2/ros2deb-try/ros-jazzy-spinnaker-camera-driver/ros-jazzy-spinnaker-camera-driver-2.0.20/cmake/download_spinnaker", line 203, in <module>
    my_arch = ARCHS[os_code_name][arch]
```

依赖于amd64deb，delay

#### 25、ros-jazzy-ros2-ouster

报错：

```Shell
CMake Error at /opt/ros/jazzy/share/pcl_conversions/cmake/ament_cmake_export_dependencies-extras.cmake:21 (find_package):
  By not providing "FindPCL.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "PCL", but
  CMake did not find one.

  Could not find a package configuration file provided by "PCL" with any of
  the following names:

    PCLConfig.cmake
    pcl-config.cmake

  Add the installation prefix of "PCL" to CMAKE_PREFIX_PATH or set "PCL_DIR"
  to a directory containing one of the above files.  If "PCL" provides a
  separate development package or SDK, be sure it has been installed.
Call Stack (most recent call first):
  /opt/ros/jazzy/share/pcl_conversions/cmake/pcl_conversionsConfig.cmake:41 (include)
  CMakeLists.txt:21 (find_package)
```

解决：

```Shell
sudo apt-get install libpcl-dev
```

TODO，提交源码包

#### 26、ros-jazzy-mrpt-libopengl

```Shell
Cloning into '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/3rdparty/nanogui/ext/nanovg'...
fatal: unable to access 'https://github.com/wjakob/nanovg/': Failed to connect to github.com port 443 after 129963 ms: Couldn't connect to server
fatal: clone of 'https://github.com/wjakob/nanovg' into submodule path '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/3rdparty/nanogui/ext/nanovg' failed
Failed to clone 'ext/nanovg'. Retry scheduled
Cloning into '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/3rdparty/nanogui/ext/nanovg'...
fatal: unable to access 'https://github.com/wjakob/nanovg/': GnuTLS recv error (-110): The TLS connection was non-properly terminated.
fatal: clone of 'https://github.com/wjakob/nanovg' into submodule path '/<<PKGBUILDDIR>>/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/3rdparty/nanogui/ext/nanovg' failed
Failed to clone 'ext/nanovg' a second time, aborting
fatal: Failed to recurse into submodule path '3rdparty/nanog
```

网络问题，在本地配置代理编译通过

#### 27、ros-jazzy-qpoases-vendor

ros-jazzy-qpoases-vendor-3.2.3/CMakeLists.txt 如下修改

```Shell
include(ExternalProject)
ExternalProject_Add(qpoases_3_2
  GIT_REPOSITORY https://github.com/coin-or/qpOASES.git  # 使用 Git 仓库地址
  GIT_TAG master
  BUILD_IN_SOURCE TRUE
  INSTALL_COMMAND ""
)
```

#### 28、ros-jazzy-nav2-mppi-controller

报错a：

```Shell
cc1plus: error: unknown cpu ‘generic’ for ‘-mtune’
cc1plus: error: unknown cpu ‘generic’ for ‘-mtune’
cc1plus: error: unknown cpu ‘generic’ for ‘-mtune’
```

解决a:

```Shell
add_compile_options(-O3 -finline-limit=10000000 -ffp-contract=fast -ffast-math -mtune=generic)
->
add_compile_options(-O3 -finline-limit=10000000 -ffp-contract=fast -ffast-math )
```

报错b:

```Shell
    -> enable_simd_interface<T, simd_return_type<requested_type>>
      |                                     ^~~~~~~~~~~~~~~~
/usr/include/xtensor/xstrided_view.hpp:686:68: error: template argument 2 is invalid
  686 |         -> enable_simd_interface<T, simd_return_type<requested_type>>
      |                                                                    ^~
/usr/include/xtensor/xstrided_view.hpp:686:12: error: invalid use of template-name ‘xt::xstrided_view<CT, S, L, FST>::enable_simd_interface’ without an argument list
  686 |         -> enable_simd_interface<T, simd_return_type<requested_type>>
      |            ^~~~~~~~~~~~~~~~~~~~~
/usr/include/xtensor/xstrided_view.hpp:258:15: note: ‘template<class CT, class S, xt::layout_type L, class FST> template<class T, class R> using xt::xstrided_view<CT, S, L, FST>::enable_simd_interface = std::enable_if_t<(xt::has_simd_interface<T>::value && (L != xt::layout_type::dynamic)), R>’ declared here
  258 |         using enable_simd_interface = std::enable_if_t<has_simd_interface<T>::value && L != layout_type::dynamic, R>;
      |               ^~~~~~~~~~~~~~~~~~~~~
/usr/include/xtensor/xstrided_view.hpp:686:33: error: expected initializer before ‘<’ token
  686 |         -> enable_simd_interface<T, simd_return_type<requested_type>>
      |                                 ^
/usr/include/xtensor/xstrided_view.hpp: At global scope:
/usr/include/xtensor/xstrided_view.hpp:686:37: error: ‘simd_return_type’ was not declared in this scope
  686 |         -> enable_simd_interface<T, simd_return_type<requested_type>>
```

解决b:

```Shell
# add_definitions(-DXTENSOR_DISABLE_XSIMD)
# add_definitions(-DXTENSOR_USE_XSIMD)

set(XTENSOR_USE_TBB 0)
set(XTENSOR_USE_OPENMP 0)
set(XTENSOR_USE_XSIMD 0)

# set(XTENSOR_DEFAULT_LAYOUT column_major)  # row_major, column_major
# set(XTENSOR_DEFAULT_TRAVERSAL row_major)  # row_major, column_major

find_package(ament_cmake REQUIRED)
# find_package(xsimd REQUIRED)
find_package(xtensor REQUIRED)
.....

foreach(lib IN LISTS libraries)
  target_compile_options(${lib} PUBLIC -fconcepts)
  target_include_directories(${lib} PUBLIC ${xsimd_INCLUDE_DIRS})  #  ${OpenMP_INCLUDE_DIRS}
  target_link_libraries(${lib} xtensor xtensor::optimize ) # xtensor::use_xsimd
  ament_target_dependencies(${lib} ${dependencies_pkgs})
endforeach()
```

#### 29、ros-jazzy-mrpt-libmaps

网络问题

#### 30、ros-jazzy-nmea-hardware-interface

报错：

```Shell
  24 | #include <hardware_interface/base_interface.hpp>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[4]: *** [CMakeFiles/nmea_hardware_interface.dir/build.make:79：CMakeFiles/nmea_hardware_interface.dir/src/gps_hardware_interface.cpp.o] 错误 1
make[4]: 离开目录“/home/zq-ros2/ros2deb-try/ros-jazzy-nmea-hardware-interface/ros-jazzy-nmea-hardware-interface-0.0.1/.obj-riscv64-linux-gnu”
make[3]: *** [CMakeFiles/Makefile2:142：CMakeFiles/nmea_hardware_interface.dir/all] 错误 2
make[3]: *** 正在等待未完成的任务....
In file included from /home/zq-ros2/ros2deb-try/ros-jazzy-nmea-hardware-interface/ros-jazzy-nmea-hardware-interface-0.0.1/src/geopose_publisher.cpp:15:
/home/zq-ros2/ros2deb-try/ros-jazzy-nmea-hardware-interface/ros-jazzy-nmea-hardware-interface-0.0.1/include/nmea_hardware_interface/geopose_publisher.hpp:22:10: fatal error: diagnostic_msgs/msg/diagnostic_array.hpp: 没有那个文件或目录
   22 | #include <diagnostic_msgs/msg/diagnostic_array.hpp>
```

解：

```Shell
git clone https://github.com/OUXT-Polaris/nmea_hardware_interface.git
```

替换目标文件夹除debian文件夹外的所有内容

ros-jazzy-nmea-hardware-interface-0.0.1/CMakeLists.txt 修改为：

```Shell
if($ENV{ROS_DISTRO} STREQUAL "humble")
  add_definitions(-DHUMBLE)
endif()

if($ENV{ROS_DISTRO} STREQUAL "jazzy")
  add_definitions(-DHUMBLE)
endif()
 error: no matching function for call to ‘nmea_hardware_interface::GeoPosePublisher::init(const std::string&)’
   46 |   auto ret = ControllerInterface::init(controller_name);
      |              ~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~
```

原因：函数定义变化

humble：

```C++
   virtual return_type init(
    const std::string & controller_name, const std::string & namespace_ = "",
    const rclcpp::NodeOptions & node_options =
      rclcpp::NodeOptions()
        .allow_undeclared_parameters(true)
        .automatically_declare_parameters_from_overrides(true));
```

jazzy：

```C++
  return_type init(
    const std::string & controller_name, const std::string & urdf, unsigned int cm_update_rate,
    const std::string & node_namespace, const rclcpp::NodeOptions & node_options);
```

解决：

ros-jazzy-nmea-hardware-interface-0.0.1/include/nmea_hardware_interface/geopose_publisher.hpp 如下修改：

```C++
  controller_interface::return_type init(
    const std::string & controller_name, const std::string & urdf = " ", unsigned int cm_update_rate = 20, const std::string & namespace_ = "",
    const rclcpp::NodeOptions & node_options =
      rclcpp::NodeOptions()
        .allow_undeclared_parameters(true)
        .automatically_declare_parameters_from_overrides(true)) ;
```

ros-jazzy-nmea-hardware-interface-0.0.1/src/geopose_publisher.cpp 如下修改：

```C++
controller_interface::return_type GeoPosePublisher::init(
  const std::string & controller_name, const std::string & urdf, unsigned int cm_update_rate, const std::string & namespace_,
  const rclcpp::NodeOptions & node_options)
{
  auto ret = ControllerInterface::init(controller_name, urdf, cm_update_rate, namespace_, node_options);
  if (ret != controller_interface::return_type::OK) {
    return ret;
  }
```

#### 31、ros-jazzy-mrpt-apps

```Shell
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:221:18: error: ‘CQtGlCanvasBase’ is not a member of ‘mrpt::gui’; did you mean ‘CGlCanvasBase’?

  221 |       mrpt::gui::CQtGlCanvasBase* gl = new mrpt::gui::CQtGlCanvasBase();
      |                  ^~~~~~~~~~~~~~~
      |                  CGlCanvasBase
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:221:35: error: ‘gl’ was not declared in this scope
  221 |       mrpt::gui::CQtGlCanvasBase* gl = new mrpt::gui::CQtGlCanvasBase();
      |                                   ^~
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:221:44: error: expected type-specifier
  221 |       mrpt::gui::CQtGlCanvasBase* gl = new mrpt::gui::CQtGlCanvasBase();
      |                                            ^~~~
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:236:18: error: ‘CQtGlCanvasBase’ is not a member of ‘mrpt::gui’; did you mean ‘CGlCanvasBase’?
  236 |       mrpt::gui::CQtGlCanvasBase* gl1 = new mrpt::gui::CQtGlCanvasBase();
      |                  ^~~~~~~~~~~~~~~
      |                  CGlCanvasBase
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:236:35: error: ‘gl1’ was not declared in this scope
  236 |       mrpt::gui::CQtGlCanvasBase* gl1 = new mrpt::gui::CQtGlCanvasBase();
      |                                   ^~~
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:236:45: error: expected type-specifier
  236 |       mrpt::gui::CQtGlCanvasBase* gl1 = new mrpt::gui::CQtGlCanvasBase();
      |                                             ^~~~
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:244:20: error: ‘CQtGlCanvasBase’ is not a member of ‘mrpt::gui’; did you mean ‘CGlCanvasBase’?
  244 |         mrpt::gui::CQtGlCanvasBase* gl2 = new mrpt::gui::CQtGlCanvasBase();
      |                    ^~~~~~~~~~~~~~~
      |                    CGlCanvasBase
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:244:37: error: ‘gl2’ was not declared in this scope
  244 |         mrpt::gui::CQtGlCanvasBase* gl2 = new mrpt::gui::CQtGlCanvasBase();
      |                                     ^~~
/home/zq-ros2/ros2deb-try/ros-jazzy-mrpt-apps/ros-jazzy-mrpt-apps-2.14.0/.obj-riscv64-linux-gnu/mrpt-build/src/mrpt/apps/robot-map-gui/gui/CMainWindow.cpp:244:47: error: expected type-specifier
  244 |         mrpt::gui::CQtGlCanvasBase* gl2 = new mrpt::gui::CQtGlCanvasBase();
```

#### 32、ros-jazzy-mrpt-sensor-gnns-nmea

依赖冲突:

```Shell
dpkg: 处理归档 /var/cache/apt/archives/ros-jazzy-mrpt2_2.13.5-1noble_riscv64.deb (--unpack)时出错：
 正试图覆盖 /opt/ros/jazzy/include/mrpt/apps/include/mrpt/apps/BaseAppDataSource.h，它同时被包含于软件包 ros-jazzy-mrpt-libapps 2.14.0-1noble
dpkg-deb: 错误: 粘贴 子进程被信号(断开的管道) 终止了
在处理时有错误发生：
```

#### 33、ros-jazzy-scenario-execution-rviz

报错

```Shell
<command-line>: error: "_FORTIFY_SOURCE" redefined [-Werror]
<command-line>: note: this is the location of the previous definition
<command-line>: error: "_FORTIFY_SOURCE" redefined [-Werror]
<command-line>: note: this is the location of the previous definition
<command-line>: error: "_FORTIFY_SOURCE" redefined [-Werror]
<command-line>: note: this is the location of the previous definition
<command-line>: error: "_FORTIFY_SOURCE" redefined [-Werror]
<command-line>: note: this is the location of the previous definition
```

解决：

ros-jazzy-scenario-execution-rviz-1.2.0/CMakeLists.txt

add_compile_options如下修改：

```CMake
add_compile_options(-Wall -Wextra -Werror -flto -fvisibility=hidden -z noexecstack -fPIC -Wl,-z,relro,-z,now -fstack-protector-strong)
```

#### ros-jazzy-popf

```C++
sudo apt install libfl-dev
```

### 未构建或构建失败包说明：

#### 1、ros-jazzy-mrpt-sensor-gnns-nmea

依赖冲突，见32，ubuntu24.04无此包

#### 2、ros-jazzy-mrpt2

ubuntu24.04无此包

#### 3、依赖gazebo的包

```Shell
ros-jazzy-dolly-ignition -> ros-jazzy-ros-ign-gazebo
ros-jazzy-gazebo-ros2-control -> ros-jazzy-gazebo-dev
ros-jazzy-turtlebot3-gazebo -> ros-jazzy-gazebo-ros-pkgs
ros-jazzy-velodyne-gazebo-plugins -> ros-jazzy-gazebo-dev
```

ros-jazzy-gazebo-dev、ros-jazzy-gazebo-ros-pkgs无源码包

#### 4、connextdds相关

```Shell
ros-jazzy-domain-bridge -> ros-jazzy-rmw-connextdds -> ros-jazzy-rmw-connextdds-common
-> ros-jazzy-rti-connext-dds-cmake-module -> rti-connext-dds-6.0.1

ros-jazzy-rmw-connextddsmicro -> ros-jazzy-rmw-connextdds-common
```

#### oom异常

从buildd发现building数量多于实际节点数量，且同一节点出现两次

[ros-jazzy-rtabmap-sync](https://buildd.bianbu.xyz/package.php?p=ros-jazzy-rtabmap-sync&suite=noble-ros) (45m, riscv-bianbu-noble-ros-16g-5)

查看所在pod，发现出现了重建容器现象

![img](https://spacemit.feishu.cn/space/api/box/stream/download/asynccode/?code=ZWYxOWE5MTIzM2ZjMWIzZjk1ZjE3MzllZTQyMWU2NmJfZFdtamlPbWxPS2VydmN0SlQ4cUZ2Z2E4VXhEZ1laa2NfVG9rZW46UmJqTGJrN3NTb1ZwcVp4MHp3amNxdXVmbkRiXzE3ODMzOTM0NjE6MTc4MzM5NzA2MV9WNA&add_watermark=true&scene_type=CCM)

pod事件

![img](https://spacemit.feishu.cn/space/api/box/stream/download/asynccode/?code=MzBmNzZhODMzNzZiMzhkN2YxNGZiMDZmMjVjMzRmOWJfSzlxaVJ1WUhlQ2lIRDdkdFlSTlhZYVVjSGVuUW16THZfVG9rZW46UnRQUWJGbHphbzUzdlh4ZXE5TmMxTHhHbkZiXzE3ODMzOTM0NjE6MTc4MzM5NzA2MV9WNA&add_watermark=true&scene_type=CCM)

node事件

![img](https://spacemit.feishu.cn/space/api/box/stream/download/asynccode/?code=MDNiZjEyMDU3MWYxNTAwNGYwOTZlZDE4MDY4OGUyN2ZfNkw5QWNzdWxtYWFJeTN4aVVRblQ2MTYyZlVndXRPS3ZfVG9rZW46QWx2V2JCbUJSb25ncmh4OFFmUGNGMnBHbmFkXzE3ODMzOTM0NjE6MTc4MzM5NzA2MV9WNA&add_watermark=true&scene_type=CCM)

跟踪Prometheus数据确认

![img](https://spacemit.feishu.cn/space/api/box/stream/download/asynccode/?code=ZDFiYjAxNmFhZGRkNDdkOTBiZmNlMzRjYzE1MTE5MDRfbmMxZkF4OXJ6dW0wRXpteUl2Um44ZEJZWkNpbGM4cjdfVG9rZW46QTk4b2I3MXRCb0lDc0F4bU9sZGNncTBUblhQXzE3ODMzOTM0NjE6MTc4MzM5NzA2MV9WNA&add_watermark=true&scene_type=CCM)

### humble构建问题

#### ros-humble-webots-ros2-driver

CMakeList.txt  34行，上传gitlab

#### ros-humble-mimick-vendor-0.2.8

CMakeList.txt 如下修改

```CMake
  include(ExternalProject)
  # set(mimick_version "de11f8377eb95f932a03707b583bf3d4ce5bd3e7")
  set(mimick_version "90d02296025f38da2e33c67b02b7fa0c7c7d460c")
  externalproject_add(mimick-${mimick_version}
    # GIT_REPOSITORY https://github.com/ros2/Mimick.git
    # GIT_TAG ${mimick_version}
    GIT_REPOSITORY https://github.com/ziyao233/Mimick.git
    GIT_TAG ${mimick_version}
    GIT_CONFIG advice.detachedHead=false
    # Suppress git update due to https://gitlab.kitware.com/cmake/cmake/-/issues/16419
    UPDATE_COMMAND ""
    TIMEOUT 6000
    ${cmake_commands}
    CMAKE_ARGS
      ${cmake_configure_args}
  )
```

#### ros-humble-ortools-vendor

CMakeList.txt 如下修改

```CMake
ament_vendor(ortools_vendor
  # VCS_URL https://gitee.com/chen_zhao_qi/or-tools.git
  # VCS_VERSION master
  VCS_URL https://gitee.com/chen_zhao_qi/or-tools.git
  VCS_VERSION master
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=Release
    -DBUILD_DEPS:BOOL=ON
    -DBUILD_SAMPLES:BOOL=OFF
    -DBUILD_EXAMPLES:BOOL=OFF
  PATCHES patches
)
```

#### ros-humble-reach

暂缓，不是重要依赖

#### ros-humble-caret-analyze-cpp-impl-0.5.0

uint64_t未被包含

```C++
#include <cstdint>
```

#### ros-humble-hash-library-vendor

rules文件添加：

```C++
-DFETCHCONTENT_FULLY_DISCONNECTED=OFF
override_dh_auto_configure:
    # In case we're installing to a non-standard location, look for a setup.sh
    # in the install tree and source it.  It will set things like
    # CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
    if [ -f "/opt/ros/jazzy/setup.sh" ]; then . "/opt/ros/jazzy/setup.sh"; fi && \
    dh_auto_configure -- \
        -DCMAKE_INSTALL_PREFIX="/opt/ros/jazzy" \
        -DAMENT_PREFIX_PATH="/opt/ros/jazzy" \
        -DCMAKE_PREFIX_PATH="/opt/ros/jazzy" \
        -DFETCHCONTENT_FULLY_DISCONNECTED=OFF \
        $(BUILD_TESTING_ARG)
```

#### ros-humble-rmw-implementation

control 文件中去掉 connextdds 的相关依赖

#### mrpt:

libwxgtk3.0-gtk3-dev  -> libwxgtk3.2-dev

#### ros-humble-mavros

libgeographic-dev -> libgeographiclib-dev

### humble 动态库文件架构相关失败

#### ros-humble-hebi-cpp-api

#### ros-humble-kortex-api

#### ros-humble-nao-meshes

#### ros-humble-pepper-meshes

#### ros-humble-qb-softhand-industry-driver