cmake_minimum_required(VERSION 3.10)

project(flutter-embedder-minimal)

# Set C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        third/flutter/headers
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

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${ENGINE_PATH} RUNTIME DESTINATION lib)
