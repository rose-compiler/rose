include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = tests

if ROSE_USE_ASSEMBLY_SEMANTICS

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = x86AssemblyToCDemo x86AssemblyToNetlist x86AssemblyToBtor simplifyCnf removeUnusedVariables unitResolve factorLiteralCoincidences doubleStep eliminateRedundantVariables readAndWriteSets binaryClauseSimplify lookForErrors btorUnroll

# x86AssemblyToCDemo_SOURCES = x86AssemblyToCDemo.C x86AssemblyToC.C
x86AssemblyToCDemo_SOURCES = x86AssemblyToC.C x86InstructionSemantics.h
x86AssemblyToCDemo_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToCDemo_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
x86AssemblyToNetlist_SOURCES = x86AssemblyToNetlist.C satProblem.h x86InstructionSemantics.h x86AssemblyToNetlist.h
x86AssemblyToNetlist_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToNetlist_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
x86AssemblyToBtor_SOURCES = x86AssemblyToBtor.C btorProblem.h x86InstructionSemantics.h x86AssemblyToNetlist.h btorProblem.C
x86AssemblyToBtor_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToBtor_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
readAndWriteSets_SOURCES = readAndWriteSets.C x86InstructionSemantics.h
readAndWriteSets_CPPFLAGS = $(ROSE_INCLUDES)
readAndWriteSets_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
simplifyCnf_SOURCES = simplifyCnf.C satProblem.h
removeUnusedVariables_SOURCES = removeUnusedVariables.C satProblem.h
unitResolve_SOURCES = unitResolve.C satProblem.h
factorLiteralCoincidences_SOURCES = factorLiteralCoincidences.C satProblem.h
doubleStep_SOURCES = doubleStep.C satProblem.h
eliminateRedundantVariables_SOURCES = eliminateRedundantVariables.C satProblem.h
binaryClauseSimplify_SOURCES = binaryClauseSimplify.C satProblem.h
lookForErrors_SOURCES = lookForErrors.C satProblem.h x86AssemblyToNetlist.h
btorUnroll_SOURCES = btorUnroll.C btorProblem.h btorProblem.C

include_HEADERS = x86AssemblyToC.h satProblem.h x86InstructionSemantics.h x86AssemblyToNetlist.h

check:
	if test -e $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; then \
	  ./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./x86AssemblyToNetlist $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./x86AssemblyToBtor $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./readAndWriteSets $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 > fnord.rws ; \
	fi

CLEANFILES = outputFileTemplate.o rose_outputFileTemplate.C fnord.rws

endif

EXTRA_DIST = outputFileTemplate.C

