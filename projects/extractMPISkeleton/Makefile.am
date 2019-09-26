
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if !ROSE_USING_BOOST_VERSION_1_62
SUBDIRS=\
	src \
	tests
endif

EXTRA_DIST=apis.coll mpi_api.spec stdio_input.spec \
           doc/user-manual.tex mpi-fakeheader/mpi.h

check-local:
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

