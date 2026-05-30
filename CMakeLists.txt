cmake_minimum_required(VERSION 3.16)
project(programming_tech_lab_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(logger)
add_subdirectory(data_model)
add_subdirectory(parser)

add_executable(${PROJECT_NAME} main.cpp)

# Абсолютный путь к корню проекта, чтобы данные находились независимо от рабочей директории.
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

# parser публично тянет logger и data_model, поэтому линкуем только его.
target_link_libraries(${PROJECT_NAME} PRIVATE parser)
