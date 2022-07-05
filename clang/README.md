# MEGA65 libc for the llvm-mos compiler

The llvm-mos libc target can be build and exported using
CMake. This is has been implemented according to recommendations
[here](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/).

## Building

1. Install [llvm-mos-sdk](https://github.com/llvm-mos/llvm-mos-sdk#getting-started).
2. Configure and install `mega65libc` with:
~~~ bash
cd mega65-libc/clang/
cmake -DCMAKE_PREFIX_PATH=<llvm-mos-sdk-install-prefix> .
make install
~~~

## Dependent projects

A dependent project's `CMakeLists.txt` should look like this:
~~~ cmake
cmake_minimum_required(VERSION 3.5)
project(myproject VERSION 0.1.0 LANGUAGES C)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
find_package(mega65libc REQUIRED)

add_executable(main main.c)
target_link_libraries(main mega65libc::mega65libc)
~~~
See more [here](https://github.com/llvm-mos/llvm-mos-sdk#developing-for-6502-with-cmake).

