cmake_minimum_required(VERSION 3.12)

project(OpenGLStudy)

find_package(OpenGL REQUIRED)
#find_package(GLFW3 REQUIRED)

set(GLFW_ROOT D:/DevCode/SDK/glfw-3.4)

file(GLOB SRC_FILES *.cpp *.h)


find_path(GLFW3_INCLUDE_DIR
    NAMES GLFW/glfw3.h
    PATHS ${GLFW_ROOT}/include
)

find_library(GLFW3_LIBRARIES
    NAMES glfw3
    PATHS ${GLFW_ROOT}/lib-vc2022
)

add_executable(OpenGLStudy ${SRC_FILES})

target_compile_definitions(OpenGLStudy PRIVATE GLFW_STATIC)

target_include_directories(OpenGLStudy
    PRIVATE 
    ${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR}
)

target_link_libraries(OpenGLStudy
    PRIVATE 
    ${OPENGL_LIBRARIES}
    ${GLFW3_LIBRARIES}
)