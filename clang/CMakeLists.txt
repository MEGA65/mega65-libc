cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
project(mega65libc VERSION 1.0.0 LANGUAGES C ASM)

# all source and header files
set(assembler src/fileio.s src/dirent.s)

set(objects ../cc65/src/conio.c ../cc65/src/debug.c ../cc65/src/fat32.c
    ../cc65/src/hal.c ../cc65/src/memory.c ../cc65/src/mouse.c ../cc65/src/random.c
    ../cc65/src/sdcard.c ../cc65/src/targets.c ../cc65/src/tests.c ../cc65/src/time.c)

set(headers ../cc65/include/conio.h ../cc65/include/debug.h ../cc65/include/hal.h
    ../cc65/include/memory.h ../cc65/include/mouse.h ../cc65/include/random.h ../cc65/include/sdcard.h ../cc65/include/targets.h
    ../cc65/include/tests.h ../cc65/include/time.h ../cc65/include/fileio.h ../cc65/include/dirent.h)

set_source_files_properties(${objects} PROPERTIES LANGUAGE C)
set_source_files_properties(${assembler} PROPERTIES LANGUAGE ASM)
set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

# mega65libc target
add_library(mega65libc ${objects} ${headers} ${assembler})
target_include_directories(mega65libc
    PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../cc65/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
set_target_properties(mega65libc PROPERTIES PUBLIC_HEADER "${headers}")
target_compile_options(mega65libc PRIVATE -mcpu=mos65c02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token)

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
#add_executable(example src/example.c)
#target_link_libraries(example mega65libc)
#set_target_properties(example PROPERTIES OUTPUT_NAME example.prg EXCLUDE_FROM_ALL true)

