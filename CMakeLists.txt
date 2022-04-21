cmake_minimum_required(VERSION 3.22)
project(sdvx_cpp)

set(CMAKE_CXX_STANDARD 14)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(sdvx_cpp
        main.cpp

        asphyxia/aspParser.cpp
        asphyxia/aspParser.h

        sdvx/sdvxParser.cpp
        sdvx/sdvxParser.h

        util/logger.cpp
        util/logger.h)

# jsoncpp
find_package(jsoncpp CONFIG REQUIRED)
if (jsoncpp_FOUND)
    message("find jsoncpp ${jsoncpp_VERSION}")
    target_link_libraries(sdvx_cpp PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
endif ()

# tinyxml2
find_package(tinyxml2 CONFIG REQUIRED)
if (tinyxml2_FOUND)
    message("find tinyxml2 ${tinyxml2_VERSION}")
    target_link_libraries(sdvx_cpp PRIVATE tinyxml2::tinyxml2)
endif ()

# spdlog
find_package(spdlog CONFIG REQUIRED)
if (spdlog_FOUND)
    message("find spdlog ${spdlog_VERSION}")
    target_link_libraries(sdvx_cpp PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
endif ()

# sqlite3
find_package(SQLiteCpp CONFIG REQUIRED)
if (SQLiteCpp_FOUND)
    message("find SQLiteCpp ${SQLiteCpp_VERSION}")
    target_link_libraries(sdvx_cpp PRIVATE SQLiteCpp)
endif()
