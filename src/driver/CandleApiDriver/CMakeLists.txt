# driver/CandleApiDriver/CMakeLists.txt

# Add source and header files
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/CandleApiDriver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CandleApiInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/api/candle.c
    ${CMAKE_CURRENT_LIST_DIR}/api/candle_ctrl_req.c
    ${CMAKE_CURRENT_LIST_DIR}/CandleApiTiming.cpp

    ${CMAKE_CURRENT_LIST_DIR}/CandleApiDriver.h
    ${CMAKE_CURRENT_LIST_DIR}/CandleApiInterface.h
    ${CMAKE_CURRENT_LIST_DIR}/CandleApiTiming.h
    ${CMAKE_CURRENT_LIST_DIR}/api/candle.h
    ${CMAKE_CURRENT_LIST_DIR}/api/candle_ctrl_req.h
    ${CMAKE_CURRENT_LIST_DIR}/api/candle_defs.h
    ${CMAKE_CURRENT_LIST_DIR}/api/ch_9.h
)

# Add include path
target_include_directories(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/api
)

# Windows-specific system libraries
if(WIN32)
    target_link_libraries(cangaroo PRIVATE
        SetupAPI
        Ole32
        WinUSB
    )
endif()

# CFLAGS equivalent (QMAKE_CFLAGS += -std=gnu11)
# Only needed if compiling C files with specific standard
set_sour_
