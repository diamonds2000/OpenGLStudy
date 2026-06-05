cmake_minimum_required(VERSION 3.12)

project(OpenGLStudy)

find_package(OpenGL REQUIRED)

if (WIN32)
    set(GLEW_ROOT D:/DevCode/SDK/glew)
    set(GLFW_ROOT D:/DevCode/SDK/glfw-3.4)

    find_path(GLEW_INCLUDE_DIR
        NAMES glew.h
        PATHS ${GLEW_ROOT}/include
    )
    find_path(GLFW3_INCLUDE_DIR
        NAMES GLFW/glfw3.h
        PATHS ${GLFW_ROOT}/include
    )

    find_library(GLEW_LIBRARIES
        NAMES glew32s
        PATHS ${GLEW_ROOT}/lib
    )

    find_library(GLFW3_LIBRARIES
        NAMES glfw3
        PATHS ${GLFW_ROOT}/lib-vc2022
    )

elseif (APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")

    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)

    set(GLEW_LIBRARIES GLEW::GLEW)
    set(GLFW3_LIBRARIES glfw)
endif()


file(GLOB SRC_FILES *.cpp *.h)

add_executable(OpenGLStudy ${SRC_FILES})

target_compile_definitions(OpenGLStudy PRIVATE GLEW_STATIC)
target_compile_definitions(OpenGLStudy PRIVATE GLFW_STATIC)

target_include_directories(OpenGLStudy
    PRIVATE 
    ${GLEW_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(OpenGLStudy
    PRIVATE 
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW3_LIBRARIES}
)