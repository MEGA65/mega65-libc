find_program(XEMU NAMES xmega65 xmega65.native)
set(XEMU_ARGS "-besure -headless -sleepless -testing")

function(TEST NAME)
    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} mega65libc)
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${NAME}.prg)
    target_compile_options(${NAME} PRIVATE -Os ${CLANG_WARNINGS})
    if (XEMU)
        add_test(NAME ${NAME} COMMAND sh -c "${XEMU} ${XEMU_ARGS} -prg ${NAME}.prg")
    endif()
endfunction()

TEST(test-memory)
