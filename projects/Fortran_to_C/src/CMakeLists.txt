
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Fortran_to_C
	main.C f2c/f2cStatement.C f2c/f2cArray.C
)
target_link_libraries(Fortran_to_C ROSE_DLL ${Boost_LIBRARIES})

