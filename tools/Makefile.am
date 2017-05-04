# Build the ROSE-based tools
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = 

# install the tools in 'bin'
bin_SCRIPTS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = moveDeclarationToInnermostScope rajaChecker

moveDeclarationToInnermostScope_SOURCES = moveDeclarationToInnermostScope.C
rajaChecker_SOURCES                       = rajaChecker.C

#----------testing part 
generatedCodeExamples = 
# DQ (4/12/2017): This needs more restrictions to be included in ROSE Matrix testing.
# Restricted to GNU compilers and version EDG 4.9 for only GNU version 4.8, and EDG 4.12 
# for later versions of GNU compilers. We can relax this constraints later if approriate.
# C++ 11 features need GCC 4.8 and later
# Liao 4/10/2017
if USING_GNU_COMPILER
if ROSE_USE_EDG_VERSION_4_9
# Restrict to GNU 4.8 for use with EDG 4.9
if !ROSE_USING_GCC_VERSION_LATER_4_9
if ROSE_USING_GCC_VERSION_LATER_4_8
generatedCodeExamples += \
  rose_inputrajaChecker.C
endif
endif
else
# Using EDG 4.12 and later.
if ROSE_USING_GCC_VERSION_LATER_4_9
generatedCodeExamples += \
  rose_inputrajaChecker.C
endif
endif
endif

# special options for raja checker 
rose_inputrajaChecker.C: $(top_srcdir)/tools/tests/inputrajaChecker.C rajaChecker
	./rajaChecker -rose:Cxx11_only -c $<

EXTRA_DIST += tests

check: $(generatedCodeExamples)
