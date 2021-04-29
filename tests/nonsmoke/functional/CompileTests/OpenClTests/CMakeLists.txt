set(files_to_test empty.cl)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND files_to_test
    transpose_kernel.cl nbody.cl nbody_cpu.cl nbody_gpu.cl)
endif()

set(ROSE_FLAGS -rose:skipfinalCompileStep)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} OPENCLTEST TRUE)
endforeach()
