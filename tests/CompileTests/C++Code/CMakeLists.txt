set(ROSE_FLAGS --edg:no_warnings -w)
set(C_ONLY -rose:C_only)

if(valgrind)
  # Test for Old-Style C function definitions
  add_test(
    NAME test2004_19
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS} -rose:C_only
            -c ${CMAKE_CURRENT_SOURCE_DIR}/test2004_19.C
  )

  # Test for Old-Style C function definitions
  add_test(
    NAME test2004_21
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS} ${C_ONLY}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/test2004_21.c
  )

  add_test(
    NAME test2005_56
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS}
            -DTEST_STRING_MACRO=""Thu Apr 14 08:18:33 PDT 2005""
            ${CMAKE_CURRENT_SOURCE_DIR}/test2005_56.C -o test2005_56
  )

  add_test(
    NAME test2005_168
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS} -rose:C_only
            -rose:verbose 0 -c
            -I${CMAKE_SOURCE_DIR}/src/util/commandlineProcessing
            ${CMAKE_CURRENT_SOURCE_DIR}/test2005_168.c
  )

  # Test for Old-Style C function definitions
  #test2005_172.o: ${CMAKE_CURRENT_SOURCE_DIR}/test2005_172.c
  #	rm -f test2005_172
  #	${valgrind} testTranslator ${ROSE_FLAGS} -rose:verbose 0 ${C_ONLY} ${CMAKE_CURRENT_SOURCE_DIR}/test2005_172.c -o test2005_172
  #	rm -f test2005_172.o
  #	${valgrind} testTranslator ${ROSE_FLAGS} -rose:verbose 0 ${C_ONLY} -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_172.c
  #	rm -f test2005_172
  #	${valgrind} testTranslator ${ROSE_FLAGS} -rose:verbose 0 ${C_ONLY} test2005_172.o -o test2005_172

  # This code requires access to Python.h
  # (in the ${CMAKE_CURRENT_SOURCE_DIR}/pythonDir directory)
  add_test(
    NAME test2005_54
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS}
            -I${CMAKE_CURRENT_SOURCE_DIR}/pythonDir
            -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_54.C
  )

  add_test(
    NAME test2005_64
    COMMAND ${valgrind} testTranslator ${ROSE_FLAGS} -fno-implicit-templates
            -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_64.C
  )

endif()

add_test(
  NAME gconv_info_Cpp
  COMMAND	testTranslator ${ROSE_FLAGS} -rose:verbose 0 -c
          -I${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/gconv_info.c
)

add_test(
  NAME gconv_info_C
  COMMAND testTranslator ${ROSE_FLAGS} -rose:verbose 0 -rose:C_only -c
          -I${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/gconv_info.c
)

add_test(
  NAME gconv_info_C99
  COMMAND testTranslator ${ROSE_FLAGS} -rose:verbose 0 -rose:C99_only -c
          -I${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/gconv_info.c
)

add_test(
  NAME stdio_Cpp
  COMMAND testTranslator ${ROSE_FLAGS} -rose:verbose 0
          -I${CMAKE_CURRENT_SOURCE_DIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/stdio.c
)

add_test(
  NAME stdio_C
  COMMAND testTranslator ${ROSE_FLAGS} -rose:verbose 0 -rose:C
          -I${CMAKE_CURRENT_SOURCE_DIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/stdio.c
)

add_test(
  NAME stdio_C99
  COMMAND testTranslator ${ROSE_FLAGS} -rose:verbose 0 -rose:C99_only
          -I${CMAKE_CURRENT_SOURCE_DIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/stdio.c
)

add_test(
  NAME test_CplusplusMacro_Cpp
  COMMAND testTranslator ${ROSE_FLAGS}
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test_CplusplusMacro_Cpp.C
)

add_test(
  NAME test_CplusplusMacro_C
  COMMAND testTranslator ${ROSE_FLAGS} -rose:C_only
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test_CplusplusMacro_C.C
)

# Test of export keyword
add_test(
  NAME test2004_30
  COMMAND testTranslator ${ROSE_FLAGS} --edg:restrict --edg:export
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2004_30.C
)

# Test of simple STL (list) example code
add_test(
  NAME test2004_41
  COMMAND testTranslator ${ROSE_FLAGS} --edg:restrict --edg:export
          -I../g++_HEADERS/hdrs1/bits
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2004_41.C
)

set(files_to_test
  test2001_11.C test2001_12.C test2001_13.C test2001_16.C test2004_35.C
  test2004_65.C test2004_67.C test2001_01.C test2001_02.C test2001_04.C
  test2001_05.C test2001_06.C test2001_07.C test2001_08.C test2001_10.C
  test2001_17.C test2001_18.C test2001_19.C test2001_20.C test2001_21.C
  test2001_22.C test2001_23.C test2001_26.C test2001_28.C test2001_29.C
  test2003_02.C test2003_03.C test2003_04.C test2003_05.C test2003_06.C
  test2003_08.C test2003_09.C test2003_10.C test2003_11.C test2003_29.C
  test2003_30.C test2003_31.C test2003_32.C test2003_35.C test2003_36.C
  test2003_37.C test2004_01.C test2004_02.C test2004_03.C test2004_04.C
  test2004_05.C test2004_07.C test2004_08.C test2004_09.C test2004_11.C
  test2004_12.C test2004_13.C test2004_14.C test2004_15.C test2004_16.C
  test2004_17.C test2004_18.C test2004_23.C test2004_26.C test2004_27.C
  test2004_28.C test2004_31.C test2004_32.C test2004_33.C
  test2004_34.C test2004_36.C test2004_37.C test2004_38.C test2004_39.C
  test2004_40.C test2004_43.C test2004_44.C test2004_45.C test2004_47.C
  test2004_48.C test2004_49.C test2004_52.C test2004_58.C test2004_60.C
  test2004_64.C test2004_70.C test2004_71.C test2004_72.C test2004_73.C
  test2004_74.C test2004_75.C test2004_76.C test2004_77.C test2004_78.C
  test2004_79.C test2004_80.C test2004_81.C test2004_82.C test2004_84.C
  test2004_85.C test2004_86.C test2004_87.C test2004_88.C test2004_89.C
  test2004_90.C test2004_91.C test2004_93.C test2004_94.C test2004_95.C
  test2004_96.C test2004_97.C test2004_98.C test2004_99.C test2004_100.C
  test2004_101.C test2004_102.C test2004_103.C test2004_104.C test2004_105.C
  test2004_106.C test2004_107.C test2004_108.C test2004_109.C test2004_110.C
  test2004_111.C test2004_112.C test2004_113.C test2004_114.C test2004_115.C
  test2004_116.C test2004_117.C test2004_118.C test2004_119.C test2004_120.C
  test2004_121.C test2004_122.C test2004_123.C test2004_124.C test2004_125.C
  test2004_127.C test2004_128.C test2004_129.C test2004_130.C test2004_131.C
  test2004_132.C test2004_133.C test2004_134.C test2004_135.C test2004_136.C
  test2004_137.C test2004_138.C test2004_139.C test2004_140.C test2004_141.C
  test2004_142.C test2004_143.C test2004_144.C test2004_145.C test2004_146.C
  test2004_147.C test2004_148.C test2004_149.C test2004_150.C test2004_151.C
  test2004_152.C test2004_155.C test2004_156.C test2004_157.C test2004_158.C
  test2004_159.C test2004_160.C test2004_161.C test2004_162.C test2004_163.C
  test2004_164.C test2005_02.C test2005_03.C test2005_04.C test2005_05.C
  test2005_06.C test2005_07.C test2005_08.C test2005_09.C test2005_10.C
  test2005_11.C test2005_12.C test2005_13.C test2005_14.C test2005_15.C
  test2005_16.C test2005_17.C test2005_19.C test2005_20.C test2005_21.C
  test2005_22.C test2005_23.C test2005_24.C test2005_25.C test2005_26.C
  test2005_27.C test2005_28.C test2005_29.C test2005_30.C test2005_31.C
  test2005_32.C test2005_33.C test2005_34.C test2005_35.C test2005_36.C
  test2005_37.C test2005_38.C test2005_39.C test2005_40.C test2005_41.C
  test2005_42.C test2005_43.C test2005_44.C test2005_45.C test2005_46.C
  test2005_47.C test2005_48.C test2005_49.C test2005_50.C test2005_51.C
  test2005_52.C test2005_53.C test2005_55.C test2005_57.C test2005_58.C
  test2005_59.C test2005_60.C test2005_61.C test2005_62.C test2005_63.C
  test2005_65.C test2005_66.C test2005_67.C test2005_68.C
  test2005_70.C test2005_71.C test2005_72.C test2005_73.C test2005_74.C
  test2005_75a.C test2005_75b.C test2005_76.C test2005_77.C test2005_78.C
  test2005_79.C test2005_80.C test2005_82.C test2005_83.C test2005_84.C
  test2005_86.C test2005_87.C test2005_88.C test2005_89.C test2005_90.C
  test2005_91.C test2005_92.C test2005_94.C test2005_95.C test2005_96.C
  test2005_97.C test2005_99.C test2005_100.C test2005_101.C test2005_102.C
  test2005_103.C test2005_104.C test2005_105.C test2005_106.C test2005_107.C
  test2005_108.C test2005_109.C test2005_110.C test2005_111.C test2005_112.C
  test2005_113.C test2005_114.C test2005_116.C test2005_117.C test2005_118.C
  test2005_119.C test2005_120.C test2005_121.C test2005_122.C test2005_123.C
  test2005_124.C test2005_125.C test2005_126.C test2005_127.C test2005_128.C
  test2005_129.C test2005_130.C test2005_131.C test2005_132.C test2005_133.C
  test2005_134.C test2005_135.C test2005_136.C test2005_137.C test2005_138.C
  test2005_139.C test2005_140.C test2005_141.C test2005_142.C test2005_143.C
  test2005_144.C test2005_145.C test2005_146.C test2005_147.C test2005_148.C
  test2005_149.C test2005_150.C test2005_152.C test2005_153.C test2005_154.C
  test2005_155.C test2005_156.C test2005_157.C test2005_158.C test2005_159.C
  test2005_160.C test2005_161.C test2005_162.C test2005_163.C test2005_164.C
  test2005_165.C test2005_166.C test2005_167.C test2005_169.C test2005_170.C
  test2005_171.C test2005_173.C test2005_174.C test2005_175.C test2005_176.C
  test2005_177.C test2005_178.C test2005_179.C test2005_180.C test2005_181.C
  test2005_182.C test2005_183.C test2005_184.C test2005_185.C test2005_187.C
  test2005_188.C test2005_190.C test2005_191.C test2005_192.C test2005_193.C
  test2005_194.C test2005_195.C test2005_196.C test2005_197.C test2005_198.C
  test2005_199.C test2005_203.C test2005_205.C test2005_206.C test2005_207.C
  test2005_208.C test2005_209.C test2006_01.C test2006_02.C test2006_03.C
  test2006_04.C test2006_05.C test2006_06.C test2006_07.C test2006_08.C
  test2006_56.C test2006_61.C test2006_62.C test2006_63.C test2006_66.C
  test2006_67.C test2006_68.C test2006_69.C test2006_70.C test2006_71.C
  test2006_72.C test2006_73.C test2006_74.C test2006_75.C test2006_76.C
  test2006_77.C test2006_78.C test2006_79.C test2006_80.C test2006_81.C
  test2006_82.C test2006_83.C test2006_84.C lexPhase2003_01.C)

list(APPEND ROSE_FLAGS
  -I${CMAKE_SOURCE_DIR}/tests/CompileTests/A++Code
  -I${CMAKE_CURRENT_SOURCE_DIR}
)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test})
endforeach()
