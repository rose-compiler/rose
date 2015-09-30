include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

ROSE2LLVM_FLAGS = --edg:no-warnings -rose:verbose 0

C_TESTCODES_REQUIRED_TO_PASS = \
							   example0.c \
							   example1.c \
							   example3.c \
							   example4.c \
							   example5.c \
							   arrayref.c \
							   arrayref1.c

# example2.c - rose2llvm cannot compile
# switch5.c, switch6.c - output files differ
# test.c - need to remove test script for this to run

C_TESTCODES_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(C_TESTCODES_CURRENTLY_FAILING)	

if ROSE_USE_LLVM

# GENERATE Executables using rose2llvm
C_TESTS = $(C_TESTCODES_REQUIRED_TO_PASS)

../src/rose2llvmAA:
	$(MAKE) -C ../src rose2llvmAA

$(C_TESTS): ../src/rose2llvmAA 
	@cp $(srcdir)/$@ $@
	../src/rose2llvmAA $@ > $@.out

check-local:
	@echo "Test for rose2llvm translator."	
	@$(MAKE) $(C_TESTS)
	@echo "======== make check rule for rose2llvmAA terminated normally ========"

clean-local:	
	rm -f *.out *.c

endif

EXTRA_DIST = $(ALL_TESTCODES) 
