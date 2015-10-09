include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
AM_CPPFLAGS = $(ROSE_INCLUDES)
TEST_TARGETS =
TEST_CONFIG = $(top_srcdir)/scripts/test_exit_status
EXTRA_DIST =
MOSTLYCLEANFILES =

########################################################################################################################
# The test programs, how to run them, and on which specimens they should run.  All information about a particular test
# is localized to its few contiguous lines in this makefile.
########################################################################################################################

noinst_PROGRAMS += proFunSIG
proFunSIG_SOURCES = proFunSIG.C
proFunSIG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
proFunSIG_SPECIMENS = mf1.C mf3.C mf4.C mf5.C
proFunSIG_TEST_TARGETS = $(addprefix pfs_, $(addsuffix .passed, $(proFunSIG_SPECIMENS)))

$(proFunSIG_TEST_TARGETS): pfs_%.passed: % $(TEST_CONFIG) proFunSIG
	@$(RTH_RUN) CMD="./proFunSIG $<" $(TEST_CONFIG) $@

.PHONY: check-proFunSIG
check-proFunSIG: $(proFunSIG_TEST_TARGETS)

EXTRA_DIST += $(proFunSIG_SPECIMENS)
TEST_TARGETS += $(proFunSIG_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(proFunSIG_SPECIMENS)) \
	bar.dot barfoo.dot foo.dot hotness0.dot slow.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += interproceduralCFG
interproceduralCFG_SOURCES = interproceduralCFG.C
interproceduralCFG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
interproceduralCFG_SPECIMENS = test11.C test12.C test13.C test14.C
interproceduralCFG_TEST_TARGETS = $(addprefix ipcfg_, $(addsuffix .passed, $(interproceduralCFG_SPECIMENS)))

$(interproceduralCFG_TEST_TARGETS): ipcfg_%.passed: % $(TEST_CONFIG) interproceduralCFG
	@$(RTH_RUN) CMD="./interproceduralCFG $<" $(TEST_CONFIG) $@

.PHONY: check-interproceduralCFG
check-interproceduralCFG: $(interproceduralCFG_TEST_TARGETS)

EXTRA_DIST += $(interproceduralCFG_SPECIMENS)
TEST_TARGETS += $(interproceduralCFG_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(interproceduralCFG_SPECIMENS)) \
	hotness0.dot main.0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += e0
e0_SOURCES = efor2.C.maintest.C
e0_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

e0_efor2.C.passed: efor2.C $(TEST_CONFIG) e0
	@$(RTH_RUN) CMD="./e0 $<" $(TEST_CONFIG) $@

.PHONY: check-e0
check-e0: e0_efor2.C.passed

EXTRA_DIST += efor2.C
#TEST_TARGETS += e0_efor2.C.passed			## not checked in original makefile
MOSTLYCLEANFILES += hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += e1
e1_SOURCES = efor3.C.maintest.C
e1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

e1_efor3.C.passed: efor3.C $(TEST_CONFIG) e1
	@$(RTH_RUN) CMD="./e1 $<" $(TEST_CONFIG) $@

.PHONY: check-e1
check-e1: e1_efor3.C.passed

EXTRA_DIST += efor3.C
#TEST_TARGETS += e1_efor3.C.passed			## not checked in original makefile
MOSTLYCLEANFILES += efor3.C.main.dot efor3.C.maintest.o hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += ff1
ff1_SOURCES = tf1.C.maintest.C
ff1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

ff1_tf1.C.passed: tf1.C $(TEST_CONFIG) ff1
	@$(RTH_RUN) CMD="./ff1 $<" $(TEST_CONFIG) $@

.PHONY: check-ff1
check-ff1: ff1_tf1.C.passed

EXTRA_DIST += tf1.C
TEST_TARGETS += ff1_tf1.C.passed
MOSTLYCLEANFILES += tf1.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += ff2
ff2_SOURCES = tf2.C.maintest.C
ff2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

ff2_tf2.C.passed: tf2.C $(TEST_CONFIG) ff2
	@$(RTH_RUN) CMD="./ff2 $<" $(TEST_CONFIG) $@

.PHONY: check-ff2
check-ff2: ff2_tf2.C.passed

EXTRA_DIST += tf2.C
TEST_TARGETS += ff2_tf2.C.passed
MOSTLYCLEANFILES += tf2.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += ff3
ff3_SOURCES = tf3.C.maintest.C
ff3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

ff3_tf3.C.passed: tf3.C $(TEST_CONFIG) ff3
	@$(RTH_RUN) CMD="./ff3 $<" $(TEST_CONFIG) $@

.PHONY: check-ff3
check-ff3: ff3_tf3.C.passed

EXTRA_DIST += tf3.C
TEST_TARGETS += ff3_tf3.C.passed
MOSTLYCLEANFILES += tf3.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += f1
f1_SOURCES = tfif1.C.maintest.C
f1_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

f1_tfif1.C.passed: tfif1.C $(TEST_CONFIG) f1
	@$(RTH_RUN) CMD="./f1 $<" $(TEST_CONFIG) $@

.PHONY: check-f1
check-f1: f1_tfif1.C.passed

EXTRA_DIST += tfif1.C
TEST_TARGETS += f1_tfif1.C.passed
MOSTLYCLEANFILES += tfif1.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += f2
f2_SOURCES = tfif2.C.maintest.C
f2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

f2_tfif2.C.passed: tfif2.C $(TEST_CONFIG) f2
	@$(RTH_RUN) CMD="./f2 $<" $(TEST_CONFIG) $@

.PHONY: check-f2
check-f2: f2_tfif2.C.passed

EXTRA_DIST += tfif2.C
TEST_TARGETS += f2_tfif2.C.passed
MOSTLYCLEANFILES += tfif2.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += f3
f3_SOURCES = tfif3.C.maintest.C
f3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

f3_tfif3.C.passed: tfif3.C $(TEST_CONFIG) f3
	@$(RTH_RUN) CMD="./f3 $<" $(TEST_CONFIG) $@

.PHONY: check-f3
check-f3: f3_tfif3.C.passed

EXTRA_DIST += tfif3.C
TEST_TARGETS += f3_tfif3.C.passed
MOSTLYCLEANFILES += tfif3.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += f4
f4_SOURCES = tfif4.C.maintest.C
f4_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

f4_tfif4.C.passed: tfif4.C $(TEST_CONFIG) f4
	@$(RTH_RUN) CMD="./f4 $<" $(TEST_CONFIG) $@

.PHONY: check-f4
check-f4: f4_tfif4.C.passed

EXTRA_DIST += tfif4.C
TEST_TARGETS += f4_tfif4.C.passed
MOSTLYCLEANFILES += tfif4.C.main.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += createTest
createTest_SOURCES = createTest.C
createTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# createTest is not tested in the original makefile
.PHONY: check-createTest
check-createTest:
	@echo "Nothing to test"

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += astTraversalTest
astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
astTraversalTest_SPECIMENS    = input1.C
astTraversalTest_TEST_TARGETS = $(addprefix att_, $(addsuffix .passed, $(astTraversalTest_SPECIMENS)))

$(astTraversalTest_TEST_TARGETS): att_%.passed: % $(TEST_CONFIG) astTraversalTest
	@$(RTH_RUN) CMD="./astTraversalTest -edg:w -c $<" $(TEST_CONFIG) $@

.PHONY: check-astTraversalTest
check-astTraversalTest: $(astTraversalTest_TEST_TARGETS)

EXTRA_DIST += $(astTraversalTest_SPECIMENS)
TEST_TARGETS += $(astTraversalTest_TEST_TARGETS)
MOSTLYCLEANFILES += rose_input1.C

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += processnew3Down4SgIncGraph2
processnew3Down4SgIncGraph2_SOURCES      = processnew3Down4SgIncGraph2.C
processnew3Down4SgIncGraph2_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
processnew3Down4SgIncGraph2_SPECIMENS    = test11.C test12.C test13.C test14.C
processnew3Down4SgIncGraph2_TEST_TARGETS = $(addprefix pnew2_, $(addsuffix .passed, $(processnew3Down4SgIncGraph2_SPECIMENS)))

$(processnew3Down4SgIncGraph2_TEST_TARGETS): pnew2_%.passed: % $(TEST_CONFIG) processnew3Down4SgIncGraph2
	@$(RTH_RUN) CMD="./processnew3Down4SgIncGraph2 $<" $(TEST_CONFIG) $@

.PHONY: check-processnew3Down4SgIncGraph2
check-processnew3Down4SgIncGraph2: $(processnew3Down4SgIncGraph2_TEST_TARGETS)

EXTRA_DIST += $(processnew3Down4SgIncGraph2_SPECIMENS)
TEST_TARGETS += $(processnew3Down4SgIncGraph2_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(processnew3Down4SgIncGraph2_SPECIMENS)) hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += processnew3Down4SgIncGraph3
processnew3Down4SgIncGraph3_SOURCES      = processnew3Down4SgIncGraph3.C
processnew3Down4SgIncGraph3_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
processnew3Down4SgIncGraph3_SPECIMENS    = eif.C eif2.C eif3.C eif4.C
processnew3Down4SgIncGraph3_TEST_TARGETS = $(addprefix pnew3_, $(addsuffix .passed, $(processnew3Down4SgIncGraph3_SPECIMENS)))

$(processnew3Down4SgIncGraph3_TEST_TARGETS): pnew3_%.passed: % $(TEST_CONFIG) processnew3Down4SgIncGraph3
	@$(RTH_RUN) CMD="./processnew3Down4SgIncGraph3 $<" $(TEST_CONFIG) $@

.PHONY: check-processnew3Down4SgIncGraph3
check-processnew3Down4SgIncGraph3: $(processnew3Down4SgIncGraph3_TEST_TARGETS)

EXTRA_DIST += $(processnew3Down4SgIncGraph3_SPECIMENS)
TEST_TARGETS += $(processnew3Down4SgIncGraph3_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(processnew3Down4SgIncGraph3_SPECIMENS)) hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += strictGraphTest
strictGraphTest_SOURCES = strictGraphTest.C
strictGraphTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

strictGraphTest.passed: strictGraphTest $(TEST_CONFIG)
	@$(RTH_RUN) CMD="./strictGraphTest" $(TEST_CONFIG) $@

.PHONY: check-strictGraphTest
check-strictGraphTest: strictGraphTest.passed

TEST_TARGETS += strictGraphTest.passed
MOSTLYCLEANFILES += hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += strictGraphTest2
strictGraphTest2_SOURCES = strictGraphTest2.C
strictGraphTest2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

strictGraphTest2.passed: strictGraphTest2 $(TEST_CONFIG)
	@$(RTH_RUN) CMD="./strictGraphTest2" $(TEST_CONFIG) $@

.PHONY: check-strictGraphTest2
check-strictGraphTest2: strictGraphTest2.passed

TEST_TARGETS += strictGraphTest2.passed
MOSTLYCLEANFILES += hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += strictGraphTest3
strictGraphTest3_SOURCES = strictGraphTest3.C
strictGraphTest3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Not all targets have support for openmp, so it can't be unconditionally added to the compiler command line.  Also, why are
# we turning on optimizations especially when the user may have requested a different level of optimizations?  Besides,
# how do we even know that the compiler accepts "-fopenmp" and "-O3" as command-line switches?!
# Therefore, disabling these switches. [Robb Matzke, 2014-10-15]
#strictGraphTest3_LDFLAGS = -fopenmp -O3 

strictGraphTest3.passed: strictGraphTest3 $(TEST_CONFIG)
	@$(RTH_RUN) CMD="./strictGraphTest3" $(TEST_CONFIG) $@

.PHONY: check-strictGraphTest3
check-strictGraphTest3: strictGraphTest3.passed

TEST_TARGETS += strictGraphTest3.passed
MOSTLYCLEANFILES += hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += smtlibParser
smtlibParser_SOURCES = smtlibParser.C
smtlibParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
smtlibParser_SPECIMENS = test11.C
smtlibParser_TEST_TARGETS = $(addprefix smtlib_, $(addsuffix .passed, $(smtlibParser_SPECIMENS)))

$(smtlibParser_TEST_TARGETS): smtlib_%.passed: % $(TEST_CONFIG) smtlibParser
	@$(RTH_RUN) CMD="./smtlibParser $<" $(TEST_CONFIG) $@

.PHONY: check-smtlibParser
check-smtlibParser: $(smtlibParser_TEST_TARGETS)

EXTRA_DIST += $(smtlibParser_SPECIMENS)
TEST_TARGETS += $(smtlibParser_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(smtlibParser_SPECIMENS)) hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += sourcePTP
sourcePTP_SOURCES = sourcePTP.C
sourcePTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
sourcePTP_SPECIMENS = test14.C
sourcePTP_TEST_TARGETS = $(addprefix sptp_, $(addsuffix .passed, $(sourcePTP_SPECIMENS)))

$(sourcePTP_TEST_TARGETS): sptp_%.passed: % $(TEST_CONFIG) sourcePTP
	@$(RTH_RUN) CMD="./sourcePTP $<" $(TEST_CONFIG) $@

.PHONY: check-sourcePTP
check-sourcePTP: $(sourcePTP_TEST_TARGETS)

EXTRA_DIST += $(sourcePTP_SPECIMENS)
TEST_TARGETS += $(sourcePTP_TEST_TARGETS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(sourcePTP_SPECIMENS)) hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
binaryPaths_SPECIMENS = t1 t2 t3 t4
binaryPaths_TEST_TARGETS = $(addprefix bp_, $(addsuffix .passed, $(binaryPaths_SPECIMENS)))

$(binaryPaths_TEST_TARGETS): bp_%.passed: % $(TEST_CONFIG) binaryPaths
	@$(RTH_RUN) CMD="./binaryPaths $<" $(TEST_CONFIG) $@

.PHONY: check-binaryPaths
check-binaryPaths: $(binaryPaths_TEST_TARGETS)

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
noinst_PROGRAMS += binaryPaths
TEST_TARGETS += $(binaryPaths_TEST_TARGETS)
endif
EXTRA_DIST += $(binaryPaths_SPECIMENS)
MOSTLYCLEANFILES += analysis.dot hotness0.dot


#------------------------------------------------------------------------------------------------------------------------
bPTP_SOURCES = bPTP.C
bPTP_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
bPTP_SPECIMENS = t1 t2 t3 t4
bPTP_TEST_TARGETS = $(addprefix bptp_, $(addsuffix .passed, $(bPTP_SPECIMENS)))

$(bPTP_TEST_TARGETS): bptp_%.passed: % $(TEST_CONFIG) bPTP
	@$(RTH_RUN) CMD="./bPTP $<" $(TEST_CONFIG) $@

.PHONY: check-bPTP
check-bPTP: $(bPTP_TEST_TARGETS)

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
noinst_PROGRAMS += bPTP
TEST_TARGETS += $(bPTP_TEST_TARGETS)
endif
EXTRA_DIST += $(bPTP_SPECIMENS)
MOSTLYCLEANFILES += analysis.dot hotness0.dot

#------------------------------------------------------------------------------------------------------------------------
yicesParser_SOURCES = yicesParser.C
yicesParser_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
yicesParser_SPECIMENS = efor1.C efor2.C efor3.C eif.C eif2.C eif3.C mf1.C mf10.C mf11.C mf12.C mf13.C mf2.C mf3.C    \
	mf8.C mf9.C mfEx.C mfEx2.C mfEx3.C mfEx4.C mfEx6.C mfEx7.C mfEx8.C tf1.C tf2.C tf3.C tfif1.C tfif2.C tfif3.C \
	tfif4.C whiletest.C whiletest2.C whiletest3.C
yicesParser_TEST_TARGETS = $(addprefix yp_, $(addsuffix .passed, $(yicesParser_SPECIMENS)))

$(yicesParser_TEST_TARGETS): yp_%.passed: % $(TEST_CONFIG) yicesParser
	@$(RTH_RUN) CMD="./yicesParser $<" $(TEST_CONFIG) $@

.PHONY: check-yicesParser
check-yicesParser: $(yicesParser_TEST_TARGETS)

if ROSE_HAVE_YICES
noinst_PROGRAMS += yicesParser
TEST_TARGETS += $(yicesParser_TEST_TARGETS)
endif
EXTRA_DIST += $(yicesParser_SPECIMENS)
MOSTLYCLEANFILES += $(addsuffix .main.dot, $(yicesParser_SPECIMENS)) hotness0.dot pathsets yices.txt

########################################################################################################################
# Automake rules
########################################################################################################################

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)


