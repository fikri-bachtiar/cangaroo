# parser/dbc/CMakeLists.txt

# Add source and header files to the cangaroo target
target_sources(cangaroo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/DbcParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DbcTokens.cpp

    ${CMAKE_CURRENT_LIST_DIR}/DbcParser.h
    ${CMAKE_CURRENT_LIST_DIR}/DbcTokens.h
)

# Add include directory
target_include_directories(cangaroo PRIVATE ${CMAKE_CURRENT_LIST_DIR})
