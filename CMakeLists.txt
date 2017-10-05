cmake_minimum_required(VERSION 2.6)
project(project10)

add_executable(project10 main.cpp)

install(TARGETS project10 RUNTIME DESTINATION bin)
