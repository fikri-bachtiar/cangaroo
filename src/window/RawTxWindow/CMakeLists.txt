# window/RawTxWindow/CMakeLists.txt

# Wrap the .ui file
qt6_wrap_ui(rawtxwindow_ui
    ${CMAKE_CURRENT_LIST_DIR}/RawTxWindow.ui
)

# Add sources, headers, and UI-generated files to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/RawTxWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/RawTxWindow.h

    ${CMAKE_CURRENT_LIST_DIR}/RawTxWindow.ui
)

# Include this directory for header resolution
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
