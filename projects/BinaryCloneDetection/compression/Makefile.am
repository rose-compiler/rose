# Binary clone detection.  Finds semantically similar functions based on fuzz testing. See Andreas and Earl for details.
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
noinst_PROGRAMS=
TEST_TARGETS=

#-----------------------------------------------------------------------------------------------------------------------------
# Level 00-09: initializing database (non-incremental)
noinst_PROGRAMS += 00-split-file-into-functions
00_split_file_into_functions_SOURCES = 00-split-file-into-functions.C
00_split_file_into_functions_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)/../semantic
00_split_file_into_functions_LDADD = $(builddir)/../semantic/libCloneDetection.la $(BOOST_LDFLAGS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)


#-----------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)

