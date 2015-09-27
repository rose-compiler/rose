#==========================================================================

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = -I$(srcdir)/.. -I$(srcdir)/../../compassSupport $(ROSE_INCLUDES)

#==========================================================================
# Auxiliary library.

if ROSE_MPI
#LTCXXCOMPILE = \
#	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
#        $(DEFAULT_AM_CPPFLAGS) $(AM_CPPFLAGS) $(AM_CPPFLAGS) $(CPPFLAGS) \
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

# CH (2/22/2010): put $(ROSE_LIBS_WITH_PATH) ahead to avoid the linker to find boost libraries in usr/lib first
# this problem still need to be solved in the future
LDADD = $(ROSE_LIBS_WITH_PATH) ../../compassSupport/libCompass.la libC-API.la
#LDADD = ../../compassSupport/libCompass.la libC-API.la $(ROSE_LIBS_WITH_PATH)
#LDADD = ../../compassSupport/libCompass.la libC-API.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testC_API_SOURCES = testC-API.C

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind

# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND  = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

test: libC-API.la testC-API$(EXEEXT) $(srcdir)/c-string.api $(srcdir)/c-string.c
	$(VALGRIND) ./testC-API$(EXEEXT) --api $(srcdir)/c-string.api -rose:C_only -c $(srcdir)/c-string.c

check-local:
	echo "Running test rule in ROSE/projects/compass/src/util/C-API..."
	@$(MAKE) test
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/compass/src.util/C-API: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"

#==========================================================================

EXTRA_DIST = c-string.api c-string.c

clean-local:
	rm -f *.ti a.out rose_*.c

# eof
