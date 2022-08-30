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

## Dependent CMake projects

`CMakeLists.txt` of a dependent project could look like this:
~~~ cmake
cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(myproject VERSION 0.1.0 LANGUAGES C)
find_package(mega65libc REQUIRED)

add_compile_options(-mcpu=mos65c02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token)

add_executable(main main.c)
target_link_libraries(main mega65libc::mega65libc)
set_target_properties(main PROPERTIES OUTPUT_NAME main.prg)
~~~
See more [here](https://github.com/llvm-mos/llvm-mos-sdk#developing-for-6502-with-cmake).

### CPM.cmake dependency manager

If using [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake),
`mega65libc` can be easily added to your project with:
~~~ cmake
CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY mega65/mega65-libc SOURCE_SUBDIR clang)
target_link_libraries(<mytarget> mega65libc)
~~~

