cmake_minimum_required(VERSION 3.16)

project(cangaroo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Xml Charts SerialPort Network Core5Compat
)

# Optional: for UNIX-specific library linking via pkg-config
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-route-3.0)
endif()

# Add source directory
add_subdirectory(src)
