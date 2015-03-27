include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
EXTRA_DIST =

#------------------------------------------------------------------------------------------------------------------------
# dump specimen memory to files

bin_PROGRAMS += dumpMemory
dumpMemory_SOURCES = dumpMemory.C
dumpMemory_CPPFLAGS = $(ROSE_INCLUDES)
dumpMemory_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# linear disassembly at specified addresses

bin_PROGRAMS += linearDisassemble
linearDisassemble_SOURCES = linearDisassemble.C
linearDisassemble_CPPFLAGS = $(ROSE_INCLUDES)
linearDisassemble_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# disassembler using the new partitioner

bin_PROGRAMS += recursiveDisassemble
recursiveDisassemble_SOURCES = recursiveDisassemble.C
recursiveDisassemble_CPPFLAGS = $(ROSE_INCLUDES)
recursiveDisassemble_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Single-step a process and compare execution addresses with a list of addresses

bin_PROGRAMS += checkExecutionAddresses
checkExecutionAddresses_SOURCES = checkExecutionAddresses.C
checkExecutionAddresses_CPPFLAGS = $(ROSE_INCLUDES)
checkExecutionAddresses_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Scans memory for magic numbers

bin_PROGRAMS += magicScanner
magicScanner_SOURCES = magicScanner.C
magicScanner_CPPFLAGS = $(ROSE_INCLUDES)
magicScanner_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Execute instructions

bin_PROGRAMS += simulate
simulate_SOURCES = simulate.C
simulate_CPPFLAGS = $(ROSE_INCLUDES)
simulate_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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
bROwSE_server_LDADD = -L$(WT_PREFIX)/lib -lwt -lwthttp $(LIBS_WITH_RPATH) $(ROSE_LIBS)

pkginclude_HEADERS = 				\
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
# Find paths in the CFG

bin_PROGRAMS += findPath
findPath_SOURCES = findPath.C
findPath_CPPFLAGS = $(ROSE_INCLUDES)
findPath_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# find similar functions in two specimens

if ROSE_USE_DLIB

bin_PROGRAMS += findSimilarFunctions
findSimilarFunctions_SOURCES = findSimilarFunctions.C
findSimilarFunctions_CPPFLAGS = $(ROSE_INCLUDES) -I$(DLIB_PREFIX)
findSimilarFunctions_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

endif

#------------------------------------------------------------------------------------------------------------------------
# maximize bijection of two sets

bin_PROGRAMS += maxBijection
maxBijection_SOURCES = maxBijection.C
maxBijection_CPPFLAGS = $(ROSE_INCLUDES)
maxBijection_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# debug instruction semantics

bin_PROGRAMS += debugSemantics
debugSemantics_SOURCES = debugSemantics.C
debugSemantics_CPPFLAGS = $(ROSE_INCLUDES)
debugSemantics_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-call-targets

bin_PROGRAMS += x86-call-targets
x86_call_targets_SOURCES = x86-call-targets.C
x86_call_targets_CPPFLAGS = $(ROSE_INCLUDES)
x86_call_targets_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# x86-function-vas

bin_PROGRAMS += x86-function-vas
x86_function_vas_SOURCES = x86-function-vas.C
x86_function_vas_CPPFLAGS = $(ROSE_INCLUDES)
x86_function_vas_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

