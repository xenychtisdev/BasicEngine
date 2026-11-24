cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED SOURCE_DIR OR NOT DEFINED DEST_DIR)
    message(FATAL_ERROR "SOURCE_DIR and DEST_DIR are required")
endif()

if(NOT IS_DIRECTORY "${SOURCE_DIR}")
    message(FATAL_ERROR "Asset source folder does not exist: ${SOURCE_DIR}")
endif()

get_filename_component(SOURCE_DIR "${SOURCE_DIR}" ABSOLUTE)
get_filename_component(DEST_DIR "${DEST_DIR}" ABSOLUTE)

# Prevent synchronization between overlapping folders.
cmake_path(IS_PREFIX SOURCE_DIR "${DEST_DIR}" NORMALIZE source_contains_dest)
cmake_path(IS_PREFIX DEST_DIR "${SOURCE_DIR}" NORMALIZE dest_contains_source)

if(source_contains_dest OR dest_contains_source)
    message(FATAL_ERROR "Asset source and destination must not overlap")
endif()

file(MAKE_DIRECTORY "${DEST_DIR}")

# Remove stale destination entries, handle file/directory replacements
file(GLOB_RECURSE destination_entries
    LIST_DIRECTORIES true
    RELATIVE "${DEST_DIR}"
    "${DEST_DIR}/*"
)

foreach(relative_path IN LISTS destination_entries)
    set(source "${SOURCE_DIR}/${relative_path}")
    set(destination "${DEST_DIR}/${relative_path}")

    if(IS_DIRECTORY "${destination}")
        if(NOT IS_DIRECTORY "${source}")
            file(REMOVE_RECURSE "${destination}")
        endif()
    elseif(NOT EXISTS "${source}" OR IS_DIRECTORY "${source}")
        file(REMOVE "${destination}")
    endif()
endforeach()

# Copy new or changed files
file(GLOB_RECURSE source_files
    LIST_DIRECTORIES false
    RELATIVE "${SOURCE_DIR}"
    "${SOURCE_DIR}/*"
)

foreach(relative_path IN LISTS source_files)
    set(source "${SOURCE_DIR}/${relative_path}")
    set(destination "${DEST_DIR}/${relative_path}")

    get_filename_component(parent_directory "${destination}" DIRECTORY)
    file(MAKE_DIRECTORY "${parent_directory}")

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${source}"
            "${destination}"
        RESULT_VARIABLE copy_result
    )

    if(NOT copy_result STREQUAL "0")
        message(FATAL_ERROR "Could not copy asset: ${relative_path}")
    endif()
endforeach()