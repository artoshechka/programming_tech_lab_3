cmake_minimum_required(VERSION 3.16)

project(data_model LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    src/timeline_data.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
