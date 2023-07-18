cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(mega65libc VERSION 0.3.0 LANGUAGES C ASM)

set(assembler
    src/llvm/fileio.s
    src/llvm/dirent.s)

set(objects
    src/conio.c
    src/debug.c
    src/fat32.c
    src/hal.c
    src/memory.c
    src/mouse.c
    src/random.c
    src/sdcard.c
    src/targets.c
    src/tests.c
    src/time.c)

set(headers
    include/conio.h
    include/debug.h
    include/dirent.h
    include/fileio.h
    include/hal.h
    include/memory.h
    include/mouse.h
    include/random.h
    include/sdcard.h
    include/targets.h
    include/tests.h
    include/time.h)

set_source_files_properties(${objects} PROPERTIES LANGUAGE C)
set_source_files_properties(${assembler} PROPERTIES LANGUAGE ASM)
set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

# mega65libc target
add_library(mega65libc ${objects} ${headers} ${assembler})
target_include_directories(mega65libc
    PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
set_target_properties(mega65libc PROPERTIES PUBLIC_HEADER "${headers}")
target_compile_options(mega65libc PRIVATE -mcpu=mos65c02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token -Wpedantic)

# install target
include(GNUInstallDirs)
install(TARGETS mega65libc
    EXPORT mega65libc-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION include
    )
install(FILES cmake/mega65libcConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mega65libc)

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
add_executable(time-test tests/time.c)
target_link_libraries(time-test mega65libc)
set_target_properties(time-test PROPERTIES OUTPUT_NAME time.prg EXCLUDE_FROM_ALL true)
