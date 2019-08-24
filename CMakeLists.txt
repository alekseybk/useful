cmake_minimum_required(VERSION 2.8)

project(useful)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic-errors -ferror-limit=1")

file(GLOB headers "useful/*.hpp")
file(GLOB test_headers "test/*.hpp")
file(GLOB test_sources "test/*.cpp")

add_executable(${PROJECT_NAME} ${test_sources} ${test_headers} ${headers})
target_link_libraries(${PROJECT_NAME} "pthread")

