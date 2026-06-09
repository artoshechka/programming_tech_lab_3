cmake_minimum_required(VERSION 3.16)
project(style LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Charts)

add_library(${PROJECT_NAME} STATIC
    src/color_style.cpp
    src/grayscale_style.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC logger Qt5::Charts)
