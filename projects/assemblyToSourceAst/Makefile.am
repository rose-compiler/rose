include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = tests

if ROSE_USE_ASSEMBLY_SEMANTICS

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = x86AssemblyToCDemo powerpcAssemblyToCDemo x86AssemblyToNetlist x86AssemblyToBtor simplifyCnf removeUnusedVariables unitResolve factorLiteralCoincidences doubleStep eliminateRedundantVariables readAndWriteSets binaryClauseSimplify lookForErrors btorUnroll btorCleanup

# x86AssemblyToCDemo_SOURCES = x86AssemblyToCDemo.C x86AssemblyToC.C
x86AssemblyToCDemo_SOURCES = x86AssemblyToC.C x86InstructionSemantics.h semanticsModule.h
x86AssemblyToCDemo_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToCDemo_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
powerpcAssemblyToCDemo_SOURCES = powerpcAssemblyToC.C powerpcInstructionSemantics.h semanticsModule.h
powerpcAssemblyToCDemo_CPPFLAGS = $(ROSE_INCLUDES)
powerpcAssemblyToCDemo_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
powerpcAssemblyToCDemo_DEPENDENCIES = powerpcAssemblyToC.C powerpcInstructionSemantics.h semanticsModule.h
x86AssemblyToNetlist_SOURCES = x86AssemblyToNetlist.C satProblem.h x86InstructionSemantics.h x86AssemblyToNetlist.h semanticsModule.h
x86AssemblyToNetlist_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToNetlist_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
x86AssemblyToBtor_SOURCES = x86AssemblyToBtor.C x86AssemblyToBtor.h btorProblem.h x86InstructionSemantics.h x86AssemblyToNetlist.h btorProblem.C semanticsModule.h
x86AssemblyToBtor_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToBtor_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
readAndWriteSets_SOURCES = readAndWriteSets.C x86InstructionSemantics.h semanticsModule.h
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
btorCleanup_SOURCES = btorCleanup.C btorProblem.h btorProblem.C

include_HEADERS = x86AssemblyToC.h satProblem.h x86InstructionSemantics.h powerpcInstructionSemantics.h x86AssemblyToNetlist.h semanticsModule.h x86AssemblyToBtor.h

# powerpcAssemblyToCDemo-powerpcAssemblyToC.o: powerpcInstructionSemantics.h

check:
	if test -e $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; then \
	  ./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./x86AssemblyToNetlist $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./x86AssemblyToBtor $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; \
	  ./readAndWriteSets $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 > fnord.rws ; \
	fi

ppc-check:
	-rm powerpcAssemblyToCDemo-powerpcAssemblyToC.o
	$(MAKE) powerpcAssemblyToCDemo
	./powerpcAssemblyToCDemo $(srcdir)/powerpcOutputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.ppc

CLEANFILES = outputFileTemplate.o rose_outputFileTemplate.C fnord.rws

endif

EXTRA_DIST = outputFileTemplate.C

