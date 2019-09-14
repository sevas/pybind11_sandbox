project(hello_pybind11)
cmake_minimum_required(VERSION 3.6)


include(${CMAKE_SOURCE_DIR}/${CONAN_BUILD_DIR}/conanbuildinfo.cmake)

if(CMAKE_VERSION LESS 3.12)
	find_package(PythonInterp)
	find_package(PythonLibs)

	if (NOT PYTHONLIBS_FOUND)
		message(ERROR " python not found")

	endif()

	set(Python_INCLUDE_DIR ${PYTHON_INCLUDE_DIRS})
	set(Python_LIBRARY_RELEASE ${PYTHON_LIBRARIES})

else()
	find_package(Python COMPONENTS Development)

	message("Found python : ${Python_EXECUTABLE} ${Python_INTERPRETER_ID}")
endif()

message(${Python_LIBRARY_RELEASE})



add_subdirectory(src/lib)
# add_subdirectory(src/app)

