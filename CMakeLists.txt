cmake_minimum_required(VERSION 3.30)
project(Monke)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)

add_executable(Monke src/main.cpp
        src/window.cpp
        include/window.h
        src/font.cpp
        include/font.h
        src/game.cpp
        include/game.h
)

target_include_directories(Monke PUBLIC include)
target_link_libraries(Monke PUBLIC sfml-graphics sfml-audio fmt)
