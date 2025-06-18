cmake_minimum_required(VERSION 3.16)
project(cangaroo LANGUAGES CXX)

# Find required Qt packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Xml
    Charts
    SerialPort
    Network
    Core5Compat
)

# Enable automatic moc, uic, and rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define the main application target
add_executable(cangaroo)

# Common include directories
target_include_directories(cangaroo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Common Qt modules
target_link_libraries(cangaroo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
    Qt6::Charts
    Qt6::SerialPort
    Qt6::Network
    Qt6::Core5Compat
)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(driver)
add_subdirectory(parser/dbc)
add_subdirectory(window/TraceWindow)
add_subdirectory(window/SetupDialog)
add_subdirectory(window/LogWindow)
add_subdirectory(window/GraphWindow)
add_subdirectory(window/CanStatusWindow)
add_subdirectory(window/RawTxWindow)

# Add top-level application files
set(UIS
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
)

qt6_wrap_ui(mainwindow_ui ${UIS})

target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${mainwindow_ui}
    ${CMAKE_CURRENT_SOURCE_DIR}/cangaroo.qrc
)

qt6_add_resources(cangaroo cangaroo.qrc)

# Output directories
set_target_properties(cangaroo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

# Unix-specific settings
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-route-3.0)
    
    target_include_directories(cangaroo PRIVATE ${LIBNL_INCLUDE_DIRS})
    target_link_libraries(cangaroo PRIVATE ${LIBNL_LIBRARIES})
    
    # Add SocketCAN driver
    add_subdirectory(driver/SocketCanDriver)
    # target_link_libraries(cangaroo PRIVATE SocketCanDriver)
endif()

# Windows-specific settings
if(WIN32)
    target_link_libraries(cangaroo PRIVATE Setupapi Ole32 Winusb)
    # Add CandleApi driver
    add_subdirectory(driver/CandleApiDriver)
    # target_link_libraries(cangaroo PRIVATE CandleApiDriver)
endif()

# Common drivers (both Unix and Windows)
add_subdirectory(driver/CANBlastDriver)
add_subdirectory(driver/SLCANDriver)
# target_link_libraries(cangaroo PRIVATE CANBlastDriver SLCANDriver)