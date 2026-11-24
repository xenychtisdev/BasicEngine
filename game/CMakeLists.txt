add_executable(game
    src/main.cpp
)

target_link_libraries(game PRIVATE engine)

set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

#[[add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:Game>
    COMMENT "Copying Engine DLL next to Game executable"
)]]

add_custom_target(sync_game_assets
    COMMAND "${CMAKE_COMMAND}"
        "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}/assets"
        "-DDEST_DIR=$<TARGET_FILE_DIR:game>/assets"
        -P "${PROJECT_SOURCE_DIR}/cmake/syncAssets.cmake"
    COMMENT "Synchronizing game assets"
    VERBATIM
)

add_dependencies(game sync_game_assets)