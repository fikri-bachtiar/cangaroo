# core/CMakeLists.txt

# Add these files to the parent target 'cangaroo'
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanMessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanTrace.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanDbMessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanDb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanDbNode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanDbSignal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementSetup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementNetwork.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/LogModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ConfigurableWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Log.cpp

    ${CMAKE_CURRENT_LIST_DIR}/portable_endian.h
    ${CMAKE_CURRENT_LIST_DIR}/Backend.h
    ${CMAKE_CURRENT_LIST_DIR}/CanMessage.h
    ${CMAKE_CURRENT_LIST_DIR}/CanTrace.h
    ${CMAKE_CURRENT_LIST_DIR}/CanDbMessage.h
    ${CMAKE_CURRENT_LIST_DIR}/CanDb.h
    ${CMAKE_CURRENT_LIST_DIR}/CanDbNode.h
    ${CMAKE_CURRENT_LIST_DIR}/CanDbSignal.h
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementSetup.h
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementNetwork.h
    ${CMAKE_CURRENT_LIST_DIR}/MeasurementInterface.h
    ${CMAKE_CURRENT_LIST_DIR}/LogModel.h
    ${CMAKE_CURRENT_LIST_DIR}/ConfigurableWidget.h
    ${CMAKE_CURRENT_LIST_DIR}/Log.h
)

# Add include path for this module
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
