set(ROSE_FLAGS -rose:C89 -pedantic-errors)

set(files_to_test
   test2003_12.c  test2009_01.c   test2009_21.c   test2012_110.c  test2012_153.c 
   test2012_24.c  test2012_48.c   test2012_68.c   test2012_85.c   test2013_111.c 
   test2013_37.c  test2013_69.c   test2013_86.c   test2013_94.c   test2014_13.c 
   test2014_26.c  test2014_48.c   test2014_86.c   test2004_20.c   test2009_02.c 
   test2009_22.c  test2012_113.c  test2012_154.c  test2012_26.c   test2012_49.c 
   test2012_69.c  test2012_86.c   test2013_113.c  test2013_39.c   test2013_71.c 
   test2013_87.c  test2014_14.c   test2014_34.c   test2014_49.c 
   test2014_92.c  test2005_172.c  test2009_04.c   test2010_01.c   test2012_11.c 
   test2012_158.c test2012_27.c   test2012_61.c   test2012_70.c   test2012_87.c 
   test2013_24.c  test2013_72.c   test2013_89.c   test2013_96.c 
   test2014_15.c  test2014_37.c   test2014_58.c   test2014_94.c   test2007_177.c 
   test2009_07.c  test2010_10.c   test2012_127.c  test2012_165.c  test2012_34.c 
   test2012_62.c  test2012_81.c   test2012_93.c   test2013_26.c   test2013_55.c 
   test2013_78.c  test2013_91.c   test2014_01.c   test2014_16.c   test2014_38.c 
   test2014_70.c  test2008_01.c   test2009_08.c   test2010_17.c   test2012_140.c 
   test2012_170.c test2012_35.c   test2012_65.c   test2012_83.c   test2012_99.c 
   test2013_29.c  test2013_62.c   test2013_82.c   test2013_92.c   test2014_05.c 
   test2014_17.c  test2014_39.c   test2014_83.c   test2008_02.c   test2009_16.c 
   test2011_05.c  test2012_148.c  test2012_22.c   test2012_37.c   test2012_67.c 
   test2012_84.c  test2013_100.c  test2013_30.c   test2013_64.c   test2013_83.c 
   test2013_93.c  test2014_12.c   test2014_21.c   test2014_40.c   test2014_84.c)

# disabled because it currently fails on Windows
set(disabled_tests test2013_95.c)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test})
endforeach()

list(APPEND ROSE_FLAGS
  -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII/
  -I${CMAKE_SOURCE_DIR}/src/util/commandlineProcessing)
