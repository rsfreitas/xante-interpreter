cmake_minimum_required(VERSION 2.8)

project(xanter)

include_directories(include)
include_directories("/usr/local/include")

file(GLOB SOURCES "src/*c")
add_executable(${PROJECT_NAME} ${SOURCES})

link_directories("/usr/local/lib")
target_link_libraries(${PROJECT_NAME} xante collections)

