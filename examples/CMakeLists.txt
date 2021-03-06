link_libraries(suisha)
add_executable(tst test.cc)
add_executable(spp dbus.cc spp.cc)
target_link_libraries(spp suishadbus)
