include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


bin_PROGRAMS=variableTypeTest functionTypeTest typecounter

variableTypeTest_SOURCES= \
  variableTypeTest.cpp

variableTypeTest_CXXFLAGS= \
  $(ROSE_INCLUDES)

variableTypeTest_LDADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

functionTypeTest_SOURCES= \
  functionTypeTest.cpp

functionTypeTest_CXXFLAGS= \
  $(ROSE_INCLUDES)

functionTypeTest_LDADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)


typecounter_SOURCES= \
  countDifferentTypes.cpp

typecounter_CXXFLAGS= \
  $(ROSE_INCLUDES)

typecounter_LDADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

shell-check:
	$(srcdir)/runVariableTypeTest.sh $(builddir)
	$(srcdir)/runFunctionTypeTest.sh $(builddir)


EXTRA_DIST= \
  inputFiles/types_001.cpp \
  inputFiles/types_002.cpp \
  inputFiles/types_003.cpp \
  inputFiles/types_004.cpp \
  inputFiles/types_005.cpp \
  inputFiles/types_006.cpp \
  inputFiles/types_007.cpp \
  inputFiles/types_008.cpp \
  inputFiles/types_009.cpp \
  inputFiles/types_010.cpp \
  inputFiles/types_011.cpp \
  inputFiles/types_012.cpp \
  inputFiles/types_013.cpp \
  inputFiles/types_014.cpp \
  inputFiles/types_015.cpp \
  inputFiles/types_016.cpp \
  inputFiles/types_017.cpp \
  inputFiles/types_018.cpp \
  inputFiles/types_019.cpp \
  inputFiles/types_020.cpp \
  inputFiles/types_021.cpp \
  inputFiles/types_022.cpp \
  inputFiles/types_023.cpp \
  inputFiles/types_024.cpp \
  inputFiles/types_025.cpp \
  inputFiles/types_026.cpp \
  inputFiles/types_027.cpp \
  inputFiles/types_028.cpp \
  inputFiles/types_029.cpp \
  inputFiles/types_030.cpp \
  inputFiles/types_031.cpp \
  inputFiles/types_032.cpp \
  inputFiles/types_033.cpp \
  inputFiles/types_034.cpp \
  inputFiles/types_035.cpp \
  inputFiles/types_036.cpp \
  inputFiles/types_037.cpp \
  inputFiles/types_038.cpp \
  inputFiles/types_039.cpp \
  inputFiles/types_040.cpp \
  inputFiles/types_041.cpp \
  inputFiles/types_042.cpp \
  inputFiles/types_043.cpp \
  inputFiles/types_044.cpp \
  inputFiles/types_045.cpp \
  inputFiles/types_046.cpp \
  inputFiles/types_047.cpp \
  inputFiles/types_048.cpp \
  inputFiles/types_049.cpp \
  inputFiles/types_050.cpp \
  inputFiles/types_051.cpp \
  inputFiles/types_052.cpp \
  inputFiles/types_053.cpp \
  inputFiles/types_054.cpp \
  inputFiles/types_055.cpp \
  inputFiles/types_056.cpp \
  inputFiles/types_057.cpp \
  inputFiles/types_058.cpp \
  inputFiles/types_059.cpp \
  inputFiles/types_060.cpp \
  inputFiles/types_061.cpp \
  inputFiles/types_062.cpp \
  inputFiles/types_063.cpp \
  inputFiles/types_064.cpp \
  inputFiles/types_065.cpp \
  inputFiles/types_066.cpp \
  inputFiles/types_067.cpp \
  inputFiles/types_068.cpp \
  inputFiles/types_069.cpp \
  inputFiles/types_070.cpp \
  inputFiles/types_071.cpp \
  inputFiles/types_072.cpp \
  inputFiles/expr_001.cpp \
  inputFiles/expr_002.cpp \
  inputFiles/expr_003.cpp \
  inputFiles/expr_004.cpp \
  inputFiles/expr_005.cpp \
  inputFiles/expr_006.cpp \
  inputFiles/expr_007.cpp \
  inputFiles/functions_001.cpp \
  inputFiles/functions_002.cpp \
  inputFiles/functions_003.cpp \
  inputFiles/functions_004.cpp \
  inputFiles/functions_005.cpp \
  inputFiles/functions_006.cpp \
  inputFiles/functions_007.cpp \
  inputFiles/functions_008.cpp \
  inputFiles/functions_009.cpp \
  inputFiles/functions_010.cpp \
  inputFiles/functions_011.cpp \
  inputFiles/functions_012.cpp \
  inputFiles/functions_013.cpp \
  inputFiles/functions_014.cpp \
  inputFiles/functions_015.cpp \
  inputFiles/functions_016.cpp \
  inputFiles/functions_017.cpp 


check-local: variableTypeTest functionTypeTest shell-check
