cmake_minimum_required(VERSION 3.16)
project(programming_tech_lab_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(logger)
add_subdirectory(data_model)
add_subdirectory(parser)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE logger data_model parser)
