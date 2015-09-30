include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

#------------------------------------------------------------------------------------------------------------------------
# hash_multimap

noinst_PROGRAMS += hash_multimap
hash_multimap_SOURCES = hash_multimap.C

TEST_TARGETS += hash_multimap.passed
hash_multimap.passed: hash_multimap
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/hash_multimap" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# graph_test_1

noinst_PROGRAMS += graph_test_1
graph_test_1_SOURCES = graph_test_1.C

graph_test_1_sage.passed: graph_test_1
	@$(RTH_RUN)				\
		USE_SUBDIR=yes			\
		CMD="$(abspath $<) sage"	\
		$(TEST_EXIT_STATUS) $@

graph_test_1_sawyer.passed: graph_test_1
	@$(RTH_RUN)				\
		USE_SUBDIR=yes			\
		CMD="$(abspath $<) sawyer"	\
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += graph_test_1_sage.passed graph_test_1_sawyer.passed

#------------------------------------------------------------------------------------------------------------------------
# graph_test_2

noinst_PROGRAMS += graph_test_2
graph_test_2_SOURCES = graph_test_2.C

TEST_TARGETS += graph_test_2.passed
graph_test_2.passed: graph_test_2
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_2" \
		$(TEST_EXIT_STATUS) $@


#------------------------------------------------------------------------------------------------------------------------
# graph_test_3

noinst_PROGRAMS += graph_test_3
graph_test_3_SOURCES = graph_test_3.C

TEST_TARGETS += graph_test_3.passed
graph_test_3.passed: graph_test_3
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_3" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# graph_test_4

noinst_PROGRAMS += graph_test_4
graph_test_4_SOURCES = graph_test_4.C

TEST_TARGETS += graph_test_4.passed
EXTRA_DIST += inputCode_main.C
graph_test_4.passed: inputCode_main.C graph_test_4
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_4 $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# graph_test_5

noinst_PROGRAMS += graph_test_5
graph_test_5_SOURCES = graph_test_5.C

TEST_TARGETS += graph_test_5.passed
EXTRA_DIST += inputCode_main.C
graph_test_5.passed: inputCode_main.C graph_test_5
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_5 $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# graph_test_6

noinst_PROGRAMS += graph_test_6 
graph_test_6_SOURCES = graph_test_6.C

TEST_TARGETS += graph_test_6a.passed
EXTRA_DIST += inputCode_main.C
graph_test_6a.passed: inputCode_main.C graph_test_6
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_6 $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += graph_test_6b.passed
graph_test_6b.passed: $(top_srcdir)/binaries/samples/x86-64-ctrlaltdel graph_test_6
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_6 $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += graph_test_6c.passed
graph_test_6c.passed: $(top_srcdir)/binaries/samples/pocketputty.exe graph_test_6
	@$(RTH_RUN) \
		DISABLED="not automatically tested by original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/graph_test_6 $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# qm.sh

qm_script = $(abspath $(top_srcdir)/scripts/qm.sh)
qm_specimens = inputCode_main.C
qm_test_targets = $(addprefix qm_, $(addsuffix .passed, $(qm_specimens)))
EXTRA_DIST += $(qm_specimens)

TEST_TARGETS += $(qm_test_targets)
$(qm_test_targets): qm_%.passed: %
	@$(RTH_RUN) \
		DISABLED="commented out in original makefile" \
		USE_SUBDIR=yes \
		CMD="$(qm_script) f rose.RoseTest $$(pwd)/main NULL $(ROSE_FLAGS) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

QMTEST_Objects = $(qm_specimens:.C=.o)
include $(top_srcdir)/config/QMTest_makefile.inc

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)

EXTRA_DIST += CMakeLists.txt
