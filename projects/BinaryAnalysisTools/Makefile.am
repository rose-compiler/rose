include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
pkginclude_HEADERS =
EXTRA_DIST =
MOSTLYCLEANFILES =
TEST_TARGETS =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

########################################################################################################################
########################################################################################################################
########################################################################################################################
######                                                                                                            ######
######                                         _____           _                                                  ######
######                                        |_   _|__   ___ | |___                                              ######
######                                          | |/ _ \ / _ \| / __|                                             ######
######                                          | | (_) | (_) | \__ \                                             ######
######                                          |_|\___/ \___/|_|___/                                             ######
######                                                                                                            ######
######                                                                                                            ######
########################################################################################################################
########################################################################################################################
########################################################################################################################

#------------------------------------------------------------------------------------------------------------------------
# dump specimen memory to files

bin_PROGRAMS += dumpMemory
dumpMemory_SOURCES = dumpMemory.C
dumpMemory_CPPFLAGS = $(ROSE_INCLUDES)
dumpMemory_LDFLAGS = $(ROSE_RPATHS)
dumpMemory_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# linear disassembly at specified addresses

bin_PROGRAMS += linearDisassemble
linearDisassemble_SOURCES = linearDisassemble.C
linearDisassemble_CPPFLAGS = $(ROSE_INCLUDES)
linearDisassemble_LDFLAGS = $(ROSE_RPATHS)
linearDisassemble_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# disassembler using the new partitioner

bin_PROGRAMS += recursiveDisassemble
recursiveDisassemble_SOURCES = recursiveDisassemble.C
recursiveDisassemble_CPPFLAGS = $(ROSE_INCLUDES)
recursiveDisassemble_LDFLAGS = $(ROSE_RPATHS)
recursiveDisassemble_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Run specimen natively by single stepping

bin_PROGRAMS += nativeExecutionTrace
nativeExecutionTrace_SOURCES = nativeExecutionTrace.C
nativeExecutionTrace_CPPFLAGS = $(ROSE_INCLUDES)
nativeExecutionTrace_LDFLAGS = $(ROSE_RPATHS)
nativeExecutionTrace_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Single-step a process and compare execution addresses with a list of addresses

bin_PROGRAMS += checkExecutionAddresses
checkExecutionAddresses_SOURCES = checkExecutionAddresses.C
checkExecutionAddresses_CPPFLAGS = $(ROSE_INCLUDES)
checkExecutionAddresses_LDFLAGS = $(ROSE_RPATHS)
checkExecutionAddresses_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Scans memory for magic numbers

bin_PROGRAMS += magicScanner
magicScanner_SOURCES = magicScanner.C
magicScanner_CPPFLAGS = $(ROSE_INCLUDES)
magicScanner_LDFLAGS = $(ROSE_RPATHS)
magicScanner_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Execute instructions

bin_PROGRAMS += simulate
simulate_SOURCES = simulate.C
simulate_CPPFLAGS = $(ROSE_INCLUDES)
simulate_LDFLAGS = $(ROSE_RPATHS)
simulate_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Decode encoded strings

bin_PROGRAMS += stringDecoder
stringDecoder_SOURCES = stringDecoder.C
stringDecoder_CPPFLAGS = $(ROSE_INCLUDES)
stringDecoder_LDFLAGS = $(ROSE_RPATHS)
stringDecoder_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Find code that cannot be reached according to static analysis

bin_PROGRAMS += findDeadCode
findDeadCode_SOURCES = findDeadCode.C
findDeadCode_CPPFLAGS = $(ROSE_INCLUDES)
findDeadCode_LDFLAGS = $(ROSE_RPATHS)
findDeadCode_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Parse and simplify symbolic expressions

bin_PROGRAMS += symbolicSimplifier
symbolicSimplifier_SOURCES = symbolicSimplifier.C
symbolicSimplifier_CPPFLAGS = $(ROSE_INCLUDES)
symbolicSimplifier_LDFLAGS = $(ROSE_RPATHS)
symbolicSimplifier_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Generate source code from a binary specimen

bin_PROGRAMS += binaryToSource
binaryToSource_SOURCES = binaryToSource.C
binaryToSource_CPPFLAGS = $(ROSE_INCLUDES)
binaryToSource_LDFLAGS = $(ROSE_RPATHS)
binaryToSource_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Generate a program trace and show some properties of it

bin_PROGRAMS += trace
trace_SOURCES = trace.C
trace_CPPFLAGS = $(ROSE_INCLUDES)
trace_LDFLAGS = $(ROSE_RPATHS)
trace_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Not distributed; not part of the Git repo

detectConstants: detectConstants.C
	$(top_builddir)/libtool --mode=compile	\
	    $(CXX) -c				\
	    -DUSE_PARTITIONER2			\
	    $(CFLAGS)				\
	    $(CPPFLAGS)				\
	    -I$(top_builddir)			\
	    $(ROSE_INCLUDES)			\
	    -o detectConstants.o $^
	$(top_builddir)/libtool --mode=link	\
	    $(CXX)				\
	    $(CFLAGS)				\
	    $(LIBS_WITH_RPATH)			\
	    $(ROSE_LIBS)			\
	    -o $@ detectConstants.lo

MOSTLYCLEANFILES += detectConstants detectConstants.o

#------------------------------------------------------------------------------------------------------------------------
# Experimental "binary ROSE on-line workbench for specimen exploration"

if ROSE_USE_WT

bin_PROGRAMS += bROwSE-server
bROwSE_server_SOURCES =				\
	bROwSE/Application.C			\
	bROwSE/bROwSE.C				\
	bROwSE/FunctionListModel.C		\
	bROwSE/FunctionUtil.C			\
	bROwSE/InstructionListModel.C		\
	bROwSE/Statistics.C			\
	bROwSE/WAddressComboBox.C		\
	bROwSE/WAddressSpace.C			\
	bROwSE/WAssemblyListing.C		\
	bROwSE/WBusy.C				\
	bROwSE/WCrossReferences.C		\
	bROwSE/WFunctionCfg.C			\
	bROwSE/WFunctionList.C			\
	bROwSE/WFunctionNavigation.C		\
	bROwSE/WFunctionSummary.C		\
	bROwSE/WHexDump.C			\
	bROwSE/WHexValueEdit.C			\
	bROwSE/WInstructionList.C		\
	bROwSE/WMagic.C				\
	bROwSE/WMemoryMap.C			\
	bROwSE/WPartitioner.C			\
	bROwSE/WSemantics.C			\
	bROwSE/WSplash.C			\
	bROwSE/WStatus.C			\
	bROwSE/WStringDetail.C			\
	bROwSE/WStrings.C			\
	bROwSE/WToggleButton.C
bROwSE_server_CPPFLAGS = -I$(ROSE_INCLUDES) -I$(top_builddir)/src/roseSupport -I$(WT_PREFIX)/include
bROwSE_server_LDFLAGS = $(ROSE_RPATHS)
bROwSE_server_LDADD = -L$(WT_PREFIX)/lib -lwt -lwthttp -lboost_signals $(ROSE_LIBS)

pkginclude_HEADERS += 				\
        bROwSE/Application.h			\
        bROwSE/FunctionListModel.h		\
        bROwSE/FunctionUtil.h			\
        bROwSE/InstructionListModel.h		\
        bROwSE/Statistics.h			\
        bROwSE/WAddressComboBox.h		\
        bROwSE/WAddressSpace.h			\
        bROwSE/WAssemblyListing.h		\
        bROwSE/WBasicBlock.C			\
        bROwSE/WBusy.h				\
	bROwSE/WCrossReferences.h		\
        bROwSE/WFunctionCfg.h			\
        bROwSE/WFunctionList.h			\
        bROwSE/WFunctionNavigation.h		\
        bROwSE/WFunctionSummary.h		\
        bROwSE/WHexDump.h			\
        bROwSE/WHexValueEdit.h			\
        bROwSE/WInstructionList.h		\
	bROwSE/WMagic.h				\
        bROwSE/WMemoryMap.h			\
        bROwSE/WPartitioner.h			\
        bROwSE/WSemantics.h			\
        bROwSE/WSplash.h			\
        bROwSE/WStatus.h			\
	bROwSE/WStrings.h			\
	bROwSE/WStringDetail.h			\
        bROwSE/WToggleButton.h			\
        bROwSE/bROwSE.h

endif


#------------------------------------------------------------------------------------------------------------------------
# Generate programs having specific structures (for path testing)

bin_PROGRAMS += generatePaths
generatePaths_SOURCES = generatePaths.C
generatePaths_CPPFLAGS = $(ROSE_INCLUDES)
generatePaths_LDFLAGS = $(ROSE_RPATHS)
generatePaths_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Find paths in the CFG

bin_PROGRAMS += findPath
findPath_SOURCES =				\
	PathFinder/PathFinder.C			\
	PathFinder/semantics.C			\
	PathFinder/findPath.C
findPath_CPPFLAGS = $(ROSE_INCLUDES)
findPath_LDFLAGS = $(ROSE_RPATHS)
findPath_LDADD = $(ROSE_LIBS)

pkginclude_HEADERS +=				\
	PathFinder/PathFinder.h			\
	PathFinder/semantics.h

#------------------------------------------------------------------------------------------------------------------------
# find similar functions in two specimens

if ROSE_HAVE_DLIB

bin_PROGRAMS += findSimilarFunctions
findSimilarFunctions_SOURCES = findSimilarFunctions.C
findSimilarFunctions_CPPFLAGS = $(ROSE_INCLUDES)
findSimilarFunctions_LDFLAGS = $(ROSE_RPATHS)
findSimilarFunctions_LDADD = $(ROSE_LIBS)

endif

#------------------------------------------------------------------------------------------------------------------------
# maximize bijection of two sets

bin_PROGRAMS += maxBijection
maxBijection_SOURCES = maxBijection.C
maxBijection_CPPFLAGS = $(ROSE_INCLUDES)
maxBijection_LDFLAGS = $(ROSE_RPATHS)
maxBijection_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# debug instruction semantics

bin_PROGRAMS += debugSemantics
debugSemantics_SOURCES = debugSemantics.C
debugSemantics_CPPFLAGS = $(ROSE_INCLUDES)
debugSemantics_LDFLAGS = $(ROSE_RPATHS)
debugSemantics_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-call-targets

bin_PROGRAMS += x86-call-targets
x86_call_targets_SOURCES = x86-call-targets.C
x86_call_targets_CPPFLAGS = $(ROSE_INCLUDES)
x86_call_targets_LDFLAGS = $(ROSE_RPATHS)
x86_call_targets_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# generate XML from ROSE internals

bin_PROGRAMS += rxml
rxml_SOURCES = rxml.C
rxml_CPPFLAGS = $(ROSE_INCLUDES)
rxml_LDFLAGS = $(ROSE_RPATHS)
rxml_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# DWARF line numbers

bin_PROGRAMS += dwarf-lines
dwarf_lines_SOURCES = dwarf-lines.C
dwarf_lines_CPPFLAGS = $(ROSE_INCLUDES)
dwarf_lines_LDFLAGS = $(ROSE_RPATHS)
dwarf_lines_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Very fast XML to JSON converter

bin_PROGRAMS += xml2json
xml2json_SOURCES = xml2json.C
xml2json_CPPFLAGS = $(ROSE_INCLUDES)
xml2json_LDFLAGS = $(ROSE_RPATHS)
xml2json_LDADD = $(ROSE_LIBS)


########################################################################################################################
########################################################################################################################
########################################################################################################################
######                                                                                                            ######
######                                         _____         _                                                    ######
######                                        |_   _|__  ___| |_ ___                                              ######
######                                          | |/ _ \/ __| __/ __|                                             ######
######                                          | |  __/\__ \ |_\__ \                                             ######
######                                          |_|\___||___/\__|___/                                             ######
######                                                                                                            ######
######                                                                                                            ######
########################################################################################################################
########################################################################################################################
########################################################################################################################

# Uses the debugSemantics and a carefully crafted input specimen to test features of RegisterStateGeneric.

TEST_TARGETS += testAdaptiveRegisters.passed
testAdaptiveRegisters.passed: $(srcdir)/specimens/x86-64-adaptiveRegs		\
			      $(srcdir)/TestAnswers/testAdaptiveRegisters.ans		\
			      debugSemantics
	@$(RTH_RUN)												\
		TITLE="RegisterStateGeneric [$@]"								\
		CMD="./debugSemantics --test-adaptive-registers --semantics=symbolic --track-writers=last $<"	\
		ANS=$(srcdir)/TestAnswers/testAdaptiveRegisters.ans						\
		$(top_srcdir)/scripts/test_with_answer $@



endif

#------------------------------------------------------------------------------------------------------------------------
# Makefile boilerplate

clean-local:
	rm -f $(MOSTLYCLEANFILES)

check-local: $(TEST_TARGETS)

