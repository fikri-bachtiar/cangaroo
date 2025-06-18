# driver/CANBlastDriver/CMakeLists.txt

# Add source and header files to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/CANBlasterDriver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CANBlasterInterface.cpp

    ${CMAKE_CURRENT_LIST_DIR}/CANBlasterDriver.h
    ${CMAKE_CURRENT_LIST_DIR}/CANBlasterInterface.h
)

# Include this directory for headers
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
