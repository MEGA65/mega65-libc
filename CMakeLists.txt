cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(mega65libc VERSION 0.3.0 LANGUAGES C ASM)

enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Let's set a rather loud warning level
set(CLANG_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wnull-dereference # warn if a null dereference is detected
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
    -Wpedantic # warn if non-standard C/C++ is used
    -Wno-c23-extensions
    -Wno-fixed-enum-extension
    -Wno-language-extension-token
)

install(FILES cmake/mega65libcConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mega65libc)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(tests)

