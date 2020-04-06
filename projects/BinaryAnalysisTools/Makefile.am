include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
bin_SCRIPTS =
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

bin_PROGRAMS += rose-dump-memory
rose_dump_memory_SOURCES = dumpMemory.C
rose_dump_memory_CPPFLAGS = $(ROSE_INCLUDES)
rose_dump_memory_LDFLAGS = $(ROSE_RPATHS)
rose_dump_memory_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# linear disassembly at specified addresses

bin_PROGRAMS += rose-linear-disassemble
rose_linear_disassemble_SOURCES = linearDisassemble.C
rose_linear_disassemble_CPPFLAGS = $(ROSE_INCLUDES)
rose_linear_disassemble_LDFLAGS = $(ROSE_RPATHS)
rose_linear_disassemble_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# disassembler using the new partitioner

bin_PROGRAMS += rose-recursive-disassemble
rose_recursive_disassemble_SOURCES = recursiveDisassemble.C
rose_recursive_disassemble_CPPFLAGS = $(ROSE_INCLUDES)
rose_recursive_disassemble_LDFLAGS = $(ROSE_RPATHS)
rose_recursive_disassemble_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Run specimen natively by single stepping

bin_PROGRAMS += rose-native-execution-trace
rose_native_execution_trace_SOURCES = nativeExecutionTrace.C
rose_native_execution_trace_CPPFLAGS = $(ROSE_INCLUDES)
rose_native_execution_trace_LDFLAGS = $(ROSE_RPATHS)
rose_native_execution_trace_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Single-step a process and compare execution addresses with a list of addresses

bin_PROGRAMS += rose-check-execution-addresses
rose_check_execution_addresses_SOURCES = checkExecutionAddresses.C
rose_check_execution_addresses_CPPFLAGS = $(ROSE_INCLUDES)
rose_check_execution_addresses_LDFLAGS = $(ROSE_RPATHS)
rose_check_execution_addresses_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Scans memory for magic numbers

bin_PROGRAMS += rose-magic-scanner
rose_magic_scanner_SOURCES = magicScanner.C
rose_magic_scanner_CPPFLAGS = $(ROSE_INCLUDES)
rose_magic_scanner_LDFLAGS = $(ROSE_RPATHS)
rose_magic_scanner_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Execute instructions

bin_PROGRAMS += rose-simulate
rose_simulate_SOURCES = simulate.C
rose_simulate_CPPFLAGS = $(ROSE_INCLUDES)
rose_simulate_LDFLAGS = $(ROSE_RPATHS)
rose_simulate_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Decode encoded strings

bin_PROGRAMS += rose-string-decoder
rose_string_decoder_SOURCES = stringDecoder.C
rose_string_decoder_CPPFLAGS = $(ROSE_INCLUDES)
rose_string_decoder_LDFLAGS = $(ROSE_RPATHS)
rose_string_decoder_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Parse and simplify symbolic expressions

bin_PROGRAMS += rose-symbolic-simplifier
rose_symbolic_simplifier_SOURCES = symbolicSimplifier.C
rose_symbolic_simplifier_CPPFLAGS = $(ROSE_INCLUDES)
rose_symbolic_simplifier_LDFLAGS = $(ROSE_RPATHS)
rose_symbolic_simplifier_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Generate source code from a binary specimen

bin_PROGRAMS += rose-binary-to-source
rose_binary_to_source_SOURCES = binaryToSource.C
rose_binary_to_source_CPPFLAGS = $(ROSE_INCLUDES)
rose_binary_to_source_LDFLAGS = $(ROSE_RPATHS)
rose_binary_to_source_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Generate a program trace and show some properties of it

bin_PROGRAMS += rose-trace
rose_trace_SOURCES = trace.C
rose_trace_CPPFLAGS = $(ROSE_INCLUDES)
rose_trace_LDFLAGS = $(ROSE_RPATHS)
rose_trace_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Monitor execution and estimate number of unique instructions

#~ bin_PROGRAMS += rose-execution-monitor

#~ rose_execution_monitor_SOURCES = ExecutionMonitor.C
#~ rose_execution_monitor_CPPFLAGS = $(ROSE_INCLUDES)
#~ rose_execution_monitor_LDFLAGS = $(ROSE_RPATHS)
#~ rose_execution_monitor_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
#~ bin_PROGRAMS += rose-perf-execution-wrapper
#~ rose_perf_execution_wrapper_SOURCES = rose-perf-execution-wrapper.c

#------------------------------------------------------------------------------------------------------------------------
# Analyze perf output for estimating the number of unique instructions

#~ AM_YFLAGS = -d -t

#~ bin_PROGRAMS += rose-perf-analyzer
#~ rose_perf_analyzer_SOURCES = PerfOutputAnalyzer/parser.ypp PerfOutputAnalyzer/lexer.lpp \
#~                              PerfOutputAnalyzer/analyzer.cpp 
#~ rose_perf_analyzer_CPPFLAGS = $(ROSE_INCLUDES)
#~ rose_perf_analyzer_LDFLAGS = $(ROSE_RPATHS)
#~ rose_perf_analyzer_LDADD = $(ROSE_LIBS)


#------------------------------------------------------------------------------------------------------------------------
# Test for rose-execution-monitor
#   specimen can be invoked: testExecutionMonitor [mode] [N]
#                            mode = 0 .. invoke identity N times.
#                            mode = 1 .. alternate identity, sum, fibonacci over N runs.
#                            default mode = 0, default N = 10

#~ bin_PROGRAMS += testExecutionMonitor

#~ testExecutionMonitor_SOURCES = specimens/test-execution-monitor.C

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

bin_PROGRAMS += rose-generate-paths
rose_generate_paths_SOURCES = generatePaths.C
rose_generate_paths_CPPFLAGS = $(ROSE_INCLUDES)
rose_generate_paths_LDFLAGS = $(ROSE_RPATHS)
rose_generate_paths_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Find paths in the CFG

bin_PROGRAMS += rose-find-path
rose_find_path_SOURCES =			\
	PathFinder/PathFinder.C			\
	PathFinder/semantics.C			\
	PathFinder/findPath.C
rose_find_path_CPPFLAGS = $(ROSE_INCLUDES)
rose_find_path_LDFLAGS = $(ROSE_RPATHS)
rose_find_path_LDADD = $(ROSE_LIBS)

pkginclude_HEADERS +=				\
	PathFinder/PathFinder.h			\
	PathFinder/semantics.h

#------------------------------------------------------------------------------------------------------------------------
# find similar functions in two specimens

if ROSE_HAVE_DLIB

bin_PROGRAMS += rose-find-similar-functions
rose_find_similar_functions_SOURCES = findSimilarFunctions.C
rose_find_similar_functions_CPPFLAGS = $(ROSE_INCLUDES)
rose_find_similar_functions_LDFLAGS = $(ROSE_RPATHS)
rose_find_similar_functions_LDADD = $(ROSE_LIBS)

endif

#------------------------------------------------------------------------------------------------------------------------
# maximize bijection of two sets

bin_PROGRAMS += rose-max-bijection
rose_max_bijection_SOURCES = maxBijection.C
rose_max_bijection_CPPFLAGS = $(ROSE_INCLUDES)
rose_max_bijection_LDFLAGS = $(ROSE_RPATHS)
rose_max_bijection_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# debug instruction semantics

bin_PROGRAMS += rose-debug-semantics
rose_debug_semantics_SOURCES = debugSemantics.C
rose_debug_semantics_CPPFLAGS = $(ROSE_INCLUDES)
rose_debug_semantics_LDFLAGS = $(ROSE_RPATHS)
rose_debug_semantics_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-call-targets

bin_PROGRAMS += rose-x86-call-targets
rose_x86_call_targets_SOURCES = x86-call-targets.C
rose_x86_call_targets_CPPFLAGS = $(ROSE_INCLUDES)
rose_x86_call_targets_LDFLAGS = $(ROSE_RPATHS)
rose_x86_call_targets_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# generate XML from ROSE internals

bin_PROGRAMS += rose-rxml
rose_rxml_SOURCES = rxml.C
rose_rxml_CPPFLAGS = $(ROSE_INCLUDES)
rose_rxml_LDFLAGS = $(ROSE_RPATHS)
rose_rxml_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# DWARF line numbers

bin_PROGRAMS += rose-dwarf-lines
rose_dwarf_lines_SOURCES = dwarf-lines.C
rose_dwarf_lines_CPPFLAGS = $(ROSE_INCLUDES)
rose_dwarf_lines_LDFLAGS = $(ROSE_RPATHS)
rose_dwarf_lines_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Very fast XML to JSON converter

bin_PROGRAMS += rose-xml2json
rose_xml2json_SOURCES = xml2json.C
rose_xml2json_CPPFLAGS = $(ROSE_INCLUDES)
rose_xml2json_LDFLAGS = $(ROSE_RPATHS)
rose_xml2json_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Script to print historgram of instructions with missing semantics

bin_SCRIPTS += rose-missing-semantics

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
			      rose-debug-semantics
	@$(RTH_RUN)													\
		TITLE="RegisterStateGeneric [$@]"									\
		CMD="./rose-debug-semantics --test-adaptive-registers --semantics=symbolic --track-writers=last $<"	\
		ANS=$(srcdir)/TestAnswers/testAdaptiveRegisters.ans							\
		$(top_srcdir)/scripts/test_with_answer $@

# Tests that the xml2json tool works correctly

xml2json_inputs =				\
	xml2json-test01.xml			\
	xml2json-test02.xml			\
	xml2json-test03.xml			\
	xml2json-test04.xml			\
	xml2json-test05.xml			\
	xml2json-test06.xml			\
	xml2json-test07.xml			\
	xml2json-test08.xml			\
	xml2json-test09.xml			\
	xml2json-test10.xml			\
	xml2json-test11.xml			\
	xml2json-test12.xml

xml2json_tests = $(addsuffix .passed, $(xml2json_inputs))
TEST_TARGETS += $(xml2json_tests)
check-xml2json: $(xml2json_tests)

$(xml2json_tests): %.passed: $(abspath $(srcdir))/specimens/% rose-xml2json
	@$(RTH_RUN)										\
		TITLE="convert $(notdir $<) to JSON [$@]"					\
		USE_SUBDIR=yes									\
		CMD="$$(pwd)/rose-xml2json $< $(notdir $<).json && cat $(notdir $<).json"	\
		ANS="$(srcdir)/TestAnswers/$*.ans"						\
		FILTER="$(srcdir)/semanticsFilter"						\
		$(top_srcdir)/scripts/test_with_answer $@

endif

#------------------------------------------------------------------------------------------------------------------------
# Makefile boilerplate

clean-local:
	rm -f $(MOSTLYCLEANFILES)

check-local: $(TEST_TARGETS)

