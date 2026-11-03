add_library(engine SHARED
    src/engine.cpp
    src/core/application.cpp
    src/window/window.cpp
    src/renderer/renderer.cpp
    src/input/input.cpp
)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine
    PRIVATE
        glfw
        glad
        OpenGL::GL
    PUBLIC
        glm::glm
        imgui
)

target_compile_definitions(engine PRIVATE ENGINE_BUILD)
target_compile_definitions(engine INTERFACE ENGINE_SHARED)

set_target_properties(engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)