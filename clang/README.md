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

After installation, dependent projects should import the
`mega65libc` target like this:

~~~ cmake
find_package(mega65libc 0.1 REQUIRED)
target_link_libraries(<your-target> mega65libc::mega65libc)
~~~

