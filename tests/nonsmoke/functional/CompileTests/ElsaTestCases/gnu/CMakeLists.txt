set(TESTCODES_1 d0085.cc)
set(TESTCODES_2 asm01.c)

set(ROSE_FLAGS --edg:no_warnings -w)
foreach(file_to_test ${TESTCODES_1})
  compile_test(${file_to_test} ELSATEST)
endforeach()

list(APPEND ROSE_FLAGS -rose:C)
foreach(file_to_test ${TESTCODES_2})
  compile_test(${file_to_test} ELSATEST)
endforeach()
