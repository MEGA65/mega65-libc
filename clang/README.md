# MEGA65 libc for the llvm-mos compiler

The llvm-mos libc target can be build and exported using
CMake. This is done according to the recommendations
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

The general CMake setup for llvm-mos projects should follow
[these instructions](https://github.com/llvm-mos/llvm-mos-sdk#developing-for-6502-with-cmake).
The mega65libc dependency is then added like this:
~~~ cmake
find_package(mega65libc REQUIRED)
target_link_libraries(<your-target> mega65libc::mega65libc)
~~~

