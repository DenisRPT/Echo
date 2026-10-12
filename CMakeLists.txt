cmake_minimum_required(VERSION 3.20)
project(Echo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1)
FetchContent_MakeAvailable(glfw glm)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

file(GLOB_RECURSE ECHO_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_executable(Echo ${ECHO_SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "src" FILES ${ECHO_SOURCES})

file(GLOB_RECURSE ECHO_SHADERS CONFIGURE_DEPENDS shaders/*.vert shaders/*.frag shaders/*.glsl)
target_sources(Echo PRIVATE ${ECHO_SHADERS})
set_source_files_properties(${ECHO_SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE ${CMAKE_SOURCE_DIR}/shaders PREFIX "shaders" FILES ${ECHO_SHADERS})

target_include_directories(Echo PRIVATE src)
target_link_libraries(Echo PRIVATE glfw glad glm::glm)
target_compile_definitions(Echo PRIVATE GLFW_INCLUDE_NONE)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Echo)
set_target_properties(Echo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")