include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

BUILT_SOURCES=testSupport

testSupport:
	$(MAKE) -C $(top_builddir)/tests/nonsmoke/functional/testSupport/

bin_PROGRAMS=sageInterfaceTest sageBuilderTest

sageInterfaceTest_SOURCES= \
  typesAreEqualUnitTests.cpp \
  simpleExpressionEvaluatorUnitTests.cpp

sageInterfaceTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageInterfaceTest_LDFLAGS = \
  $(ROSE_RPATHS)

sageInterfaceTest_LDADD= \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest_main.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest.la \
  $(ROSE_LIBS)


sageBuilderTest_SOURCES= \
  sageBuilderBuildTypes.cpp \
  sageBuilderBuildExpressions.cpp \
  buildExpressionUnitTests.cpp

sageBuilderTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageBuilderTest_LDFLAGS = \
  $(ROSE_RPATHS)

sageBuilderTest_LDADD= \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest_main.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest.la \
  $(ROSE_LIBS)


check-local: sageInterfaceTest sageBuilderTest
	./sageInterfaceTest
	./sageBuilderTest
