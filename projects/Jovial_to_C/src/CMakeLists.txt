
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Jovial_to_C
	main.C
	j2c/j2cExpression.C
	j2c/j2cStatement.C
	j2c/j2cType.C
	j2c/j2cArray.C
)
target_link_libraries(Fortran_to_C ROSE_DLL ${Boost_LIBRARIES})

