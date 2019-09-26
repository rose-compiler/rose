add_executable(generateVirtualCFG generateVirtualCFG.C)
target_link_libraries(generateVirtualCFG ROSE_DLL EDG ${link_with_libraries})
install(TARGETS generateVirtualCFG DESTINATION bin)

add_executable(testVirtualCFG testVirtualCFG.C)
target_link_libraries(testVirtualCFG ROSE_DLL EDG ${link_with_libraries})

# Some of these test codes reference A++ header fiels as part of their tests
# Include the path to A++ and the transformation specification
set(TESTCODE_INCLUDES
  -I${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/A++Code
  -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII)

set(ROSE_FLAGS
  --edg:no_warnings -w -rose:verbose 0 --edg:restrict)

# This populates the list ROSE__CXX_TESTS
include(${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests/Cxx_Testcodes.cmake)

foreach(file_to_test ${ROSE__CXX_TESTS})
  add_test(
    NAME testVirtualCFG_CXX_${file_to_test}
    COMMAND testVirtualCFG ${ROSE_FLAGS}
    -I${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests ${TESTCODE_INCLUDES}
    -c ${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests/${file_to_test})
endforeach()

set(C_FILES
  test_CplusplusMacro_C.C test2001_02.C test2001_03.C test2001_05.C
  test2001_07.C test2001_08.C test2001_09.C test2001_14.C test2001_17.C
  test2001_23.C test2001_24.C test2001_25.C test2001_26.C test2002_01.C
  test2003_04.C test2003_05.C test2003_06.C test2003_10.C test2003_11.C
  test2003_13.C test2003_14.C test2003_15.C test2003_16.C test2003_18.C
  test2003_19.C test2003_20.C test2003_21.C test2003_23.C test2003_24.C
  test2003_25.C test2003_26.C test2003_29.C test2003_30.C test2003_31.C
  test2003_32.C test2003_35.C test2003_36.C test2004_03.C test2004_04.C
  test2004_05.C test2004_13.C test2004_14.C test2004_19.C test2004_23.C
  test2004_28.C test2004_29.C test2004_49.C test2004_53.C test2004_60.C
  test2004_61.C test2004_72.C test2004_74.C test2004_94.C test2004_96.C
  test2004_102.C test2004_105.C test2004_107.C test2004_108.C test2004_114.C
  test2004_118.C test2004_131.C test2004_152.C test2004_153.C test2004_154.C
  test2004_163.C test2005_16.C test2005_20.C test2005_21.C test2005_22.C
  test2005_23.C test2005_26.C test2005_48.C test2005_62.C test2005_84.C
  test2005_92.C test2005_95.C test2005_100.C test2005_111.C test2005_116.C
  test2005_125.C test2005_157.C test2005_159.C test2005_164.C test2005_165.C
  test2005_169.C test2005_170.C test2005_176.C test2005_180.C test2005_182.C
  test2005_184.C test2005_187.C test2005_191.C test2005_198.C test2005_202.C
  test2005_209.C test2006_07.C test2006_10.C test2006_12.C test2006_14.C
  test2006_16.C test2006_23.C test2006_34.C test2006_40.C test2006_50.C
  test2006_52.C test2006_53.C test2006_55.C test2006_56.C test2006_57.C
  test2006_62.C test2006_69.C test2006_75.C test2006_76.C test2006_82.C
  test2006_83.C test2006_88.C test2006_91.C test2006_93.C test2006_94.C
  test2006_97.C test2006_99.C test2006_103.C test2006_104.C test2006_105.C
  test2006_106.C test2006_107.C test2006_108.C test2006_109.C test2006_111.C
  test2006_113.C test2006_116.C test2006_137.C test2006_140.C test2006_142.C
  test2006_145.C test2006_146.C test2006_147.C test2006_148.C test2006_149.C
  test2006_150.C test2006_151.C test2006_152.C test2006_153.C test2006_154.C
  test2006_181.C test_int_lexemes.C test_int_lexemes_donot_pass.C
  test_simple_int.C test_wchars.C X.C)

foreach(input_file ${C_FILES})
  get_filename_component(base_filename "${input_file}" NAME_WE)
  set(file_to_test "${base_filename}.temp.c")
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests/${input_file}
    ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test}
    COPYONLY)
  add_test(
    NAME testVirtualCFG_C_${file_to_test}
    COMMAND testVirtualCFG ${ROSE_FLAGS}
      -I${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests ${TESTCODE_INCLUDES}
      -c ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test})
endforeach()

set(C99_FILES
  bool.c complex_01.c complex_03.c constants.c test2005_186.c test2006_127.c
  test2006_143.c test2008_01.c)

foreach(file_to_test ${C99_FILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../C99_tests/${file_to_test}
    ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test}
    COPYONLY)
  add_test(
    NAME testVirtualCFG_C99_${file_to_test}
    COMMAND testVirtualCFG ${ROSE_FLAGS}
      -I${CMAKE_CURRENT_SOURCE_DIR}/../C99_tests ${TESTCODE_INCLUDES}
      -c ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test})
endforeach()

if(enable-java)
  set(JAVA_FILES
    test2011_01.java test2011_02.java test2011_03.java test2011_06.java
    test2011_07.java test2011_08.java test2011_09.java test2011_10.java
    test2011_11.java test2011_12.java test2011_13.java test2011_14.java
    test2011_15.java test2011_16.java test2011_17.java 
    test2011_19.java test2011_26.java test2011_29.java
    test2011_30.java test2011_31.java test2011_32.java test2011_33.java
    test2011_34.java test2011_35.java test2011_36.java test2011_37.java
    test2011_38.java test2011_39.java test2011_40.java test2011_41.java
    test2011_44.java test2011_47.java 
    test2011_50.java pgc_test2011_03.java pgc_test2011_08.java
    cave3_IntLit0.java cave3_LoadDep0.java cave3_assert0.java cave3_assert1.java
    cave3_block0.java cave3_call0.java cave3_call1.java cave3_constructor0.java
    cave3_constructor1.java cave3_constructor2.java cave3_constructor3.java
    cave3_DoubleLit0.java cave3_dowhile0.java cave3_for0.java cave3_for1.java
    cave3_for2.java cave3_for3.java cave3_for4.java cave3_for5.java
    cave3_for6.java cave3_for7.java cave3_for8.java cave3_foreach0.java
    cave3_foreach1.java cave3_if0.java cave3_if1.java
    cave3_labelDecl0.java cave3_labelDecl1.java cave3_labelDecl2.java
    cave3_labelDecl3.java cave3_labelBreak0.java cave3_labelBreak1.java
    cave3_labelBreak2.java cave3_labelBreak5.java
    cave3_labelContinue0.java cave3_labelContinue1.java cave3_labelContinue2.java
    cave3_labelContinue3.java cave3_labelContinueBreak0.java
    cave3_labelContinueBreak1.java cave3_mthParam0.java cave3_nameResolution0.java
    cave3_rshift0.java cave3_rshift1.java cave3_synchronized0.java cave3_while0.java
    cave3_while1.java cave3_while2.java cave3_while3.java cave3_while4.java)

  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
  list(APPEND TESTCODES_REQUIRED_TO_PASS 
      test2011_18.java test2011_20.java 
      test2011_43.java test2011_46.java test2011_48.java test2011_49.java
      pgc_test2011_09.java 
      cave3_foreach2.java cave3_foreach4.java cave3_foreach5.java
      cave3_foreach6.java cave3_foreach7.java cave3_labelBreak4.java)
  endif()

  foreach(file_to_test ${JAVA_FILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/../Java_tests/${file_to_test}
      ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test}
      COPYONLY)
    add_test(
      NAME testVirtualCFG_Java_${file_to_test}
      COMMAND testVirtualCFG ${ROSE_FLAGS}
        -c ${CMAKE_CURRENT_BINARY_DIR}/${file_to_test})
  endforeach()
endif()


if(enable-fortran)
  set(F90_FILES
    test2007_01.f90 test2007_02.f90 test2007_03.f90 test2007_04.f90
    test2007_05.f90 test2007_06.f90 test2007_07.f90 test2007_08.f90
    test2007_09.f90 test2007_10.f90 test2007_11.f90 test2007_12.f90
    test2007_13.f90 test2007_14.f90 test2007_15.f90 test2007_17.f90
    test2007_20.f90 test2007_21.f90 test2007_22.f90 test2007_23.f90
    test2007_24.f90 test2007_25.f90 test2007_26.f90 test2007_27.f90
    test2007_28.f90 test2007_29.f90 test2007_32.f90 test2007_33.f90
    test2007_34.f90 test2007_36.f90 test2007_38.f90 test2007_39.f90
    test2007_40.f90 test2007_48.f90 test2007_49.f90 test2007_50.f90
    test2007_51.f90 test2007_52.f90 test2007_53.f90 test2007_54.f90
    test2007_55.f90 test2007_56.f90 test2007_57.f90 test2007_58.f90
    test2007_59.f90 test2007_60.f90 test2007_62.f90 test2007_63.f90
    test2007_64.f90 test2007_65.f90 test2007_66.f90 test2007_67.f90
    test2007_68.f90 test2007_69.f90 test2007_71.f90 test2007_72.f90
    test2007_73.f90 test2007_74.f90 test2007_75.f90 test2007_76.f90
    test2007_77.f90 test2007_78.f90 test2007_79.f90 test2007_80.f90
    test2007_89.f90 test2007_90.f90 test2007_91.f90 test2007_92.f90
    test2007_93.f90 test2007_95.f90 test2007_96.f90 test2007_97.f90
    test2007_100.f90 test2007_102.f90 test2007_103.f90 test2007_104.f90
    test2007_105.f90 test2007_106.f90 test2007_110.f90 test2007_117.f90
    test2007_118.f90 test2007_119.f90 test2007_121.f90 test2007_122.f90
    test2007_126.f90 test2007_135.f90 test2007_136.f90 test2007_137.f90
    test2007_138.f90 test2007_139.f90 test2007_140.f90 test2007_141.f90
    test2007_143.f90 test2007_149.f90 test2007_158.f90 test2007_159.f90
    test2007_160.f90 test2007_161.f90 test2007_169.f90 test2007_178.f90
    test2007_179.f90 test2007_181.f90 test2007_182.f90 test2007_190.f90
    test2007_236.f90 test2007_238.f90 test2007_239.f90 test2007_244.f90
    test2007_248.f90 test2007_249.f90 test2007_250.f90 test2007_262.f90
    canonicalExample.f90)

  foreach(file_to_test ${F90_FILES})
    add_test(
      NAME testVirtualCFG_F90_${file_to_test}
      COMMAND testVirtualCFG ${ROSE_FLAGS} -rose:f90
        -c ${CMAKE_CURRENT_SOURCE_DIR}/../Fortran_tests/${file_to_test})
  endforeach()

  set(F77_FILES
    test2007_129.f test2007_132.f test2007_148.f test2007_150.f
    test2007_151.f test2007_153.f test2007_154.f test2007_155.f
    test2007_156.f test2007_162.f test2007_163.f test2007_164.f
    test2007_175.f test2007_198.f test2007_200.f test2007_201.f
    test2007_202.f test2007_204.f test2007_205.f test2007_206.f
    test2007_207.f test2007_208.f test2007_211.f test2007_212.f
    test2007_213.f test2007_214.f test2007_215.f test2007_216.f
    test2007_217.f test2007_218.f test2007_220.f test2007_221.f
    test2007_222.f test2007_223.f test2007_224.f test2007_226.f
    test2007_228.f test2007_229.f test2007_230.f test2007_231.f
    test2007_232.f test2007_234.f test2007_242.f test2007_243.f
    test2007_245.f test2007_246.f test2007_247.f test2007_251.f
    test2007_253.f test2007_259.f test2007_260.f test2007_261.f)

  foreach(file_to_test ${F77_FILES})
    add_test(
      NAME testVirtualCFG_F77_${file_to_test}
      COMMAND testVirtualCFG ${ROSE_FLAGS} -rose:f77
        -c ${CMAKE_CURRENT_SOURCE_DIR}/../Fortran_tests/${file_to_test})
  endforeach()

  set(F03_FILES
    test2007_31.f03 test2007_37.f03 test2007_41.f03 test2007_42.f03
    test2007_44.f03 test2007_47.f03 test2007_98.f03 test2007_112.f03
    test2007_113.f03 test2007_114.f03 test2007_120.f03 test2007_123.f03
    test2007_134.f03 test2007_174.f03)

  foreach(file_to_test ${F03_FILES})
    add_test(
      NAME testVirtualCFG_F03_${file_to_test}
      COMMAND testVirtualCFG ${ROSE_FLAGS} -rose:skip_syntax_check
        -rose:skipfinalCompileStep -rose:f2003
        -c ${CMAKE_CURRENT_SOURCE_DIR}/../Fortran_tests/${file_to_test})
  endforeach()

endif()
