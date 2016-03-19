cmake_minimum_required(VERSION 2.8)

# Use C++11
set(CMAKE_CXX_FLAGS "-std=c++11")  # This does the trick for Linux
set(CMAKE_CXX_STANDARD 11)  # This does the trick for Mac OS

project(CartLauncher)

# Build executable
add_executable(CartLauncher CartLauncher.cpp)

# Then find SDL
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CartLauncher_SOURCE_DIR}/cmake")

# Include SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    )
target_link_libraries(
    CartLauncher
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    )
