add_library(Engine SHARED
    src/Engine.cpp
    src/Core/Application.cpp
    src/Window/Window.cpp
    src/Renderer/Renderer.cpp
)

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Engine
    PRIVATE
        glfw
        glad
        OpenGL::GL
)

target_compile_definitions(Engine PRIVATE ENGINE_BUILD)
target_compile_definitions(Engine INTERFACE ENGINE_SHARED)

set_target_properties(Engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)