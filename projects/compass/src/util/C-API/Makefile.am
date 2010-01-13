#==========================================================================

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../../compassSupport $(ROSE_INCLUDES)

#==========================================================================
# Auxiliary library.

if ROSE_MPI
#LTCXXCOMPILE = \
#	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
#        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
#        $(AM_CXXFLAGS) $(CXXFLAGS)
#CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
#        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
endif

C_API_SRCS = C-API.C SigBase.C FuncSig.C Recognizer.C
C_API_HDRS = $(C_API_SRCS:.C=.h) ConstSig.h TypeSig.h

noinst_LTLIBRARIES = libC-API.la

libC_API_la_SOURCES = $(C_API_SRCS) $(C_API_HDRS)

#==========================================================================
# Unit test

noinst_PROGRAMS = testC-API

LDADD = ../../compassSupport/libCompass.la \
  libC-API.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testC_API_SOURCES = testC-API.C

test: libC-API.la testC-API$(EXEEXT) $(srcdir)/c-string.api $(srcdir)/c-string.c
	./testC-API$(EXEEXT) --api $(srcdir)/c-string.api -rose:C_only -c $(srcdir)/c-string.c

check-local:
#	DQ (1/10/2009): Comment this out for now to test Hudson tests.
#	Skip testing on 4.2 compilers only (debugging hudsons problem).
#	echo "Output disk usage as part of debugging..."
#	Note that df will always exit 1 (I don't know why), so call "pwd" so it will exit 0.
#	df /export
if BACKEND_COMPILER_VERSION_OK_FOR_ROSE_TESTING
	echo "Running test rule in ROSE/projects/compass/src.util/C-API for GNU 4.2.x compiler"
	@$(MAKE) test
else
#	echo "Calling test rule in ROSE/projects/compass/src.util/C-API (for all other non-4.2.x compilers)"
	echo "Running test rule in ROSE/projects/compass/src.util/C-API for GNU 4.2.x compiler"
	@$(MAKE) test
endif
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/compass/src.util/C-API: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"

#==========================================================================

EXTRA_DIST = c-string.api c-string.c

clean-local:
	rm -f *.ti a.out rose_*.c

# eof
