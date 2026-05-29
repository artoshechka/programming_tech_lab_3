cmake_minimum_required(VERSION 3.16)

project(parser LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

add_library(${PROJECT_NAME} STATIC
    src/json_parser.cpp
    src/database_parser.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        data_model
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)
