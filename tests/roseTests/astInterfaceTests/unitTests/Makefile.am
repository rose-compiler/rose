include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

BUILT_SOURCES=testSupport

testSupport:
	$(MAKE) -C $(top_builddir)/tests/testSupport/

bin_PROGRAMS=sageInterfaceTest sageBuilderTest

sageInterfaceTest_SOURCES= \
  typesAreEqualUnitTests.cpp \
  simpleExpressionEvaluatorUnitTests.cpp

sageInterfaceTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/testSupport \
  -I$(top_srcdir)/tests/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageInterfaceTest_LDADD= \
  $(top_builddir)/tests/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/testSupport/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)


sageBuilderTest_SOURCES= \
  sageBuilderBuildTypes.cpp \
  sageBuilderBuildExpressions.cpp \
  buildExpressionUnitTests.cpp

sageBuilderTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/testSupport \
  -I$(top_srcdir)/tests/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageBuilderTest_LDADD= \
  $(top_builddir)/tests/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/testSupport/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)


check-local: sageInterfaceTest sageBuilderTest
	./sageInterfaceTest
	./sageBuilderTest
