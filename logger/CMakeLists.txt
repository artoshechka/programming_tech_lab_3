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

# GCC/MinGW < 9 выносит std::filesystem в отдельную библиотеку stdc++fs;
# без явной линковки получаем undefined reference из fs_path.h (create_directories и пр.).
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
