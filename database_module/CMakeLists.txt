cmake_minimum_required(VERSION 3.16)
project(database_module LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Sql)

add_library(${PROJECT_NAME} STATIC
    src/sqlite_db.cpp
    src/sqlite_db_manager.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Sql)
