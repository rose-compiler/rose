include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_LLVM

TESTCODE_INCLUDES = 

ROSE2LLVM_FLAGS =

CLANG=clang
CLANG_FLAGS=-lm

DIFF=diff

C_TESTCODES_REQUIRED_TO_PASS = \
							   example1.c \
							   example3.c \
							   example4.c \
							   example5.c \
							   example6.c \
							   example7.c \
							   example8.c \
							   example9.c \
							   example10.c \
							   example11.c \
							   example13.c \
							   example14.c \
							   example15.c \
							   example16.c \
							   example17.c \
							   example18.c \
							   example19.c \
							   example20.c \
							   example21.c \
							   example22.c \
							   m.c \
							   mm.c \
							   mort.c \
							   mortgage.c \
							   mortg.c \
							   t.c \
							   switch0.c \
							   switch1.c \
							   switch2.c \
							   switch3.c \
							   switch4.c 

# example2.c - rose2llvm cannot compile
# switch5.c, switch6.c - output files differ

C_TESTCODES_CURRENTLY_FAILING = \
 								example2.c \
								switch5.c \
								switch6.c

# GENERATE Executables using rose2llvm
C_TEST_EXECUTABLES = $(C_TESTCODES_REQUIRED_TO_PASS:.c=)

# Generate executables using clang
C_TEST_CLANG_EXECUTABLES = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.clang)

# run the executables by rose2llvm	
C_TEST_OUT = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.c.out)

# run the executables by clang	
C_TEST_CLANG_OUT = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.c.clang.out)	

# diff the output of 2 executables	
DIFF_CHECK = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.c.diff)	

../src/rose2llvm:
	$(MAKE) -C ../src rose2llvm

$(C_TEST_EXECUTABLES): ../src/rose2llvm 
	../src/rose2llvm $(TESTCODE_INCLUDES) $(ROSE2LLVM_FLAGS) -o $@ $(srcdir)/$(@:=.c)

$(C_TEST_OUT): $(C_TEST_EXECUTABLES)
	./$(@:.c.out=) < $(srcdir)/$(@:.c.out=.c.in) > $@	

$(C_TEST_CLANG_EXECUTABLES):
	$(CLANG) $(TESTCODE_INCLUDES) $(CLANG_FLAGS) -o $@ $(srcdir)/$(@:.clang=.c)

$(C_TEST_CLANG_OUT): $(C_TEST_CLANG_EXECUTABLES)
	./$(@:.c.clang.out=.clang) < $(srcdir)/$(@:.c.clang.out=.c.in) > $@

$(DIFF_CHECK): $(C_TEST_OUT) $(C_TEST_CLANG_OUT)
	@if $(DIFF)	$(@:.c.diff=.c.out) $(@:.c.diff=.c.clang.out); then :; else echo "Files differ; test falied"; exit 1; fi

check-local:
	@echo "Test for rose2llvm translator."
	@$(MAKE) $(C_TEST_EXECUTABLES)
	@$(MAKE) $(C_TEST_OUT)
	@$(MAKE) $(C_TEST_CLANG_EXECUTABLES)
	@$(MAKE) $(C_TEST_CLANG_OUT)
	@$(MAKE) $(DIFF_CHECK)
	@echo "======== make check rule for rose2llvm terminated normally ========"

clean-local:	
	rm -f $(C_TEST_EXECUTABLES) *.ll *.out *.clang

endif
