set(TESTCODES dC0010.c dC0011.c dC0013.c dC0017.c dC0018.c)
set(ROSE_FLAGS -rose:C --edg:no_warnings -w)

foreach(file_to_test ${TESTCODES})
  compile_test(${file_to_test} ELSATEST)
endforeach()
