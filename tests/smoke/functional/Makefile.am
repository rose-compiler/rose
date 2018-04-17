include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
SUBDIRS += BinaryAnalysis
endif

# Rasmussen (11.17.2017): This test is no longer needed as the Fortran parse table
# has been moved to the source tree.  See comment in ./Fortran/Makefile.am.
#if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#SUBDIRS += Fortran
#endif
