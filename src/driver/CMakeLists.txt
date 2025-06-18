# driver/CMakeLists.txt

qt6_wrap_ui(driver_ui
    ${CMAKE_CURRENT_LIST_DIR}/GenericCanSetupPage.ui
)

target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/CanInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanListener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanDriver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanTiming.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GenericCanSetupPage.cpp

    ${CMAKE_CURRENT_LIST_DIR}/CanInterface.h
    ${CMAKE_CURRENT_LIST_DIR}/CanListener.h
    ${CMAKE_CURRENT_LIST_DIR}/CanDriver.h
    ${CMAKE_CURRENT_LIST_DIR}/CanTiming.h
    ${CMAKE_CURRENT_LIST_DIR}/GenericCanSetupPage.h

    ${CMAKE_CURRENT_LIST_DIR}/GenericCanSetupPage.ui
)

target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
