
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(hash_multimap_SRCS hash_multimap.C)

kde4_add_executable(hash_multimap ${hash_multimap_SRCS})

target_link_libraries(hash_multimap ${KDE4_KDECORE_LIBS})

install(TARGETS hash_multimap DESTINATION bin)


########### next target ###############

set(graph_test_1_SRCS graph_test_1.C)

kde4_add_executable(graph_test_1 ${graph_test_1_SRCS})

target_link_libraries(graph_test_1 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_1 DESTINATION bin)


########### next target ###############

set(graph_test_2_SRCS graph_test_2.C)

kde4_add_executable(graph_test_2 ${graph_test_2_SRCS})

target_link_libraries(graph_test_2 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_2 DESTINATION bin)


########### next target ###############

set(graph_test_3_SRCS graph_test_3.C)

kde4_add_executable(graph_test_3 ${graph_test_3_SRCS})

target_link_libraries(graph_test_3 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_3 DESTINATION bin)


########### next target ###############

set(graph_test_4_SRCS graph_test_4.C)

kde4_add_executable(graph_test_4 ${graph_test_4_SRCS})

target_link_libraries(graph_test_4 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_4 DESTINATION bin)


########### next target ###############

set(graph_test_5_SRCS graph_test_5.C)

kde4_add_executable(graph_test_5 ${graph_test_5_SRCS})

target_link_libraries(graph_test_5 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_5 DESTINATION bin)


########### next target ###############

set(graph_test_6_SRCS graph_test_6.C)

kde4_add_executable(graph_test_6 ${graph_test_6_SRCS})

target_link_libraries(graph_test_6 ${KDE4_KDECORE_LIBS})

install(TARGETS graph_test_6 DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
###################################################################################
## These are testcodes for testing the new Graph IR nodes in ROSE
###################################################################################
#
## "if" for DQ_DEVELOPER_TESTS (AutoMake conditional block)
#if DQ_DEVELOPER_TESTS 
#
## if ROSE_USE_NEW_GRAPH_NODES
#
#INCLUDES = $(ROSE_INCLUDES)
#
## This is a shorted list that tests more quickly.
#TESTCODES_REQUIRED_TO_PASS = \
#   inputCode_main.C 
#
#TESTCODES_CURRENTLY_FAILING = 
#
## Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
#TESTCODES = \
#$(TESTCODES_REQUIRED_TO_PASS)
#
## QMTest allows both passing and failing tests.
#ALL_TESTCODES = \
#$(TESTCODES_REQUIRED_TO_PASS) \
#$(TESTCODES_CURRENTLY_FAILING)
#
#
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
## VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
## VALGRIND = valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
#PASSING_TEST_Objects = ${TESTCODES:.C=.o}
#TEST_Objects = ${ALL_TESTCODES:.C=.o}
#
#ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0
#
#$(TEST_Objects):
#	echo "Copy the file to build a second one to main ..."
#	$(VALGRIND) ./graph_test_1 $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#
#bin_PROGRAMS = hash_multimap graph_test_1 graph_test_2 graph_test_3 graph_test_4 graph_test_5 graph_test_6 
#
#hash_multimap_SOURCES = hash_multimap.C
#graph_test_1_SOURCES = graph_test_1.C
#graph_test_2_SOURCES = graph_test_2.C
#graph_test_3_SOURCES = graph_test_3.C
#graph_test_4_SOURCES = graph_test_4.C
#graph_test_5_SOURCES = graph_test_5.C
#graph_test_6_SOURCES = graph_test_6.C
#
#
#test_hash_multimap: hash_multimap
#	$(VALGRIND) ./hash_multimap
#
#test_1: graph_test_1
#	$(VALGRIND) ./graph_test_1
#
#test_2: graph_test_2
#	$(VALGRIND) ./graph_test_2
#
#test_3: graph_test_3
#	$(VALGRIND) ./graph_test_3
#
#test_4: graph_test_4
#	$(VALGRIND) ./graph_test_4 $(srcdir)/inputCode_main.C
#
#test_5: graph_test_5
#	$(VALGRIND) ./graph_test_5 $(srcdir)/inputCode_main.C
#
#test_6: graph_test_6
#	$(VALGRIND) ./graph_test_6 $(srcdir)/inputCode_main.C
#
#small_binary_test_6: graph_test_6
#	$(VALGRIND) ./graph_test_6 $(srcdir)/../Disassembler_tests/x86-64-ctrlaltdel
#
#large_binary_test_6: graph_test_6
#	$(VALGRIND) ./graph_test_6 $(srcdir)/../Disassembler_tests/pocketputty.exe
#
#
#QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}
#
## Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): graph_test_1 $(srcdir)/$(@:.qmt=.C)
#	@echo "Calling QMTEST_Objects rule: "
##	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/main NULL $(ROSE_FLAGS) -c $(srcdir)/tests/$(@:.qmt=.C)
#
## Include makefile rules specific to QMTest
#include $(top_srcdir)/config/QMTest_makefile.inc
#
## EXTRA_DIST = tests 
#
#check-local:
#	@echo "Dan Quinlan's development tests for new graph IR nodes."
##  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
##	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects)
#	@echo "******************************************************************************************************************"
#	@echo "****** ROSE/developersScratchSpace/Dan/graph_tests: make check rule complete (terminated normally) ******"
#	@echo "******************************************************************************************************************"
#
## endif
#
#else
#check-local:
#	@echo "\nDan Quinlan's development tests for new graph IR nodes, tests NOT run.  To run these reconfigure with:"
#	@echo "*********************************************************************************************************************************"
#	@echo "******************* make check not run here: configure --enable-newGraphNodes and --enable-dq-developer-tests *******************"
#	@echo "*********************************************************************************************************************************"
#endif
#
#clean-local:
#	rm -f *.o rose_*.[cC] *.dot test200?_*.[cC]
#	rm -rf QMTest
