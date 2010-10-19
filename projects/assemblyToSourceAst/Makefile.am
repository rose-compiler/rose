include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if !ROSE_MPI

SUBDIRS = tests

if ROSE_USE_ASSEMBLY_SEMANTICS

INCLUDES = $(ROSE_INCLUDES)

LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


bin_PROGRAMS = createDDG x86AssemblyToCDemo powerpcAssemblyToCDemo x86AssemblyToNetlist x86AssemblyToBtor simplifyCnf removeUnusedVariables unitResolve factorLiteralCoincidences doubleStep eliminateRedundantVariables readAndWriteSets binaryClauseSimplify lookForErrors btorUnroll btorCleanup
lib_LTLIBRARIES = libroseBtorSupport.la libroseSatProblem.la

BUILT_SOURCES = libroseBtorSupport.la libroseSatProblem.la

createDDG_SOURCES = createDDG.C generatedCOpts.C generatedCOpts.h x86AssemblyToC.h
createDDG_CPPFLAGS = $(ROSE_INCLUDES)
createDDG_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


x86AssemblyToCDemo_SOURCES = x86AssemblyToC.C generatedCOpts.C generatedCOpts.h x86AssemblyToC.h
x86AssemblyToCDemo_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToCDemo_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
powerpcAssemblyToCDemo_SOURCES = powerpcAssemblyToC.C generatedCOpts.C generatedCOpts.h powerpcInstructionSemantics.h
powerpcAssemblyToCDemo_CPPFLAGS = $(ROSE_INCLUDES)
powerpcAssemblyToCDemo_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
powerpcAssemblyToCDemo_DEPENDENCIES = powerpcAssemblyToC.C powerpcInstructionSemantics.h
x86AssemblyToNetlist_SOURCES = x86AssemblyToNetlist.C satProblem.h x86AssemblyToNetlist.h
x86AssemblyToNetlist_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToNetlist_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lroseSatProblem
x86AssemblyToBtor_SOURCES = x86AssemblyToBtorMain.C x86AssemblyToBtor.h btorProblem.h x86AssemblyToNetlist.h
x86AssemblyToBtor_CPPFLAGS = $(ROSE_INCLUDES)
x86AssemblyToBtor_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lroseBtorSupport
readAndWriteSets_SOURCES = readAndWriteSets.C
readAndWriteSets_CPPFLAGS = $(ROSE_INCLUDES)
readAndWriteSets_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

simplifyCnf_SOURCES = simplifyCnf.C satProblem.h
simplifyCnf_LDADD = -lroseSatProblem
removeUnusedVariables_SOURCES = removeUnusedVariables.C satProblem.h
removeUnusedVariables_LDADD = -lroseSatProblem
unitResolve_SOURCES = unitResolve.C satProblem.h
unitResolve_LDADD = -lroseSatProblem
factorLiteralCoincidences_SOURCES = factorLiteralCoincidences.C satProblem.h
factorLiteralCoincidences_LDADD = -lroseSatProblem
doubleStep_SOURCES = doubleStep.C satProblem.h
doubleStep_LDADD = -lroseSatProblem
eliminateRedundantVariables_SOURCES = eliminateRedundantVariables.C satProblem.h
eliminateRedundantVariables_LDADD = -lroseSatProblem
binaryClauseSimplify_SOURCES = binaryClauseSimplify.C satProblem.h
binaryClauseSimplify_LDADD = -lroseSatProblem
lookForErrors_SOURCES = lookForErrors.C satProblem.h x86AssemblyToNetlist.h
lookForErrors_LDADD = -lroseSatProblem $(ROSE_LIBS)
btorUnroll_SOURCES = btorUnroll.C
btorUnroll_LDADD = -lroseBtorSupport $(ROSE_LIBS)
btorCleanup_SOURCES = btorCleanup.C
btorCleanup_LDADD = -lroseBtorSupport $(ROSE_LIBS)
libroseBtorSupport_la_SOURCES = btorProblem.C x86AssemblyToBtor.C btorProblem.h
libroseSatProblem_la_SOURCES = satProblem.h satProblem.C

include_HEADERS = x86AssemblyToC.h satProblem.h powerpcInstructionSemantics.h x86AssemblyToNetlist.h x86AssemblyToBtor.h btorProblem.h

#powerpcAssemblyToCDemo-powerpcAssemblyToC.o: powerpcInstructionSemantics.h

test_assemblyToC:
	./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/binaries/samples/fnord.i386 

test_assemblyToNetlist:
	./x86AssemblyToNetlist $(top_srcdir)/binaries/samples/fnord.i386 

test_assemblyToBtor:
	./x86AssemblyToBtor start $(top_srcdir)/binaries/samples/fnord.i386 

test_assemblyToReadWriteSets:
	./readAndWriteSets $(top_srcdir)/binaries/samples/fnord.i386 > fnord.rws  

# DQ (3/15/2009): Changed this from check to check-local rule (as required by automake).
check-local: test_assemblyToC test_assemblyToNetlist test_assemblyToBtor test_assemblyToReadWriteSets

#check-local:
#	if test -e $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386; then \
#	  ./x86AssemblyToCDemo -c $(srcdir)/outputFileTemplate.C $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 && \
#	  ./x86AssemblyToNetlist $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 && \
#	  ./x86AssemblyToBtor start $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 && \
#	  ./readAndWriteSets $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/fnord.i386 > fnord.rws ; \
#	fi

#powerpcAssemblyToCDemo.o : powerpcInstructionSemantics.h
# powerpcAssemblyToC.C: powerpcInstructionSemantics.h

test-dep: powerpcAssemblyToCDemo powerpcInstructionSemantics.h

ppc-check:
	-rm powerpcAssemblyToCDemo-powerpcAssemblyToC.o
	$(MAKE) powerpcAssemblyToCDemo
	./powerpcAssemblyToCDemo -c $(srcdir)/powerpcOutputFileTemplate.C $(top_srcdir)/binaries/samples/fnord.ppc

CLEANFILES = outputFileTemplate.o rose_outputFileTemplate.C fnord.rws

endif

endif

EXTRA_DIST = outputFileTemplate.C


