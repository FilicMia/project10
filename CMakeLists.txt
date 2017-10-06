cmake_minimum_required(VERSION 2.6)
project(project10)

add_executable(project10 main.c simplemotion.c tcpclient.c sm_consts.c pcserialport.c devicedeployment.c busdevice.c bufferedmotion.c)

target_link_libraries(project10 m)

install(TARGETS project10 RUNTIME DESTINATION bin)
