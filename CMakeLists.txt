cmake_minimum_required(VERSION 3.5)

project(useful)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -ferror-limit=1")

file(GLOB test_sources "test/*.cpp")

add_executable(${PROJECT_NAME} ${test_sources})
target_link_libraries(${PROJECT_NAME} "pthread")

