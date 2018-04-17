set(files_to_test
  testRoseHeaders_00.C testRoseHeaders_01.C testRoseHeaders_02.C
  testRoseHeaders_03.C testRoseHeaders_04.C testRoseHeaders_05.C
  testRoseHeaders_06.C testRoseHeaders_07.C testRoseHeaders_08.C)

set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test})
endforeach()
