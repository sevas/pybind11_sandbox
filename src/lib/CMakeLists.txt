add_library(mylib SHARED mylib.cpp mylib.h)

target_include_directories(mylib PRIVATE 
	${CONAN_INCLUDE_DIRS} 
	${Python_INCLUDE_DIR})

target_link_libraries(mylib PRIVATE ${Python_LIBRARY_RELEASE})

if(UNIX)
	# remove the 'lib' prefix as we are generating a python extension module
	set_target_properties(mylib PROPERTIES PREFIX "")
elseif(WIN32)
	# on windows, cpython extension modules cannot be files with .dll extension
	set_target_properties(mylib PROPERTIES SUFFIX .pyd)
endif()



add_custom_command(TARGET mylib POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:mylib> ${CMAKE_SOURCE_DIR}/python/$<TARGET_FILE_NAME:mylib>
)

