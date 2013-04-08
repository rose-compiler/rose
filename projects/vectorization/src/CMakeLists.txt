
add_executable(vectorization
	SIMDAnalysis.C
	main.C
	normalization.C
	vectorization.C
)
target_link_libraries(vectorization ROSE_DLL ${Boost_LIBRARIES})

