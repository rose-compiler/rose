include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = astCopyReplTest 

astCopyReplTest_SOURCES = astCopyReplTest.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt 
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

