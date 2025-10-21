cmake_minimum_required(VERSION 3.15)
project(cstl C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Collect all source files
file(GLOB_RECURSE CSTDLIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create the static library
add_library(${PROJECT_NAME} STATIC ${CSTDLIB_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Optional: installation support
install(TARGETS ${PROJECT_NAME} EXPORT "${PROJECT_NAME}Targets")
install(DIRECTORY include/ DESTINATION include)
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Config.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)

# Add the tests subdirectory
add_subdirectory(tests)
enable_testing()
