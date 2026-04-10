cmake_minimum_required(VERSION 3.20)

project(VECTOR VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(GNUInstallDirs)

add_library(vector_child_program_runtime STATIC
  src/child_program_runtime.c
)

add_executable(vector_host
  tools/vector_host.c
)

target_include_directories(vector_child_program_runtime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vector_host
  PRIVATE
    vector_child_program_runtime
)

if(MSVC)
  target_compile_options(vector_child_program_runtime PRIVATE /W4)
  target_compile_options(vector_host PRIVATE /W4)
else()
  target_compile_options(vector_child_program_runtime PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(vector_host PRIVATE -Wall -Wextra -Wpedantic)
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

add_test(NAME vector_host_version
  COMMAND vector_host version)

add_test(NAME vector_host_list
  COMMAND vector_host list)

add_test(NAME vector_host_route_tool
  COMMAND vector_host route tool-execution apply-patch
    character-alpha component-active subagent-host-demo)

add_test(NAME vector_host_assign_cortex_demo
  COMMAND vector_host assign-state
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cortex_host_demo.state
    tool-execution apply-patch)

add_test(NAME vector_host_assign_audit_only_fails
  COMMAND vector_host assign-state
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cortex_host_ingest.state
    tool-execution apply-patch)
set_tests_properties(vector_host_assign_audit_only_fails
  PROPERTIES WILL_FAIL TRUE)

install(TARGETS vector_child_program_runtime vector_host
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES include/vector/child_program_runtime.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vector)
