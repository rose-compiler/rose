if(EDG_VERSION STREQUAL "4.3")
  set(files_to_test
    test2005_186.c
    test2006_127.c
    test2006_143.c
    test2008_01.c
    complex_01.c
    bool.c
    constants.c
  )
else()
  set(files_to_test
    test2005_186.c
    test2006_127.c
    test2006_143.c
    test2008_01.c
    complex_01.c
    complex_03.c
    bool.c
    constants.c
  )
endif()

# the following tests currently fail
# complex_02.c
# complex_04.c

set(ROSE_FLAGS -rose:verbose 0 -rose:C99 --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} C99TEST)
endforeach()
