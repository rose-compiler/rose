# Build the ROSE-based tools
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = 

# install the tools in 'bin'
bin_SCRIPTS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
AM_LDFLAGS = $(ROSE_LIBS)

EXTRA_DIST += tests

#-------------- only for C/C++
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

bin_PROGRAMS = moveDeclarationToInnermostScope rajaChecker

moveDeclarationToInnermostScope_SOURCES = moveDeclarationToInnermostScope.C
rajaChecker_SOURCES                       = rajaChecker.C

#----------testing part 
#  rose_inputrajaChecker.C 

generatedCodeExamples = 
# C++ 11 features need GCC 4.9  and EDG 4.12and later
# Liao 4/10/2017
if USING_GNU_COMPILER
if ROSE_USE_EDG_VERSION_4_12
if ROSE_USING_GCC_VERSION_LATER_4_9
generatedCodeExamples += \
  rose_nodalAccPattern1.cpp \
  rose_nodalAccPattern2.cpp
endif
endif
endif

# special options for raja checker 
rose_inputrajaChecker.C: $(top_srcdir)/tools/tests/inputrajaChecker.C rajaChecker
	./rajaChecker -rose:Cxx11_only -c $<

# regular for loop
#rose_nodalAccPattern1.cpp: $(top_srcdir)/tools/tests/nodalAccPattern1.cpp rajaChecker
#	./rajaChecker -c $<

$(generatedCodeExamples): $(top_srcdir)/tools/tests/$(@:rose_%=%) rajaChecker
	./rajaChecker -std=c++11 -c $(top_srcdir)/tools/tests/$(@:rose_%=%)

check: $(generatedCodeExamples)

clean:
	rm -rf rose_*.C rose_*.cpp rose*.cxx

endif	
