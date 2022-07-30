add_executable(test_378 test_378.c)
target_link_libraries(test_378 mega65libc)
set_target_properties(test_378 PROPERTIES OUTPUT_NAME test_378.prg EXCLUDE_FROM_ALL true)

