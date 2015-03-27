set(ROSE_FLAGS -rose:C89)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND ROSE_FLAGS --edg:no_warnings --edg:restrict)
endif()
list(APPEND ROSE_FLAGS -w)

set(files_to_test
  test_CplusplusMacro_C.c gconv_info.c testCvsCpp.c testAnsiC.c
  YardenPragmaPackExample.c commentTest.c test_multiline_string_01.c
  test2003_12.c test2004_20.c test2005_172.c test2005_186.c test2006_110.c
  test2006_114.c test2006_133.c test2006_135.c test2007_177.c test2008_01.c
  test2008_02.c test2009_01.c test2009_02.c test2009_03.c test2009_04.c
  test2009_07.c test2009_08.c test2009_16.c test2009_21.c test2009_22.c
  test2010_01.c test2010_06.c test2010_07.c test2010_08.c test2010_09.c
  test2010_13.c test2010_15.c test2010_17.c test2011_05.c test2012_01.c
  test2012_02.c test2012_03.c test2012_04.c test2012_05.c test2012_06.c
  test2012_07.c test2012_08.c test2012_09.c test2012_10.c test2012_11.c
  test2012_12.c test2012_13.c test2012_14.c test2012_15.c test2012_16.c
  test2012_17.c test2012_18.c test2012_19.c test2012_20.c test2012_21.c
  test2012_22.c test2012_23.c test2012_24.c test2012_25.c test2012_26.c
  test2012_27.c test2012_28.c test2012_29.c test2012_30.c test2012_32.c
  test2012_33.c test2012_34.c test2012_35.c test2012_36.c test2012_37.c
  test2012_38.c test2012_39.c test2012_40.c test2012_42.c test2012_41.c
  test2012_43.c test2012_46.c test2012_47.c test2012_48.c test2012_49.c
  test2012_50.c test2012_52.c test2012_53.c test2012_54.c
  test2012_55.c test2012_56.c test2012_57.c test2012_58.c test2012_59.c
  test2012_60.c test2012_61.c test2012_62.c test2012_63.c test2012_64.c
  test2012_65.c test2012_66.c test2012_67.c test2012_68.c test2012_69.c
  test2012_70.c test2012_71.c test2012_72.c test2012_73.c test2012_74.c
  test2012_75.c test2012_76.c test2012_77.c test2012_79.c test2012_80.c
  test2012_81.c test2012_82.c test2012_83.c test2012_84.c test2012_85.c
  test2012_86.c test2012_87.c test2012_88.c test2012_89.c test2012_90.c
  test2012_91.c test2012_92.c test2012_93.c test2012_94.c test2012_95.c
  test2012_96.c test2012_97.c test2012_98.c test2012_99.c test2012_100.c
  test2012_101.c test2012_102.c test2012_103.c test2012_104.c test2012_105.c
  test2012_106.c test2012_107.c test2012_108.c test2012_109.c test2012_110.c
  test2012_111.c test2012_112.c test2012_113.c test2012_114.c test2012_115.c
  test2012_116.c test2012_117.c test2012_118.c test2012_119.c test2012_120.c
  test2012_121.c test2012_122.c test2012_123.c test2012_124.c test2012_125.c
  test2012_126.c test2012_127.c test2012_129.c test2012_130.c test2012_131.c
  test2012_133.c test2012_134.c test2012_135.c test2012_136.c test2012_137.c
  test2012_139.c test2012_140.c test2012_141.c test2012_142.c test2012_143.c
  test2012_144.c test2012_146.c test2012_147.c test2012_148.c test2012_149.c
  test2012_150.c test2012_151.c test2012_152.c test2012_153.c test2012_154.c
  test2012_156.c test2012_157.c test2012_158.c test2012_159.c test2012_162.c
  test2012_163.c test2012_164.c test2012_165.c test2012_166.c test2012_167.c
  test2012_168.c test2012_169.c test2012_170.c test2012_171.c test2012_172.c
  test2012_173.c test2012_174.c test2012_175.c test2013_01.c test2013_02.c
  test2013_03.c test2013_05.c test2013_06.c test2013_07.c test2013_08.c
  test2013_09.c test2013_10.c test2013_11.c test2013_12.c test2013_13.c
  test2013_14.c test2013_15.c test2013_16.c test2013_17.c test2013_18.c
  test2013_19.c test2013_20.c test2013_21.c test2013_22.c test2013_23.c
  test2013_24.c test2013_25.c test2013_26.c test2013_27.c test2013_28.c
  test2013_29.c test2013_30.c test2013_32.c test2013_33.c
  test2013_34.c test2013_35.c test2013_36.c test2013_37.c test2013_38.c
  test2013_39.c test2013_40.c test2013_41.c test2013_42.c test2013_43.c
  test2013_44.c test2013_45.c test2013_46.c test2013_47.c test2013_48.c
  test2013_49.c test2013_50.c test2013_51.c test2013_52.c test2013_53.c
  test2013_54.c test2013_55.c test2013_56.c test2013_57.c test2013_58.c
  test2013_59.c test2013_60.c test2013_61.c test2013_62.c test2013_63.c
  test2013_64.c test2013_65.c test2013_66.c test2013_67.c test2013_68.c
  test2013_69.c test2013_70.c test2013_71.c test2013_72.c test2013_73.c
  test2013_74.c test2013_75.c test2013_76.c test2013_77.c test2013_78.c
  test2013_79.c test2013_80.c test2013_81.c test2013_82.c test2013_83.c
  test2013_84.c test2013_85.c test2013_86.c test2013_87.c test2013_88.c
  test2013_89.c test2013_90.c test2013_91.c test2013_92.c test2013_93.c
  test2013_94.c test2013_95.c test2013_96.c test2013_97.c test2013_98.c
  test2013_99.c test2013_100.c math.c complex_01.c stdio.c
  test2006_48.c test2006_132.c test2010_04.c test2010_10.c test2010_12.c
  test2010_11.c test2010_14.c constants.c
  #EXPECTED FAILURES: test2012_51.c test2013_31.c  
)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND files_to_test test2009_05.c test2009_17.c)
endif()

# DQ (9/26/2009): Conditionally compile this file (fails in OSX due to lack of
# OSX  support for thread local storage). Liao know more about the details of
# this test.
if(NOT APPLE)
  list(APPEND files_to_test test2009_19.c)
endif()

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test})
endforeach()

list(APPEND ROSE_FLAGS
  -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/
  -I${CMAKE_SOURCE_DIR}/src/util/commandlineProcessing)
compile_test(test2005_168.c)
