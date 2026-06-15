cmake_minimum_required(VERSION 3.16)
project(gui LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport)

add_library(${PROJECT_NAME} STATIC
    src/mainwindow.cpp
    src/chart_model.cpp
    src/table_select_dialog.cpp
    src/app_compositor.cpp
    src/theme.cpp
    src/file_item_delegate.cpp
    src/file_glyphs.cpp
    src/file_row_widget.cpp
    src/scrollable_chart_view.cpp
    src/pdf_exporter.cpp
    mainwindow.hpp
    app_compositor.hpp
    ui_strings.hpp
    ichart_exporter.hpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        chart
        style
        parser
        database_module
        Qt5::Widgets
        Qt5::Charts
        Qt5::PrintSupport
)
