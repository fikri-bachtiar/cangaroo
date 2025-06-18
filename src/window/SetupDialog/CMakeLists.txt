# window/SetupDialog/CMakeLists.txt

# Wrap .ui files
qt6_wrap_ui(setupdialog_ui
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialog.ui
    ${CMAKE_CURRENT_LIST_DIR}/SelectCanInterfacesDialog.ui
)

# Add source, header, and UI-generated files to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialogTreeModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialogTreeItem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SelectCanInterfacesDialog.cpp

    ${CMAKE_CURRENT_LIST_DIR}/SetupDialog.h
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialogTreeModel.h
    ${CMAKE_CURRENT_LIST_DIR}/SetupDialogTreeItem.h
    ${CMAKE_CURRENT_LIST_DIR}/SelectCanInterfacesDialog.h

    ${CMAKE_CURRENT_LIST_DIR}/SetupDialog.ui
    ${CMAKE_CURRENT_LIST_DIR}/SelectCanInterfacesDialog.ui
)

# Add include path
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
