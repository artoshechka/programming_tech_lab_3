cmake_minimum_required(VERSION 3.16)
project(programming_tech_lab_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/gui/src)

find_package(Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport)

# Исходники содержат кириллические строковые литералы (UI-строки, единицы размера).
# MSVC без /utf-8 интерпретирует их как ANSI и портит — задаём кодировку явно.
if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(logger)
add_subdirectory(data_model)
add_subdirectory(database_module)
add_subdirectory(parser)
add_subdirectory(chart)
add_subdirectory(style)
add_subdirectory(gui)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE gui)
