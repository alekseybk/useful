project(useful)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS} -pedantic")

file(GLOB headers useful/meta.hpp useful/basic_types.hpp useful/default_import.hpp
                  useful/utils.hpp useful/output.hpp useful/input.hpp useful/time_meter.hpp useful/all.hpp
                  useful/formats.hpp useful/strings.hpp useful/mm.hpp)

file(GLOB sources "main.cpp" "test.cpp")

add_executable(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} "pthread")

