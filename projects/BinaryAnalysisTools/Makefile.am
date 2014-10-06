include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
EXTRA_DIST =

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
# Experimental "binary ROSE on-line workbench for specimen exploration"

if ROSE_USE_WT

bin_PROGRAMS += bROwSE
bROwSE_SOURCES = bROwSE.C
bROwSE_CPPFLAGS = $(ROSE_INCLUDES) -I$(WT_PREFIX)/include
bROwSE_LDADD = -L$(WT_PREFIX)/lib -lwt -lwthttp $(LIBS_WITH_RPATH) $(ROSE_LIBS)

endif

#------------------------------------------------------------------------------------------------------------------------
# S-Record utilities

bin_PROGRAMS += dumpSRecords
dumpSRecords_SOURCES = dumpSRecords.C
dumpSRecords_CPPFLAGS = $(ROSE_INCLUDES)
dumpSRecords_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS += fileToSRecords
fileToSRecords_SOURCES = fileToSRecords.C
fileToSRecords_CPPFLAGS = $(ROSE_INCLUDES)
fileToSRecords_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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

