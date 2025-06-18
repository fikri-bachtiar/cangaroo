# window/LogWindow/CMakeLists.txt

# Wrap the .ui file
qt6_wrap_ui(logwindow_ui
    ${CMAKE_CURRENT_LIST_DIR}/LogWindow.ui
)

# Add source, header, and UI-generated code to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/LogWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/LogWindow.h

    ${CMAKE_CURRENT_LIST_DIR}/LogWindow.ui
)

# Include this directory
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
