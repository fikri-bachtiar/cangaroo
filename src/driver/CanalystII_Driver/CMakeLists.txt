# driver/CanalystII_Driver/CMakeLists.txt

# Add source and header files
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/api/canalyst.c
    ${CMAKE_CURRENT_LIST_DIR}/CanalystII_Driver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CanalystII_Interface.cpp

    ${CMAKE_CURRENT_LIST_DIR}/api/canalyst.h
    ${CMAKE_CURRENT_LIST_DIR}/CanalystII_Driver.h
    ${CMAKE_CURRENT_LIST_DIR}/CanalystII_Interface.h
)

# Add include path
target_include_directories(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/api
)
