include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir) 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

readWriteSetSources = TestabilityGrader.C \
		      LocalRWSetGenerator.C  ReadWriteSetRecords.C  VxUtilFuncs.C \
		      Name2Node.C            SageUtils.C

noinst_LTLIBRARIES = libReadWriteSet.la
readWriteSet_la_SOURCES = $(readWriteSetSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = LocalRWSetGenerator.h sageUtility.h VxUtilFuncs.h \
		     Name2Node.h SageUtils.h ReadWriteSetRecords.h  TestabilityGrader.h

EXTRA_DIST = CMakeLists.txt
