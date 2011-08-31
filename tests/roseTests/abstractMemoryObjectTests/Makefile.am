include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS= .
#if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
#SUBDIRS += fortran
#endif

# Default header file search path
INCLUDES = $(ROSE_INCLUDES)
memObjectGenerator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Put test codes without main() entry here
C_TESTCODES_REQUIRED_TO_COMPILE =  \
     struct_simplest.c

CXX_TESTCODES_REQUIRED_TO_COMPILE = \
     test_2011_08_15.cpp \
     test_2011_08_26.cpp \
     pointer.cpp \
     labeledAggregate.cpp

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
	$(C_TESTCODES_REQUIRED_TO_COMPILE) \
	$(CXX_TESTCODES_REQUIRED_TO_COMPILE) \
	$(TESTCODE_CURRENTLY_FAILING)

#TEST_DIR = $(top_srcdir)/tests/CompileTests/OpenMP_tests
#TEST_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII -I$(top_srcdir)/src/midend/programTransformation/ompLowering

noinst_PROGRAMS = memObjectGenerator
memObjectGenerator_SOURCES = memObjectGenerator.C

# this flag is used for the long list of CXX test files
TEST_FLAGS = $(TEST_INCLUDES) -g --edg:no_warnings #-rose:verbose 2 #--edg:restrict #-rose:wave 

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_COMPILE:.c=.o)
PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_COMPILE:.cpp=.o)

# rules to build .o from .c for OpenMP test codes
$(PASSING_C_TEST_Objects): %.o: %.c memObjectGenerator
	./memObjectGenerator$(EXEEXT) ${TEST_FLAGS} -c $< &>$(@:.o=.out)


$(PASSING_CXX_TEST_Objects): %.o: %.cpp memObjectGenerator
	./memObjectGenerator$(EXEEXT) ${TEST_FLAGS} -c $< &>$(@:.o=.out)


check-local: memObjectGenerator
	@echo "Test for Abstrace Memory Object Interface."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/abstractMemoryObjectTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot *.out 

EXTRA_DIST =  $(ALL_TESTCODES)

CLEANFILES = 



