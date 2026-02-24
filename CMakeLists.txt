cmake_minimum_required(VERSION 3.10)
project(libuart_v2 VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Library source files
set(LIB_SOURCES
    protocol/uni_communication.c
)

set(LIB_HEADERS
    protocol/uni_communication.h
)

# Create static library
add_library(uart_comm STATIC ${LIB_SOURCES})
target_include_directories(uart_comm PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocol>
    $<INSTALL_INTERFACE:include>
)

# Compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(uart_comm PRIVATE 
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
endif()

# Examples (optional)
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    # Linux POSIX example
    if(UNIX AND NOT APPLE)
        add_executable(linux_demo example/linux_posix_demo.c)
        target_link_libraries(linux_demo uart_comm pthread)
    endif()
endif()

# Installation
install(TARGETS uart_comm
    EXPORT uart_comm-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${LIB_HEADERS}
    DESTINATION include
)

install(EXPORT uart_comm-targets
    FILE uart_comm-config.cmake
    DESTINATION lib/cmake/uart_comm
)
