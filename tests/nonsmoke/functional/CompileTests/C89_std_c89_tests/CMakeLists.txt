set(files_to_test
  test2003_12.c test2004_20.c test2005_172.c test2006_132.c test2006_48.c
  test2007_177.c test2008_01.c test2008_02.c test2009_08.c test2009_07.c
  test2009_02.c test2009_22.c test2009_01.c test2009_21.c test2009_04.c
  test2009_16.c test2010_14.c test2010_10.c test2010_01.c test2011_05.c
  test2010_15.c test2010_17.c test2010_11.c test2012_07.c test2012_153.c
  test2012_30.c test2012_51.c test2012_68.c test2012_84.c test2012_05.c
  test2012_148.c test2012_175.c test2012_49.c test2012_67.c test2012_83.c
  test2012_99.c test2012_11.c test2012_158.c test2012_35.c test2012_61.c
  test2012_70.c test2012_86.c test2012_127.c test2012_165.c test2012_47.c
  test2012_62.c test2012_71.c test2012_87.c test2012_140.c test2012_170.c
  test2012_48.c test2012_65.c test2012_81.c test2012_93.c test2012_08.c
  test2012_154.c test2012_34.c test2012_53.c test2012_69.c test2012_85.c
  test2013_05.c test2013_06.c test2013_22.c test2013_24.c test2013_25.c
  test2013_26.c test2013_29.c test2013_30.c test2013_37.c test2013_38.c
  test2013_39.c test2013_52.c test2013_55.c test2013_62.c test2013_64.c
  test2013_67.c test2013_68.c test2013_69.c test2013_71.c test2013_72.c
  test2013_78.c commentTest.c testAnsiC.c gconv_info.c
  test_CplusplusMacro_C.c math.c testCvsCpp.c stdio.c
  YardenPragmaPackExample.c)

set(ROSE_FLAGS -std=c89)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND ROSE_FLAGS --edg:no_warnings --edg:restrict)
endif()
list(APPEND ROSE_FLAGS -w)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} C89TEST TRUE)
endforeach()
