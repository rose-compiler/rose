set(TESTCODES_REQUIRED_TO_PASS
  test2011_01.java test2011_02.java test2011_03.java test2011_04.java
  test2011_05.java test2011_06.java test2011_07.java test2011_08.java
  test2011_09.java test2011_10.java test2011_11.java test2011_12.java
  test2011_13.java test2011_14.java test2011_15.java test2011_16.java
  test2011_17.java test2011_18.java test2011_19.java test2011_20.java
  test2011_21.java test2011_22.java test2011_23.java test2011_24.java
  test2011_25.java test2011_26.java test2011_27.java test2011_29.java
  test2011_30.java test2011_31.java test2011_32.java test2011_33.java
  test2011_34.java test2011_35.java test2011_36.java test2011_37.java
  test2011_38.java test2011_39.java test2011_40.java test2011_41.java
  test2011_42.java test2011_47.java test2011_50.java)

set(baseline_flags -rose:java:d javac-output-classes -rose:verbose 0)
set(ROSE_FLAGS_SKIP_FINAL_COMPILATION
  -rose:verbose 0 -rose:skipfinalCompileStep)

# make sure javac output dir exists
execute_process(COMMAND ${CMAKE_COMMAND} -E
  make_directory ${CMAKE_BINARY_DIR}/tests/javac-output-classes)

# similar to "compile_test", but no -c
function(java_test input_file)
  get_filename_component(test_name ${input_file} NAME_WE)
  add_test(
    NAME ${test_name}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    COMMAND ${translator}
      ${ROSE_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
  )

  # these tests should fail if javac just prints out its usage info.
  set_tests_properties(${test_name}
    PROPERTIES FAIL_REGULAR_EXPRESSION "Usage: javac")
endfunction()


set(ROSE_FLAGS -rose:java:source 1.4 -rose:java:target 1.4 ${baseline_flags})
foreach(file_to_test ${TESTCODES_REQUIRED_TO_PASS})
  java_test(${file_to_test})
endforeach()

# Tests that pass all but the final compilation using the backend compiler
# (javac)
foreach(file_to_test
        ${TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE})
  add_test(
    NAME ${file_to_test}_step1
    COMMAND testTranslator ${ROSE_FLAGS_SKIP_FINAL_COMPILATION}
            ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  add_test(
    NAME ${file_to_test}_step2
    COMMAND testTranslator -rose:java:source 1.4 -rose:java:target 1.4
            ${ROSE_FLAGS_SKIP_FINAL_COMPILATION}
            ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(${file_to_test}_step2
    PROPERTIES DEPENDS ${file_to_test}_step1)
endforeach()
