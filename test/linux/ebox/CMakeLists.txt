
set(SOURCES ebox.c)
add_executable(ebox ${SOURCES})
target_link_libraries(ebox soem)
install(TARGETS simple_test DESTINATION bin)
