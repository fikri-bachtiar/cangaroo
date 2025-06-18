# driver/SLCANDriver/CMakeLists.txt

# Add source and header files to cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/SLCANInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SLCANDriver.cpp

    ${CMAKE_CURRENT_LIST_DIR}/SLCANInterface.h
    ${CMAKE_CURRENT_LIST_DIR}/SLCANDriver.h
)

# Include this directory for headers
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
