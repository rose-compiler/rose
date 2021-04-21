set(CUDA_NO_BUILT_IN_CODES test_2011_01.cu test_2011_02.cu)
set(CUDA_CODES test_2011_11.cu test_2011_12.cu test_2011_13.cu)

set(ROSE_FLAGS -rose:skipfinalCompileStep)
set(files_to_test ${CUDA_NO_BUILT_IN_CODES} ${CUDA_CODES})

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} CUDA TRUE)
endforeach()
