# This tests the ability to read and regnerate the input application and compile it using the backend compiler.
add_executable(newEDGInterfaceTranslator newEDGInterfaceTranslator.C)
target_link_libraries(newEDGInterfaceTranslator ROSE_DLL EDG ${link_with_libraries})

set(TESTCODES test2001_02.C test2001_03.C)

if(with-ROSE_LONG_MAKE_CHECK_RULE)
  list(APPEND TESTCODES
    test_CplusplusMacro_C.C test2001_02.C test2001_03.C test2001_05.C
    test2001_07.C test2001_08.C test2001_09.C test2001_14.C test2001_17.C
    test2001_23.C test2001_24.C test2001_25.C test2002_01.C test2003_04.C
    test2003_05.C test2003_06.C test2003_10.C test2003_11.C test2003_13.C
    test2003_14.C test2003_16.C test2003_18.C test2003_20.C test2003_21.C
    test2003_26.C test2003_29.C test2003_30.C test2003_31.C test2003_32.C
    test2003_35.C test2003_36.C test2004_03.C test2004_04.C test2004_05.C
    test2004_13.C test2004_23.C test2004_28.C test2004_29.C test2004_49.C
    test2004_53.C test2004_60.C test2004_61.C test2004_74.C test2004_94.C
    test2004_105.C test2004_107.C test2004_108.C test2004_114.C test2004_118.C
    test2004_131.C test2004_152.C test2004_153.C test2004_154.C test2005_20.C
    test2005_21.C test2005_22.C test2005_23.C test2005_26.C test2005_48.C
    test2005_62.C test2005_84.C test2005_92.C test2005_95.C test2005_100.C
    test2005_125.C test2005_157.C test2005_159.C test2005_164.C test2005_169.C
    test2005_170.C test2005_176.C test2005_180.C test2005_182.C test2005_184.C
    test2005_187.C test2005_191.C test2005_198.C test2005_202.C test2005_209.C
    test2006_07.C test2006_10.C test2006_12.C test2006_14.C test2006_16.C
    test2006_23.C test2006_34.C test2006_50.C test2006_52.C test2006_55.C
    test2006_56.C test2006_57.C test2006_62.C test2006_69.C test2006_75.C
    test2006_82.C test2006_83.C test2006_93.C test2006_97.C test2006_99.C
    test2006_103.C test2006_106.C test2006_113.C test2006_137.C test2006_140.C
    test2006_142.C test2006_145.C test2006_147.C test2006_148.C test2006_149.C
    test2006_150.C test2006_151.C test2006_154.C test2006_181.C test2008_04.C
    X.C test2003_23.C test2003_24.C test2003_25.C test2004_96.C
    test2004_102.C test2005_16.C test2005_116.C test2006_76.C test2006_146.C
    test2003_12.C test2003_17.C test2005_175.C test2005_177.C test2006_15.C
    test2006_59.C test2006_71.C test2006_98.C test2006_101.C test2006_139.C
    test2006_160.C)

  if(NOT "${EDG_VERSION}" EQUAL "4.3")
    list(APPEND TESTCODES
    test2004_19.C test2005_111.C test2005_165.C test2006_88.C test2001_26.C
    test2003_15.C test2003_19.C test2004_14.C test2004_72.C test2004_163.C
    test2005_56.C test2006_40.C test2006_53.C test2006_91.C test2006_104.C
    test2006_105.C test2006_107.C test2006_108.C test2006_109.C test2006_111.C
    test2006_116.C test_int_lexemes.C test_int_lexemes_donot_pass.C
    test_simple_int.C test_wchars.C
  endif()
endif()

foreach(testcode ${TESTCODES})
  get_filename_component(base_filename ${testcode} NAME_WE)
  set(file_to_test "${base_filename}.c")

  configure_file(
    ${CMAKE_SOURCE_DIR}/../Cxx_tests/${testcode}
    ${CMAKE_SOURCE_DIR}/${file_to_test}
    COPYONLY)

  add_test(
    NAME NEWEDGTEST_${file_to_test}
    COMMAND newEDGInterfaceTranslator ${ROSE_FLAGS}
      -I{CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests
      -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(NEWEDGTEST_${file_to_test} PROPERTIES LABELS NEWEDGTEST)

  add_test(
    NAME NEWEDGTEST_testFrontEnd_${file_to_test}
    COMMAND testFrontEnd ${ROSE_FLAGS}
      -I{CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests
      -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(NEWEDGTEST_testFrontEnd_${file_to_test} PROPERTIES LABELS NEWEDGTEST)
endforeach()

configure_file(
  ${CMAKE_SOURCE_DIR}/../Cxx_tests/test2005_56.C
  ${CMAKE_SOURCE_DIR}/test2005_56.c
  COPYONLY)

add_test(
  NAME NEWEDGTEST_test2005_56.c
  COMMAND newEDGInterfaceTranslator ${ROSE_FLAGS}
    -DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005"
    -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_56.c)
  set_tests_properties(NEWEDGTEST_test2005_56.c PROPERTIES LABELS NEWEDGTEST)
