set(TESTCODES_REQUIRED_TO_PASS
  test2011_01.java test2011_02.java test2011_03.java test2011_04.java
  test2011_05.java test2011_06.java test2011_07.java test2011_08.java
  test2011_09.java test2011_10.java test2011_11.java test2011_12.java
  test2011_13.java test2011_14.java test2011_15.java test2011_16.java
  test2011_17.java test2011_19.java 
  test2011_21.java test2011_22.java test2011_23.java test2011_24.java
  test2011_25.java test2011_26.java test2011_27.java test2011_29.java
  test2011_30.java test2011_31.java test2011_32.java test2011_33.java
  test2011_34.java test2011_35.java test2011_36.java test2011_37.java
  test2011_38.java test2011_39.java test2011_40.java test2011_41.java
  test2011_42.java test2011_47.java test2011_50.java pgc_2011_30.java
  pgc_test2011_00.java pgc_test2011_01.java pgc_test2011_02.java
  pgc_test2011_03.java pgc_test2011_04.java pgc_test2011_05.java
  pgc_test2011_06.java pgc_test2011_07.java pgc_test2011_08.java
  pgc_test2011_10.java pgc_test2011_11.java pgc_test2011_12.java
  pgc_test2011_13.java pgc_test2011_14.java pgc_test2011_15.java
  pgc_test2011_16.java pgc_test2011_17.java pgc_test2011_18.java
  pgc_test2011_19.java pgc_test2011_20.java pgc_test2011_21.java
  cave3_assert2.java cave3_assert3.java cave3_IntLit0.java
  cave3_DoubleLit0.java cave3_call0.java cave3_call1.java
  cave3_dowhile0.java cave3_LoadDep0.java cave3_block0.java
  cave3_constructor0.java cave3_constructor1.java cave3_constructor2.java
  cave3_constructor3.java cave3_for0.java cave3_for1.java cave3_for2.java
  cave3_for3.java cave3_for4.java cave3_for5.java cave3_for6.java
  cave3_for7.java cave3_for8.java cave3_if0.java cave3_if1.java
  cave3_import0.java cave3_import1.java cave3_instanceof0.java
  cave3_labelDecl0.java cave3_labelDecl1.java cave3_labelDecl2.java
  cave3_labelDecl3.java cave3_labelBreak0.java cave3_labelBreak1.java
  cave3_labelBreak2.java cave3_labelBreak5.java cave3_labelContinue0.java
  cave3_labelContinue1.java cave3_labelContinue2.java cave3_labelContinue3.java
  cave3_labelContinueBreak0.java cave3_labelContinueBreak1.java
  cave3_mthParam0.java cave3_nameResolution0.java cave3_rshift0.java
  cave3_rshift1.java cave3_synchronized0.java cave3_while0.java
  cave3_while1.java cave3_while2.java cave3_while3.java cave3_while4.java)

# These tests pass the compilation using ROSE, all AST tests, and unparsing
# to generate Java code, but fails the final compilation step of the generated
# Java code.
set(TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE
  simple.java)

# This is where the Java generics tests are put (currently not required to pass
# all phases).
set(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6
  test2011_44.java test2011_45.java cave3_assert0.java
  cave3_assert1.java cave3_import2.java cave3_import3.java cave3_foreach0.java
  cave3_foreach1.java cave3_foreach3.java)


if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
list(APPEND TESTCODES_REQUIRED_TO_PASS 
    test2011_18.java test2011_20.java )
list(APPEND TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6 
  test2011_43.java test2011_46.java test2011_48.java test2011_49.java
  pgc_test2011_09.java 
  cave3_foreach2.java cave3_foreach4.java cave3_foreach5.java
  cave3_foreach6.java cave3_foreach7.java cave3_labelBreak4.java)
endif()

set(baseline_flags -rose:java:d javac-output-classes -rose:verbose 0)
set(ROSE_FLAGS_SKIP_FINAL_COMPILATION
  -rose:verbose 0 -rose:skipfinalCompileStep)

# almost an exact duplicate of "compile_test", but no -c
function(java_test input_file)
  get_filename_component(test_name ${input_file} NAME_WE)
  add_test(
    NAME ${test_name}
    COMMAND ${translator}
      ${ROSE_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
  )
endfunction()


# Tests requiring Java 1.6 that should pass.
set(ROSE_FLAGS -rose:java:source 1.6 -rose:java:target 1.6 ${baseline_flags})
foreach(file_to_test ${TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6})
  java_test(${file_to_test})
endforeach()

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
