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

check-local: variableTypeTest functionTypeTest shell-check
