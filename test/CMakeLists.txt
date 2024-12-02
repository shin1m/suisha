add_executable(loop loop.cc)
target_link_libraries(loop PRIVATE suisha pthread)
add_test(NAME loop COMMAND loop)
