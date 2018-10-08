project(useful)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS} -pedantic")

file(GLOB headers "useful.hpp")
file(GLOB sources "main.cpp")

add_executable(${PROJECT_NAME} "main.cpp" ${sources} ${headers})

