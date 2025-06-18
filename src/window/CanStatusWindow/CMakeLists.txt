# window/CanStatusWindow/CMakeLists.txt

# Wrap the .ui file
qt6_wrap_ui(canstatuswindow_ui
    ${CMAKE_CURRENT_LIST_DIR}/CanStatusWindow.ui
)

# Add source, header, and UI-generated code to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/CanStatusWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanStatusWindow.h

    ${CMAKE_CURRENT_LIST_DIR}/CanStatusWindow.ui
)

# Include directory for this module
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
