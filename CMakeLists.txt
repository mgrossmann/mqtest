cmake_minimum_required(VERSION 3.10)
project(amqp_test)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option to use static librabbitmq
option(STATIC_RABBITMQ "Link with static librabbitmq library" OFF)

# Find librabbitmq
find_package(PkgConfig REQUIRED)

# Configure pkg-config to find the library
if(STATIC_RABBITMQ)
    # For static linking
    pkg_check_modules(RABBITMQ REQUIRED IMPORTED_TARGET librabbitmq)
    message(STATUS "Configured for static linking with librabbitmq")
else()
    # For shared linking
    pkg_check_modules(RABBITMQ REQUIRED IMPORTED_TARGET librabbitmq)
    message(STATUS "Configured for shared linking with librabbitmq")
endif()

# Create executable
add_executable(amqp_test main.c)

# Include directories
target_include_directories(amqp_test PRIVATE ${RABBITMQ_INCLUDE_DIRS})

if(STATIC_RABBITMQ)
    find_library(RABBITMQ_STATIC_LIB NAMES librabbitmq.a PATHS ${RABBITMQ_LIBRARY_DIRS})
    if(NOT RABBITMQ_STATIC_LIB)
        message(FATAL_ERROR "Static librabbitmq.a not found!")
    endif()
    target_link_libraries(amqp_test PRIVATE ${RABBITMQ_STATIC_LIB})
else()
    target_link_libraries(amqp_test PRIVATE ${RABBITMQ_LIBRARIES})
endif()

# Set compiler flags
target_compile_options(amqp_test PRIVATE ${RABBITMQ_CFLAGS_OTHER})

