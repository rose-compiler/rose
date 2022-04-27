include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsLists0
testTraitsLists0_SOURCES = testTraitsLists0.C
testTraitsLists0_CPPFLAGS = -ftemplate-depth=2000
testTraitsLists0_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsLists0.passed

testTraitsLists0.passed: testTraitsLists0
	@$(RTH_RUN) CMD="./testTraitsLists0" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsLists1
testTraitsLists1_SOURCES = testTraitsLists1.C
testTraitsLists1_CPPFLAGS = -ftemplate-depth=2000
testTraitsLists1_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsLists1.passed

testTraitsLists1.passed: testTraitsLists1
	@$(RTH_RUN) CMD="./testTraitsLists1" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsPredicates0
testTraitsPredicates0_SOURCES = testTraitsPredicates0.C
testTraitsPredicates0_CPPFLAGS = -ftemplate-depth=2000
testTraitsPredicates0_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsPredicates0.passed

testTraitsPredicates0.passed: testTraitsPredicates0
	@$(RTH_RUN) CMD="./testTraitsPredicates0" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsDescribe0
testTraitsDescribe0_SOURCES = testTraitsDescribe0.C
testTraitsDescribe0_CPPFLAGS = -ftemplate-depth=2000
testTraitsDescribe0_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsDescribe0.passed

testTraitsDescribe0.passed: testTraitsDescribe0
	@$(RTH_RUN) CMD="./testTraitsDescribe0" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsTraversal0
testTraitsTraversal0_SOURCES = testTraitsTraversal0.C
testTraitsTraversal0_CPPFLAGS = -ftemplate-depth=2000 -DUSE_META_TRAVERSAL=1
testTraitsTraversal0_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsTraversal0.passed

testTraitsTraversal0.passed: testTraitsTraversal0
	@$(RTH_RUN) CMD="./testTraitsTraversal0" $(TEST_EXIT_STATUS) $@

noinst_PROGRAMS += testTraitsTraversalOld0
testTraitsTraversalOld0_SOURCES = testTraitsTraversal0.C
testTraitsTraversalOld0_CXXFLAGS = -ftemplate-depth=2000 -DUSE_META_TRAVERSAL=0 -O2
testTraitsTraversalOld0_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsTraversalOld0.passed

testTraitsTraversalOld0.passed: testTraitsTraversalOld0
	@$(RTH_RUN) CMD="./testTraitsTraversalOld0" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testTraitsTraversal1
testTraitsTraversal1_SOURCES = testTraitsTraversal1.C
testTraitsTraversal1_CXXFLAGS = -ftemplate-depth=2000 -DUSE_META_TRAVERSAL=1 -O2
testTraitsTraversal1_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testTraitsTraversal1.passed

testTraitsTraversal1.passed: testTraitsTraversal1
	@$(RTH_RUN) CMD="./testTraitsTraversal1" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
