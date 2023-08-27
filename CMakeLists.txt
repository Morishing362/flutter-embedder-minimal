cmake_minimum_required(VERSION 3.10)

project(flutter-embedder-minimal)

# Set C++ standard
set(
    CMAKE_CXX_STANDARD_REQUIRED ON
)

# Find the OpenGL packagee
find_library(
    GLES3 REQUIRED
)

# Add link directories
link_directories(
    third/flutter/bin
)

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
        ${GLES3_LIBRARIS}
        EGL
        wayland-client
        wayland-egl
        flutter_engine
)
