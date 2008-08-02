include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if USE_BINARY_ANALYSIS_TESTS

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = x86AssemblyToCDemo

x86AssemblyToCDemo_SOURCES = x86AssemblyToCDemo.C x86AssemblyToC.C

include_HEADERS = x86AssemblyToC.h

check: x86AssemblyToCDemo
	if test -e $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; then \
	  ./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	fi

CLEANFILES = outputFileTemplate.o rose_outputFileTemplate.C

endif

EXTRA_DIST = outputFileTemplate.C x86AssemblyToCDemo.C x86AssemblyToC.C x86AssemblyToC.h

