# Taken from : https://stackoverflow.com/questions/32645217/visual-studio-as-an-editor-for-cmake-friendly-project
set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${_src_root_path})
include_directories(${MPI_INCLUDE_PATH})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

add_executable(Example1 ${_source_list})

target_link_libraries(
	Example1 
	PRIVATE  
	MPI_C
)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()