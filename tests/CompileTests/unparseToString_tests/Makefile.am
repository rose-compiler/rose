include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

########################################################################################################################
# Executables.
########################################################################################################################
noinst_PROGRAMS =

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += unparseToString
unparseToString_SOURCES = unparseToString.C
unparseToString_CPPFLAGS = $(ROSE_INCLUDES)
unparseToString_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

########################################################################################################################
# Tests.  We currently have two tests (which are both the same executable but invoked with different switches) that
# operate over a big list of specimens (*.C files all from a common directory).
########################################################################################################################

# These specimens either have compile errors (at least when using the compiler switches specified below) or they cause
# ROSE to abort before the frontend() call returns.  You can easily see the failures by commenting out this line and
# re-running "make -k check".
EXCLUDE_SPECIMENS =													\
	test2005_201.C test2005_56.C test2006_11.C test2006_130.C test2006_131.C test2006_49.C test2006_60.C		\
	test2007_105.C test2007_106.C test2007_108.C test2007_109.C test2007_112.C test2007_113.C test2007_114.C	\
	test2007_123.C test2007_125.C test2007_153.C test2007_19.C test2007_20.C test2007_30.C test2007_35.C		\
	test2007_69.C test2007_86.C test2007_95.C test2009_16.C test2009_23.C test2009_26.C test2009_36.C test2009_38.C	\
	test2010_01.C test2010_02.C test2010_07.C test2010_08.C test2010_09.C test2010_13.C test2010_14.C test2010_16.C	\
	test2010_27.C test2011_112.C test2011_130.C test2011_134.C test2011_16.C test_CplusplusMacro_C.C test_gnu.C	\
	test2005_201.C test2005_56.C test2006_11.C test2006_130.C test2006_131.C test2006_49.C test2006_60.C		\
	test2007_105.C test2007_106.C test2007_108.C test2007_109.C test2007_112.C test2007_113.C test2007_114.C	\
	test2007_123.C test2007_125.C test2007_153.C test2007_19.C test2007_20.C test2007_30.C test2007_35.C		\
	test2007_69.C test2007_86.C test2007_95.C test2009_16.C test2009_23.C test2009_26.C test2009_36.C test2009_38.C	\
	test2010_01.C test2010_02.C test2010_07.C test2010_08.C test2010_09.C test2010_13.C test2010_14.C test2010_16.C	\
	test2010_27.C test2011_112.C test2011_130.C test2011_134.C test2011_16.C test_CplusplusMacro_C.C test_gnu.C

# The list of specimens (which must all be from one directory so as to not be ambiguous)
SPECIMEN_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
SPECIMENS = $(filter-out $(EXCLUDE_SPECIMENS), $(notdir $(wildcard $(SPECIMEN_DIR)/*.C)))
TEST_CONFIG = $(top_srcdir)/scripts/test_exit_status
TEST_INCLUDES = -I$(SPECIMEN_DIR) -I$(top_srcdir)/tests/CompileTests/A++Code

# The makefile targets for "make check"
unparseAll_TEST_TARGETS = $(addprefix ua_, $(addsuffix .passed, $(SPECIMENS)))
unparseType_TEST_TARGETS = $(addprefix ut_, $(addsuffix .passed, $(SPECIMENS)))
TEST_TARGETS = $(unparseAll_TEST_TARGETS) $(unparseType_TEST_TARGETS)

# A couple of phony targets for running only one test across all the specimens
.PHONY: check-unparseAll check-unparseType
check-unparseAll: $(unparseAll_TEST_TARGETS)
check-unparseType: $(unparseType_TEST_TARGETS)

# How to run each test across each specimen
$(unparseAll_TEST_TARGETS): ua_%.passed: $(SPECIMEN_DIR)/% $(TEST_CONFIG) unparseToString
	@$(RTH_RUN) CMD="./unparseToString --all --edg:no_warnings -w --edg:restrict $(TEST_INCLUDES) -c $<" $(TEST_CONFIG) $@
$(unparseType_TEST_TARGETS): ut_%.passed: $(SPECIMEN_DIR)/% $(TEST_CONFIG) unparseToString
	@$(RTH_RUN) CMD="./unparseToString       --edg:no_warnings -w --edg:restrict $(TEST_INCLUDES) -c $<" $(TEST_CONFIG) $@

########################################################################################################################
# Additional automake rules
########################################################################################################################

check-local: $(TEST_TARGETS)

# These "rm" commands are split up because the argument lists are too long for some shells.
clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
	rm -f $(SPECIMENS:.C=.o)
	rm -f $(addprefix rose_, $(SPECIMENS))
