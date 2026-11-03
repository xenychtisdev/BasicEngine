add_executable(game
    src/main.cpp
)

target_link_libraries(game PRIVATE engine)

#[[add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:Game>
    COMMENT "Copying Engine DLL next to Game executable"
)]]

set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)