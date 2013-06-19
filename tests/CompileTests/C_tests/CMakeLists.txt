set(ROSE_FLAGS -rose:C)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND ROSE_FLAGS --edg:no_warnings --edg:restrict)
endif()
list(APPEND ROSE_FLAGS -w)

set(files_to_test
  test_CplusplusMacro_C.c gconv_info.c testCvsCpp.c testAnsiC.c
  YardenPragmaPackExample.c commentTest.c test_multiline_string_01.c
  test2003_12.c test2004_20.c test2005_172.c test2005_186.c test2006_110.c
  test2006_114.c test2006_133.c test2006_135.c test2008_01.c test2008_02.c
  test2009_01.c test2009_02.c test2009_03.c test2009_04.c test2009_07.c
  test2009_08.c test2009_16.c test2009_21.c test2009_22.c test2010_01.c
  test2010_06.c test2010_07.c test2010_08.c test2010_09.c test2010_10.c
  test2010_13.c test2010_15.c test2010_17.c test2011_05.c test2007_177.c
  math.c complex_01.c constants.c)

# DQ (10/11/2012): Don't process this file within OSX since the socket API is different there.
if (NOT APPLE AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND files_to_test test2012_12_release.c)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND files_to_test
    stdio.c test2006_48.c test2006_132.c test2010_04.c test2006_14.c
    test2006_100.c test2006_136.c test2010_02.c test2010_03.c test2011_03.c
    test2011_04.c Yarden_client.c)
endif()

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if("${EDG_VERSION}" EQUAL "4.3")
    list(APPEND files_to_test test2004_21.c test2005_168.c test2006_65.c)
  else()
    list(APPEND files_to_test
     stdio.c test2006_48.c test2006_132.c test2010_04.c test2010_11.c
     test2010_12.c test2010_14.c)
  endif()
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
