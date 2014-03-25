set(files_to_test HelloWholeWorld.x10)
set(ROSE_FLAGS)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} TRUE)
endforeach()
