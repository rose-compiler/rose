include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# testNodeId
#
# This is a little bit of a weird test series.  I wanted to verify two things:
# 1. The NodeId stuff is basically working
# 2. NodeIds are stable between runs (assuming same version of ROSE and same input file ordering)
# So for these tests I used the specimens from astInliningTests (just because they were convinient)
# and I output the nodeId and nodeType for every node in the project in pre order.
# I run that twice and put the output in <specimen_name>_v1 and <specimen_name>_v2.  If there is
# a difference between the two runs, the test should fail.

bin_PROGRAMS = testNodeId
testNodeId_SOURCES = testNodeId.C
testNodeId_CPPFLAGS = $(ROSE_INCLUDES)
testNodeId_LDFLAGS = $(ROSE_RPATHS)
testNodeId_LDADD = $(ROSE_SEPARATE_LIBS)

# No longer has name convention restrictions  since we don't use RTH anymore.
# Note: must use the name convention of specimenXX_N.C , in which N is the number of function calls inlined.   
# The specimens are named so that the number between the "_" and next "." is the number of function calls that
# we expect this specimen to inline.
testNodeId_specimens =				\
	$(srcdir)/../astInliningTests/specimen01_1.C 				\
	$(srcdir)/../astInliningTests/specimen02_1.C				\
	$(srcdir)/../astInliningTests/specimen03_1.C				\
	$(srcdir)/../astInliningTests/specimen04_1.C				\
	$(srcdir)/../astInliningTests/specimen05_1.C				\
	$(srcdir)/../astInliningTests/specimen06_1.C				\
	$(srcdir)/../astInliningTests/specimen07_1.C				\
	$(srcdir)/../astInliningTests/specimen08_1.C				\
	$(srcdir)/../astInliningTests/specimen09_1.C				\
	$(srcdir)/../astInliningTests/specimen10_1.C				\
	$(srcdir)/../astInliningTests/specimen11_3.C				\
	$(srcdir)/../astInliningTests/specimen12_3.C				\
	$(srcdir)/../astInliningTests/specimen13_1.C				\
	$(srcdir)/../astInliningTests/specimen14_1.C				\
	$(srcdir)/../astInliningTests/specimen15_1.C				\
	$(srcdir)/../astInliningTests/specimen16_4.C				\
	$(srcdir)/../astInliningTests/specimen17_10.C				\
	$(srcdir)/../astInliningTests/specimen18_1.C				\
	$(srcdir)/../astInliningTests/specimen19_1.C				\
	$(srcdir)/../astInliningTests/specimen20_1.C				\
	$(srcdir)/../astInliningTests/specimen21_1.C				\
	$(srcdir)/../astInliningTests/specimen22_1.C				\
	$(srcdir)/../astInliningTests/specimen25_1.C 




EXTRA_DIST += $(testNodeId_specimens) testNodeId.conf


# test default behavior
# Jim Leek: Sigh, this is so jank.  Please don't use as example
#-----------------------------------------------------------
NODEID_TEST_NAMES = $(notdir $(testNodeId_specimens))
NODEID_TEST_TARGETS = $(addsuffix .passed, $(NODEID_TEST_NAMES)) 

TEST_TARGETS += $(NODEID_TEST_TARGETS)

$(NODEID_TEST_TARGETS): %.passed : testNodeId $(testNodeId_specimens)
	$(srcdir)/testscript.sh $(builddir)/testNodeId $(basename $(srcdir)/../astInliningTests/$@) $(builddir)


EXTRA_DIST += testNodeId.conf 

check-local: $(TEST_TARGETS) 
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/astNodeId: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -f $(TEST_TARGETS) *_v1 *_v2 *.log *.passed
	rm -f $(TEST_TARGETS:.passed=.failed)
