cmake_minimum_required(VERSION 3.20)

project(VECTOR VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(vector_child_program_runtime STATIC
  src/child_program_runtime.c
)

target_include_directories(vector_child_program_runtime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
  target_compile_options(vector_child_program_runtime PRIVATE /W4)
else()
  target_compile_options(vector_child_program_runtime PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()

add_executable(vector_child_program_runtime_tests
  tests/test_child_program_runtime.c
)

target_link_libraries(vector_child_program_runtime_tests
  PRIVATE
    vector_child_program_runtime
)

target_compile_definitions(vector_child_program_runtime_tests
  PRIVATE
    VECTOR_TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

if(MSVC)
  target_compile_options(vector_child_program_runtime_tests PRIVATE /W4)
else()
  target_compile_options(vector_child_program_runtime_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(
  NAME vector_child_program_runtime_tests
  COMMAND vector_child_program_runtime_tests
)
