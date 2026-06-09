cmake_minimum_required(VERSION 3.12)

project(OpenGLStudy)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glm CONFIG REQUIRED)


file(GLOB SRC_FILES *.cpp *.h)

add_executable(OpenGLStudy ${SRC_FILES})

target_compile_definitions(OpenGLStudy PRIVATE GLEW_STATIC)
target_compile_definitions(OpenGLStudy PRIVATE GLFW_STATIC)

target_include_directories(OpenGLStudy
    PRIVATE 
    ${GLM_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(OpenGLStudy
    PRIVATE 
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    glfw
    glm::glm-header-only
)

if (WIN32)
    set_target_properties(OpenGLStudy PROPERTIES
        COMPILE_FLAGS "/Od /Zi"
        LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
    )
endif()