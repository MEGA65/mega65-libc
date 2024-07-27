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
    ${PROJECT_SOURCE_DIR}/include/mega65/conio.h
    ${PROJECT_SOURCE_DIR}/include/mega65/debug.h
    ${PROJECT_SOURCE_DIR}/include/mega65/dirent.h
    ${PROJECT_SOURCE_DIR}/include/mega65/fcio.h
    ${PROJECT_SOURCE_DIR}/include/mega65/fileio.h
    ${PROJECT_SOURCE_DIR}/include/mega65/hal.h
    ${PROJECT_SOURCE_DIR}/include/mega65/math.h
    ${PROJECT_SOURCE_DIR}/include/mega65/memory.h
    ${PROJECT_SOURCE_DIR}/include/mega65/mouse.h
    ${PROJECT_SOURCE_DIR}/include/mega65/random.h
    ${PROJECT_SOURCE_DIR}/include/mega65/sdcard.h
    ${PROJECT_SOURCE_DIR}/include/mega65/targets.h
    ${PROJECT_SOURCE_DIR}/include/mega65/tests.h
    ${PROJECT_SOURCE_DIR}/include/mega65/time.h)

set_source_files_properties(${objects} PROPERTIES LANGUAGE C)
set_source_files_properties(${assembler} PROPERTIES LANGUAGE ASM)
set_source_files_properties(${headers} PROPERTIES HEADER_FILE_ONLY ON)

# mega65libc target
add_library(mega65libc ${objects} ${headers} ${assembler})
target_include_directories(mega65libc
    PUBLIC 
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
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
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mega65
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


