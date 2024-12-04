cmake_minimum_required(VERSION 3.28)
project(AoC2024)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        ctre
        GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
        GIT_TAG v3.9.0
)

FetchContent_MakeAvailable(ctre)

add_executable(AoC2024 main.cpp)

target_link_libraries(AoC2024 PRIVATE ctre::ctre)