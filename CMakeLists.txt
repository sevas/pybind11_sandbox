project(hello_pybind11)
cmake_minimum_required(VERSION 3.6)

include(${CMAKE_SOURCE_DIR}/_conan/conanbuildinfo.cmake)

find_package(Python COMPONENTS Development)

add_subdirectory(src/lib)
# add_subdirectory(src/app)

