cmake_minimum_required(VERSION 3.22)
project(Document-Editor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Disable clip examples and tests
# set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
# set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")

######################################

# Raylib
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # Fetch/download and build raylib
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

######################################

# Include libraries and source files
include_directories(src)
add_executable(${PROJECT_NAME}
    src/constants.hpp
    src/raygui_impl.cpp

    src/main.cpp
    src/utils.cpp
    
    src/editor.cpp
    
    src/rope/node.cpp
    src/rope/node_leaf.cpp
    src/rope/node_concatenation.cpp
    src/rope/rope.cpp
    src/rope/utils.cpp
    
    # src/document.cpp
    src/text/nchar.cpp
    src/text/nstring.cpp
    src/text/utils.cpp

    src/dictionary/dictionary.cpp
    src/dictionary/word.cpp
    src/dictionary/trie.cpp
    src/autocomplete/suggester.cpp

    src/keybind/keybind.cpp
    src/keybind/node.cpp

    src/document/document.cpp
    src/search/search.cpp

    src/FontFactory.cpp
    src/DocumentFont.cpp
)

######################################
# Testing
add_executable(rope_test
    src/rope/test.cpp

    src/rope/node.cpp
    src/rope/node_leaf.cpp
    src/rope/node_concatenation.cpp
    src/rope/rope.cpp
    src/rope/utils.cpp

    src/text/nchar.cpp
    src/text/nstring.cpp
    src/text/utils.cpp
)

add_executable(search_test
    src/search/test.cpp
    src/search/search.cpp

    src/rope/node.cpp
    src/rope/node_leaf.cpp
    src/rope/node_concatenation.cpp
    src/rope/rope.cpp
    src/rope/utils.cpp

    src/text/nchar.cpp
    src/text/nstring.cpp
    src/text/utils.cpp
)

add_executable(dictionary_test
    src/dictionary/test.cpp
    
    src/dictionary/dictionary.cpp
    src/dictionary/word.cpp
    src/dictionary/trie.cpp
    src/autocomplete/suggester.cpp

    src/text/nchar.cpp
    src/text/nstring.cpp
    src/text/utils.cpp
)
# target_compile_options(rope_test PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)

# In case that you have ${PNG_LIBRARY} set to support copy/paste images on Linux
# set(CLIP_X11_PNG_LIBRARY "${PNG_LIBRARY}")

# Add clip subdirectory to compile the library
# add_subdirectory(clip)

target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC include)
# target_link_libraries(${PROJECT_NAME} clip)

# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)

# Copy assets to build folder
file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})