# Collect test source files
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.c")

# Create the test executable
add_executable(cstl_tests_runner ${TEST_SOURCES})

# Link the main library
target_link_libraries(cstl_tests_runner PRIVATE cstl)

# Include main library headers
target_include_directories(cstl_tests_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Register the test with CTest
add_test(NAME cstl_tests COMMAND cstl_tests_runner)
