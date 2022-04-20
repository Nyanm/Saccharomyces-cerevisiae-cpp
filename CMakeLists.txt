cmake_minimum_required(VERSION 3.22)
project(sdvx_cpp)

set(CMAKE_CXX_STANDARD 14)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(sdvx_cpp main.cpp aspParser.cpp aspParser.h)

# jsoncpp
find_package(jsoncpp CONFIG REQUIRED)
if (jsoncpp_FOUND)
    message("find jsoncpp=${jsoncpp_VERSION}")
    target_link_libraries(sdvx_cpp PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
endif ()