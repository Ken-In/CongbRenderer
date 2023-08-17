cmake_minimum_required(VERSION 3.7)

project(CongbRenderer)

set(CMAKE_CXX_STANDARD 14)

file(GLOB source_files "*.h" "*.cpp" "src/*.cpp" "include/*.h" )
set(ALL_SRC_FILES ${source_files})
add_executable(CongbRenderer ${ALL_SRC_FILES})

target_include_directories(CongbRenderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)