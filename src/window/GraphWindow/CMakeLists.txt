# window/GraphWindow/CMakeLists.txt

# Wrap the .ui file
qt6_wrap_ui(graphwindow_ui
    ${CMAKE_CURRENT_LIST_DIR}/GraphWindow.ui
)

# Add sources, headers, and UI-generated code to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/GraphWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GraphWindow.h
    # ${graphwindow_ui}
)

# Include this directory
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
