cmake_minimum_required(VERSION 3.16)

project(cangaroo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt automation features
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set common include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Find Qt6 packages (only once at root level)
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Xml Charts SerialPort Network Core5Compat
)

# Optional: for UNIX-specific library linking via pkg-config
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-route-3.0)
endif()

# Add libusb thirdparty
add_subdirectory(thirdparty/libusb-cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libusb-cmake)

# Add source directory
add_subdirectory(src)