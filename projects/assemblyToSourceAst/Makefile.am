include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if USE_BINARY_ANALYSIS_TESTS

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = x86AssemblyToCDemo x86AssemblyToNetlist simplifyCnf

x86AssemblyToCDemo_SOURCES = x86AssemblyToCDemo.C x86AssemblyToC.C
x86AssemblyToNetlist_SOURCES = x86AssemblyToNetlist.C satProblem.h
simplifyCnf_SOURCES = simplifyCnf.C
simplifyCnf_CPPFLAGS =
simplifyCnf_LDADD =

include_HEADERS = x86AssemblyToC.h satProblem.h

jeremiah-check: x86AssemblyToCDemo x86AssemblyToNetlist
	if test -e $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; then \
	  ./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./x86AssemblyToNetlist $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	fi

check:

CLEANFILES = outputFileTemplate.o rose_outputFileTemplate.C

endif

EXTRA_DIST = outputFileTemplate.C x86AssemblyToCDemo.C x86AssemblyToC.C x86AssemblyToC.h x86AssemblyToNetlist.C

