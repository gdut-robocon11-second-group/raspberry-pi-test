cmake_minimum_required(VERSION 3.24)

project(raspberry-pi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(fmt)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${CMAKE_SOURCE_DIR}/asio/include)

add_executable(raspberry-pi test.cpp)
target_include_directories(raspberry-pi PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(raspberry-pi PRIVATE
    asio
    fmt)
if (WIN32)
    target_compile_definitions(raspberry-pi PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(raspberry-pi PRIVATE ws2_32)
elseif (UNIX)
    target_link_libraries(raspberry-pi PRIVATE pthread)
endif()
