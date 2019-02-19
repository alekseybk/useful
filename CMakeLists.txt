project(useful)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS} -pedantic")

file(GLOB headers "useful/*.hpp")
file(GLOB test_headers "test/*.hpp")
file(GLOB sources test/test.cpp)

add_executable(${PROJECT_NAME} ${sources} ${headers} ${test_headers})
target_link_libraries(${PROJECT_NAME} "pthread")

