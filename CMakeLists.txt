cmake_minimum_required(VERSION 3.5)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)
find_package(Doxygen)
project(mega65libc VERSION 0.3.0 LANGUAGES C ASM)

enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(assembler
    src/llvm/fileio.s
    src/llvm/dirent.s
    src/llvm/memory_asm.s)

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
    include/mega65/conio.h
    include/mega65/debug.h
    include/mega65/dirent.h
    include/mega65/fileio.h
    include/mega65/hal.h
    include/mega65/memory.h
    include/mega65/mouse.h
    include/mega65/random.h
    include/mega65/sdcard.h
    include/mega65/targets.h
    include/mega65/tests.h
    include/mega65/time.h)

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
    -Wno-language-extension-token
)
target_compile_options(mega65libc PRIVATE -Os ${CLANG_WARNINGS})

# install target
include(GNUInstallDirs)
install(TARGETS mega65libc
    EXPORT mega65libc-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION include/mega65
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

# build API documentation using doxygen
if(DOXYGEN_FOUND)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} -q ${CMAKE_SOURCE_DIR}/doc/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()

# add tests directory
add_subdirectory(tests)
