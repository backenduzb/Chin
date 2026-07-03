cmake_minimum_required(VERSION 3.14)
project(chin CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

find_package(OpenGL REQUIRED)

find_package(glfw3 CONFIG QUIET)
if(glfw3_FOUND)
    message(STATUS "Found GLFW3 config package")
    set(GLFW_LIBS glfw)
else()
    find_package(GLFW QUIET)
    if(GLFW_FOUND)
        message(STATUS "Found GLFW package")
        set(GLFW_LIBS GLFW::GLFW)
    else()
        find_library(GLFW_LIBRARY NAMES glfw glfw3 REQUIRED)
        find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h REQUIRED)
        message(STATUS "Found GLFW library: ${GLFW_LIBRARY}")
        set(GLFW_LIBS ${GLFW_LIBRARY})
    endif()
endif()

add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    external/glad/include
)
target_link_libraries(glad PRIVATE ${CMAKE_DL_LIBS})

add_executable(app
    src/main.cpp
    src/core/Application.cpp
    src/core/Window.cpp
    src/rendering/Camera.cpp
    src/rendering/Shader.cpp
    src/rendering/Renderer.cpp
    src/glb/GLBLoader.cpp
    src/glb/Animation.cpp
    src/utils/FileSystem.cpp
    src/core/SpeechBubbleManager.cpp
    src/rendering/UIRenderer.cpp
)

target_compile_definitions(app PRIVATE PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(app PRIVATE
    src
    external/tinygltf
)

if(GLFW_INCLUDE_DIR)
    target_include_directories(app PRIVATE ${GLFW_INCLUDE_DIR})
endif()

target_link_libraries(app PRIVATE
    glad
    OpenGL::GL
    ${GLFW_LIBS}
)
