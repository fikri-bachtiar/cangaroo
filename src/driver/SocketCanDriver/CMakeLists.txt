# driver/SocketCanDriver/CMakeLists.txt

# Add sources and headers to cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/SocketCanInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SocketCanDriver.cpp

    ${CMAKE_CURRENT_LIST_DIR}/SocketCanInterface.h
    ${CMAKE_CURRENT_LIST_DIR}/SocketCanDriver.h
)

# Add include directory
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
