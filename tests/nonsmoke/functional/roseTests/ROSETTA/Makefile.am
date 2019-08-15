include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

BUILT_SOURCES=testSupport

testSupport:
	$(MAKE) -C $(top_builddir)/tests/nonsmoke/functional/testSupport/

bin_PROGRAMS=nodeHierarchyTest

nodeHierarchyTest_SOURCES= \
  symbolTableUnitTests.cpp \
  functionTypeTests.cpp

nodeHierarchyTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport \
  -I$(top_srcdir)/tests/nonsmoke/functional/testSupport/gtest/include \
  $(ROSE_INCLUDES)

nodeHierarchyTest_LDFLAGS = \
  $(ROSE_RPATHS)

nodeHierarchyTest_LDADD= \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest.la \
  $(top_builddir)/tests/nonsmoke/functional/testSupport/gtest/lib/libgtest_main.la \
  $(ROSE_LIBS)

unit-tests:
# DQ (7/2/2018): Remove this test because it used GTest which sometimes fails when 
# running in parallel with Jenkins (rarely, but a point of stability for Jenkins).
# We are unclear why these tests were added (by a student working for us) and they
# are not setup to run in a conventional manner within ROSE (thus the stability issue).
#	./nodeHierarchyTest
	@echo "*******************************************"
	@echo "Removed nodeHierarchyTest test using gTest."
	@echo "*******************************************"

check-local: unit-tests
