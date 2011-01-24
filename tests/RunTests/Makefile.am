include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = @optional_AxxPxxSpecificTest_subdirs@
# we can't make this @optional_AxxPxxSpecificTest_subdirs@
# since the substitution could be NULL which causes a problem
# for automake generated shell code placed in Makefile.

# Comment out P++ tests until they compile properly
# SUBDIRS = A++Tests P++Tests
SUBDIRS = . A++Tests AstDeleteTests

INCLUDES =  \
        $(ROSE_INCLUDES) 
#	        -I$(top_srcdir)/src/frontend/SageIII/sageInterface
LDADD =  $(ROSE_LIBS)

noinst_PROGRAMS = \
  traverseCommonBlock

traverseCommonBlock_SOURCES = traverseCommonBlock.C  

generatedCodeExamples = 

# turn off edg warnings such as "variable x declared but never referenced" for test input files
TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
generatedCodeExamples += \
	  rose_inputtraverseCommonBlock.f

rose_inputtraverseCommonBlock.f:traverseCommonBlock
	./traverseCommonBlock -c $(TEST_CXXFLAGS) $(srcdir)/inputtraverseCommonBlock.f
endif	  

EXTRA_DIST = CMakeLists.txt inputtraverseCommonBlock.f
check-local:
	$(MAKE) $(generatedCodeExamples)
clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C rose_*.f rose_*.cpp *.C.pdf *.c.pdf testfile525.cpp testfile626.cpp rose_*.upc abiStuffTestDone *.dot rose_*
