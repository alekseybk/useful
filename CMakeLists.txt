project(useful)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS} -pedantic")

file(GLOB headers useful/useful.hpp useful/meta.hpp useful/short_int.hpp
                  useful/utils.hpp useful/std_import.hpp useful/out_overloads.hpp useful/in_overloads.hpp)
file(GLOB sources "main.cpp")

add_executable(${PROJECT_NAME} "main.cpp" ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} "pthread")

