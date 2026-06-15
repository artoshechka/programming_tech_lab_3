cmake_minimum_required(VERSION 3.16)

project(logger LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    src/app_logger.cpp
    src/log_entry_stream.cpp
    src/logger_factory.cpp
    src/log_settings.cpp
    src/thread_safe_logger.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Работа с путями/каталогами через Qt (QDir/QFileInfo) вместо std::filesystem —
# std::filesystem нестабилен на MinGW < 9 и требует отдельной линковки stdc++fs.
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
