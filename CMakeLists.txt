project(jerminus)
cmake_minimum_required(VERSION 2.8)

# Options
option(DEBUG "Enable/Disable debug version" ON)

include_directories(include)
include_directories("/usr/local/include")

if(CMAKE_C_COMPILER_VERSION VERSION_GREATER 5)
    add_definitions(-fgnu89-inline)
endif()

add_definitions("-Wall -Wextra -O0")

if(DEBUG)
    add_definitions("-ggdb")
endif(DEBUG)

file(GLOB SOURCES "src/*c")
add_executable(${PROJECT_NAME} ${SOURCES})

link_directories("/usr/local/lib")
target_link_libraries(${PROJECT_NAME} xante collections)

