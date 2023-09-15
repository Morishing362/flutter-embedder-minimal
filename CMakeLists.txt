cmake_minimum_required(VERSION 3.10)

project(flutter-embedder-minimal)

# Set C++ standard
set(
    CMAKE_CXX_STANDARD_REQUIRED ON
)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    link_directories(third/flutter/bin/aarch64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    link_directories(third/flutter/bin/x86_64)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()


# Add link directories
# link_directories(
#     third/flutter/bin
# )

# Collect source files
file(
    GLOB SRC
        src/main.cc
        src/flutter/*.cc
        src/wayland/*.cc
        third/wayland/*.c
)

# Add the executable
add_executable(
    ${PROJECT_NAME} ${SRC}
)

# Include directories
target_include_directories(
    ${PROJECT_NAME} PRIVATE
        src/flutter
        src/wayland
        src/common
        third/flutter
        third/wayland
)

# Link the executable to GLFW
target_link_libraries(
    ${PROJECT_NAME} PRIVATE 
        EGL
        wayland-client
        wayland-egl
        flutter_engine
)
