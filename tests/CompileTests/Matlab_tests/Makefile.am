include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS = matlabIdentityTranslator
matlabIdentityTranslator_SOURCES = matlabIdentityTranslator.C
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/MatlabFrontend -I$(top_srcdir)/src/backend/unparser/MatlabCodeGeneration/
matlabIdentityTranslator_LDADD = $(ROSE_LIBS) $(top_builddir)/src/frontend/MatlabFrontend/libroseMatlabFrontend.la

ALL_MATLAB_TESTCODES = \
	test_forloop.m \
	test_forloop_stride.m \
	test_function1.m \
	test_function2.m \
	test_if.m \
	test_if_multiple.m \
	test_minus_unaryop.m \
	test_operators.m \
	test_transpose.m

TEST_Objects = ${ALL_MATLAB_TESTCODES:.m=.test}


#Compare the unparsed code and original code
#Remove new lines, comma, semicolon and spaces for easier diff
$(TEST_Objects): matlabIdentityTranslator
	./matlabIdentityTranslator  $(srcdir)/$(@:.test=.m)
	tr '\n' ' ' < $(srcdir)/$(@:.test=.m) > truncated_$(@:.test=.m)
	tr '\n' ' ' < rose_$(@:.test=.m) > truncated_rose_$(@:.test=.m)
	sed -i -e 's/ //g;s/;//g;s/,//g;' truncated_$(@:.test=.m)
	sed -i -e 's/ //g;s/;//g;s/,//g;' truncated_rose_$(@:.test=.m)
	diff truncated_$(@:.test=.m) truncated_rose_$(@:.test=.m)

check-local:
	@echo "Testing Matlab."
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "***** ROSE/tests/CompileTests/Matlab_tests: make check rule complete (terminated normally) ****"
	@echo "***********************************************************************************************"

clean-local:
	rm -f rose_*.m truncated_*

EXTRA_DIST = $(ALL_MATLAB_TESTCODES)
