
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## "if" for DQ_DEVELOPER_TESTS (AutoMake conditional block)
#if DQ_DEVELOPER_TESTS
#
#INCLUDES = $(ROSE_INCLUDES)
#
##bin_PROGRAMS = copyExample
#
##copyExample_SOURCES = copyExample.C
##copyExample_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#
#
#TESTCODES_REQUIRED_TO_PASS = test_preinclude.C
#
#TESTCODE_CURRENTLY_FAILING =
#
## Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
#TESTCODES = \
#$(TESTCODES_REQUIRED_TO_PASS)
#
## QMTest allows both passing and failing tests.
#ALL_TESTCODES = \
#$(TESTCODES_REQUIRED_TO_PASS) \
#$(TESTCODE_CURRENTLY_FAILING)
#
## ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
#ROSE_FLAGS = -rose:verbose 0 --edg:restrict
#
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
## VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
#PASSING_TEST_Objects = ${TESTCODES:.C=.o}
#TEST_Objects = ${ALL_TESTCODES:.C=.o}
#
## DQ (1/13/2009): The GNU option is "-include" not "--include"
#$(TEST_Objects): ../preprocessor $(srcdir)/$(@:.o=.C)
#	@echo "Calling Copy AST mechanism using TEST_Objects rule: "
#	$(VALGRIND) ../preprocessor $(ROSE_FLAGS) -include $(srcdir)/test_preinclude_1.h -c $(srcdir)/$(@:.o=.C) &
##	$(VALGRIND) ../preprocessor $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)
#
#QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}
#
## Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): ../preprocessor $(srcdir)/$(@:.qmt=.C)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../preprocessor NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)
#
## This is a rule to add the current files into a QMTest database
#build_qmtest_database: $(QMTEST_Objects)
##	@echo "Building QMTest database using file "
#	rm -rf QMTest;
#	mkdir QMTest;
#	rm -rf QMTest/*.qmt
#	mv *.qmt QMTest
#	cd QMTest; qmtest -D`pwd` create-tdb
#
#run_qm_tests:
#	cd QMTest; qmtest run -o TestResults.qmr -j4
#
#run_qm_gui:
#	cd QMTest; qmtest gui
#
#EXTRA_DIST = test_preinclude_1.h
#
#check-local:
#	@echo "Dan Quinlan's development tests for ROSE preinclude mechanism."
##  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
##	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects)
#	@echo "***********************************************************************************************************"
#	@echo "****** ROSE/developersScratchSpace/Dan/preinclude_tests: make check rule complete (terminated normally) ******"
#	@echo "***********************************************************************************************************"
#else
#check-local:
#	@echo "\nDan Quinlan's preinclude commandline option development tests NOT run.  To run these reconfigure with:"
#	@echo "********************************************************************************************************"
#	@echo "******************** make check not run here: configure --enable-dq-developer-tests ********************"
#	@echo "********************************************************************************************************"
#endif
#
