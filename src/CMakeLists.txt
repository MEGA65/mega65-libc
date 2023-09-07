set(assembler
    llvm/fileio.s
    llvm/dirent.s
    llvm/memory_asm.s)

set(objects
    conio.c
    debug.c
    fat32.c
    fcio.c
    hal.c
    math.c
    memory.c
    mouse.c
    random.c
    sdcard.c
    targets.c
    tests.c
    time.c)

set(headers
    ${CMAKE_SOURCE_DIR}/include/mega65/conio.h
    ${CMAKE_SOURCE_DIR}/include/mega65/debug.h
    ${CMAKE_SOURCE_DIR}/include/mega65/dirent.h
    ${CMAKE_SOURCE_DIR}/include/mega65/fileio.h
    ${CMAKE_SOURCE_DIR}/include/mega65/hal.h
    ${CMAKE_SOURCE_DIR}/include/mega65/math.h
    ${CMAKE_SOURCE_DIR}/include/mega65/memory.h
    ${CMAKE_SOURCE_DIR}/include/mega65/mouse.h
    ${CMAKE_SOURCE_DIR}/include/mega65/random.h
    ${CMAKE_SOURCE_DIR}/include/mega65/sdcard.h
    ${CMAKE_SOURCE_DIR}/include/mega65/targets.h
    ${CMAKE_SOURCE_DIR}/include/mega65/tests.h
    ${CMAKE_SOURCE_DIR}/include/mega65/time.h)

set_source_files_properties(${objects} PROPERTIES LANGUAGE C)
set_source_files_properties(${assembler} PROPERTIES LANGUAGE ASM)
set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

# mega65libc target
add_library(mega65libc ${objects} ${headers} ${assembler})
target_include_directories(mega65libc
    PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set_target_properties(mega65libc PROPERTIES PUBLIC_HEADER "${headers}")
target_compile_options(mega65libc PRIVATE -Os ${CLANG_WARNINGS})

# install target
include(GNUInstallDirs)
install(TARGETS mega65libc
    EXPORT mega65libc-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION include/mega65
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


