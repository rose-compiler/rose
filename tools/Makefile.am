# Build the ROSE-based tools
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIR = globalVariablesInLambdas

EXTRA_DIST = 

# install the tools in 'bin'
bin_SCRIPTS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

EXTRA_DIST += tests

#-------------- only for C/C++
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

bin_PROGRAMS = moveDeclarationToInnermostScope rajaChecker

moveDeclarationToInnermostScope_SOURCES = moveDeclarationToInnermostScope.C
rajaChecker_SOURCES                       = rajaChecker.C

#----------testing part 
#  rose_inputrajaChecker.C 

Checker_Test_Files =
# C++ 11 features need GCC 4.9  and EDG 4.12and later
# Liao 4/10/2017
if USING_GNU_COMPILER
if ROSE_USE_EDG_VERSION_4_12
if ROSE_USING_GCC_VERSION_LATER_4_9
Checker_Test_Files += \
   nodalAccPattern1.cpp \
   nodalAccPattern2.cpp \
   constDouble.cpp \
   indirectArrayAccess.cpp \
   indirectLambdaCall.cpp \
   referenceType.cpp \
   indirecArrayInLambda.cpp
endif
endif
endif

generatedCodeExamples = $(addprefix rose_,${Checker_Test_Files})

# special options for raja checker 
rose_inputrajaChecker.C: $(top_srcdir)/tools/tests/inputrajaChecker.C rajaChecker
	./rajaChecker -rose:Cxx11_only -c $<

# regular for loop
#rose_nodalAccPattern1.cpp: $(top_srcdir)/tools/tests/nodalAccPattern1.cpp rajaChecker
#	./rajaChecker -c $<

$(generatedCodeExamples): $(top_srcdir)/tools/tests/$(@:rose_%=%) rajaChecker
	./rajaChecker --debug --report report.txt -std=c++11 -c $(top_srcdir)/tools/tests/$(@:rose_%=%) &>$(@:rose_%=%).screen.log

# enable diff based correctness checking
DIFF=diff
REFERENCE_PATH=$(top_srcdir)/tools/tests/references
# generate .cpp.output,  diff them with reference files, store the diffs into .cpp.diff
# the generation of generatedCodeExamples will generate .output file when --rose:checker:enable_debug is used. 
Checker_Output_Files=$(Checker_Test_Files:.cpp=.cpp.output)
Checker_Output_Diff_Files=$(Checker_Test_Files:.cpp=.cpp.diff)

$(Checker_Output_Diff_Files): $(generatedCodeExamples) 
	if $(DIFF) $(@:.cpp.diff=.cpp.output) $(REFERENCE_PATH)/$(@:.cpp.diff=.cpp.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi


check: $(generatedCodeExamples) $(Checker_Output_Diff_Files)

clean-local:
	rm -rf rose_*.C rose_*.cpp rose*.cxx *.diff *.output *.txt *.o *.log

endif	
