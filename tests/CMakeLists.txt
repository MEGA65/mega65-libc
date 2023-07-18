find_program(XEMU NAMES xmega65 xmega65.native)
set(XEMU_ARGS "-besure -headless -sleepless -testing")

function(TEST NAME)
    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} mega65libc)
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${NAME}.prg)
    if (XEMU)
        add_test(NAME ${NAME} COMMAND sh -c "${XEMU} ${XEMU_ARGS} -prg ${NAME}.prg")
    endif()
endfunction()

# compile example prg file
add_executable(time-test time.c)
target_link_libraries(time-test mega65libc)
set_target_properties(time-test PROPERTIES OUTPUT_NAME time.prg)

TEST(test-memory)
