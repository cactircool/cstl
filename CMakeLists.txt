cmake_minimum_required(VERSION 3.15)
project(cstl C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE CSTDLIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
# Make sure headers are found when installed or used as subproject
add_library(${PROJECT_NAME} STATIC ${CSTDLIB_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 99
    POSITION_INDEPENDENT_CODE ON
)

# Optional: installation support
install(TARGETS ${PROJECT_NAME} EXPORT "${PROJECT_NAME}Targets")
install(DIRECTORY include/ DESTINATION include)

# Optional: make it findable via find_package()
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Config.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)
