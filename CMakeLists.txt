cmake_minimum_required(VERSION 3.23)
project(GarnetEngine LANGUAGES CXX)

# Set C++23 compiler version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically export all methods
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Export .dll and .lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find required SDL3 dependencies
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

# Add .cpp files
add_library(GarnetEngine SHARED
    src/vec2.cpp
    src/FontManager.cpp
    src/TextureManager.cpp
    src/Renderer.cpp
    src/Scene.cpp
)

# Set include directories
target_include_directories(GarnetEngine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/
)

# Link SDL3 dependencies to GarnetEngine
target_link_libraries(GarnetEngine PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)