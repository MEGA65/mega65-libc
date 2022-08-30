get_filename_component(mega65libc_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

if(NOT TARGET mega65libc::mega65libc)
    include("${mega65libc_CMAKE_DIR}/mega65libcTargets.cmake")
endif()
