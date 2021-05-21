set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0 --edg:restrict)

# the following tests either fail or take too long to run
#test_boost_lambda.C
#test_boost_phoenix_v1.C
#test_boost_phoenix_v2.C

if(EDG_VERSION STREQUAL "4.3")
  set(files_to_test
    test_boost_lambda.C
    test_boost_phoenix_v1.C
  )
  set(translator testFrontEnd)
endif()

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} BOOSTTEST)
endforeach()
