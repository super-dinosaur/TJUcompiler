cmake_minimum_required(VERSION 3.25.2)
project(CmmCompiler 
    VERSION 1.0.0
    DESCRIPTION "A C-- Compiler"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lexical_analysis
    ${CMAKE_CURRENT_SOURCE_DIR}/syntax_analysis
)

# Collect source files
file(GLOB_RECURSE LEXICAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/lexical_analysis/*.h)
file(GLOB_RECURSE SYNTAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/syntax_analysis/*.h)

# Main executable
add_executable(CmmCompiler
    main.cpp
    ${LEXICAL_HEADERS}
    ${SYNTAX_HEADERS}
)

# Set executable properties
set_target_properties(CmmCompiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Optional: Add a custom target to run the compiler
add_custom_target(run
    COMMAND CmmCompiler
    DEPENDS CmmCompiler
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Optional: Add a custom target to run tests
add_custom_target(test-compiler
    COMMAND CmmCompiler test/test_1.sy
    DEPENDS CmmCompiler
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Print configuration
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")

# Installation rules (optional)
install(TARGETS CmmCompiler
    RUNTIME DESTINATION bin
)

# CPack configuration for creating packages (optional)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
