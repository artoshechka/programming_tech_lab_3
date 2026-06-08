cmake_minimum_required(VERSION 3.16)
project(chart LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Charts)

add_library(${PROJECT_NAME} STATIC
    src/pie_chart_builder.cpp
    src/bar_chart_builder.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    PUBLIC data_model style Qt5::Charts
)
