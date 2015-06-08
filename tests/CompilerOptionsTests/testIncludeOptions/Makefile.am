include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator 

INCLUDES = $(ROSE_INCLUDES)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TESTCODES_REQUIRED_TO_PASS = test_include.c

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = -rose:verbose 0

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

absolute_srcdir=`cd $(srcdir) && pwd`

# DQ (1/13/2009): The GNU option is "-include" not "--include"
$(TEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.c)
#	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) -include $(absolute_srcdir)/test_preinclude_1.h -c $(srcdir)/$(@:.o=.C)
#	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) --sys_include $(srcdir)/system_include -I$(srcdir)/include -c $(srcdir)/$(@:.o=.c)
	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) -isystem $(srcdir)/system_include -I$(srcdir)/include -c $(srcdir)/$(@:.o=.c)

test_include_rose:
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) --sys_include $(srcdir)/system_include -I$(srcdir)/include -c $(srcdir)/test_include.c
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -isystem $(srcdir)/system_include -I$(srcdir)/include -c $(srcdir)/test_include.c

test_include_gcc:
	gcc -isystem $(srcdir)/system_include -I$(srcdir)/include -c $(srcdir)/test_include.c

EXTRA_DIST = CMakeLists.txt test_include_1.h $(ALL_TESTCODES)

check-local:
	@echo "Development tests for ROSE Include options."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "****************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionsTests/testIncludeOptions: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.c
