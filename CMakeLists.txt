cmake_minimum_required(VERSION 3.7)
project(CongbRenderer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

if(WIN32)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(SDL2_PATH "C:\\vs_dev_lib\\SDL2-2.0.8-MinGW")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++ (Does nothing yet)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(SDL2_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")
  endif()
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} include libs)

file(GLOB source_files "*.h" "*.cpp" "src/*.cpp" "include/*.h")
file(GLOB_RECURSE lib_files "libs/*.h" "libs/*.c" "libs/*.cpp")
set(ALL_SRC_FILES ${source_files} ${lib_files})
add_executable(CongbRenderer ${ALL_SRC_FILES})

target_include_directories(CongbRenderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CongbRenderer ${SDL2_LIBRARY})

if(WIN32)
    set_target_properties(CongbRenderer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
    #Copy sdl2 dll file
    add_custom_command(TARGET CongbRenderer POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${SDL2_DLL}
    $<TARGET_FILE_DIR:CongbRenderer>)
endif()
