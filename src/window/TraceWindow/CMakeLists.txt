# window/TraceWindow/CMakeLists.txt

# Wrap .ui file
qt6_wrap_ui(tracewindow_ui
    ${CMAKE_CURRENT_LIST_DIR}/TraceWindow.ui
)

# Add source, header, and UI-generated files to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/TraceFilterModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/LinearTraceViewModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AggregatedTraceViewModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BaseTraceViewModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AggregatedTraceViewItem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TraceWindow.cpp

    ${CMAKE_CURRENT_LIST_DIR}/TraceFilterModel.h
    ${CMAKE_CURRENT_LIST_DIR}/LinearTraceViewModel.h
    ${CMAKE_CURRENT_LIST_DIR}/AggregatedTraceViewModel.h
    ${CMAKE_CURRENT_LIST_DIR}/BaseTraceViewModel.h
    ${CMAKE_CURRENT_LIST_DIR}/AggregatedTraceViewItem.h
    ${CMAKE_CURRENT_LIST_DIR}/TraceWindow.h
    ${CMAKE_CURRENT_LIST_DIR}/TraceViewTypes.h

    ${CMAKE_CURRENT_LIST_DIR}/TraceWindow.ui
)

# Add include directory
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
