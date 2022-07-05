#
# Build and export mega65libc target according to example given here:
#
#    https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
#
# To tell CMake where to find the llvm-mos compiler, either add it to your
# path, or specify it's location using:
#
#    cmake -DCMAKE_PREFIX_PATH=<arbitrary-install-directory>
#
# (see more at https://github.com/llvm-mos/llvm-mos-sdk#developing-for-6502-with-cmake)
#
# After installation, dependent projects should locate mega65libc using:
#
#    find_package(mega65libc 0.1 REQUIRED)
#    target_link_libraries(example mega65libc::mega65libc)
#

cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(mega65libc VERSION 0.1.0 LANGUAGES C)

# all source and header files
set(objects src/conio.c src/debug.c src/fat32.c src/hal.c src/memory.c src/mouse.c src/random.c src/sdcard.c src/targets.c src/tests.c src/time.c)
set(headers include/conio.h include/debug.h include/hal.h include/memory.h include/sdcard.h include/targets.h include/tests.h include/time.h)
set_source_files_properties(${objects} PROPERTIES LANGUAGE C)
set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

# mega65libc target
add_library(mega65libc ${objects} ${headers})
target_include_directories(mega65libc
    PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_compile_options(mega65libc PRIVATE -mcpu=mos65c02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token)

# install target
include(GNUInstallDirs)
install(TARGETS mega65libc
    EXPORT mega65libc-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# export mega65libc
install(EXPORT mega65libc-export
    FILE
    mega65libcTargets.cmake
    NAMESPACE
    mega65libc::
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/mega65libc
    )

# compile example prg file
add_executable(example example.c)
target_link_libraries(example mega65libc)
set_target_properties(example PROPERTIES OUTPUT_NAME example.prg)

