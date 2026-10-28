add_executable(Game
    src/main.cpp
)

target_link_libraries(Game PRIVATE Engine)

#[[add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:Game>
    COMMENT "Copying Engine DLL next to Game executable"
)]]

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)