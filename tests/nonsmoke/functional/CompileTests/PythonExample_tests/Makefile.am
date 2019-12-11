include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################
# These are Python testcodes (C++ using Python.h) #
###################################################

# Shorten the list of code to compile to simplify checks using "make distcheck"
TESTCODES_REQUIRED_TO_PASS = \
     test2004_92.C

EXTRA_TESTCODES_REQUIRED_TO_PASS = \
     test2004_92.C \
     test2005_18.C \
     test2005_54.C

TESTCODE_CURRENTLY_FAILING = 

# DQ (5/26/2013): List these files to avoid the "unused sources policy" error.
HEADER_FILES = \
   abstract.h      cellobject.h   cobject.h       complexobject.h  dictobject.h  fileobject.h   funcobject.h    import.h \
   iterobject.h    longobject.h   methodobject.h  node.h           objimpl.h     parsetok.h     pyconfig.h      pyerrors.h \
   pymactoolbox.h  pystate.h      pythread.h      stringobject.h   symtable.h    traceback.h    unicodeobject.h \
   bitset.h        ceval.h        codecs.h        cStringIO.h      errcode.h     floatobject.h  graminit.h      intobject.h \
   listobject.h    marshal.h      modsupport.h    Numeric          opcode.h      patchlevel.h   py_curses.h     pyfpe.h \
   pymem.h         Python.h       rangeobject.h   structmember.h   sysmodule.h   tupleobject.h  weakrefobject.h \
   bufferobject.h  classobject.h  compile.h       descrobject.h    eval.h        frameobject.h  grammar.h \
   intrcheck.h     longintrepr.h  metagrammar.h   moduleobject.h   object.h      osdefs.h       pgenheaders.h   pydebug.h \
   pygetopt.h      pyport.h       pythonrun.h     sliceobject.h    structseq.h   token.h        ucnhash.h

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w --edg:restrict

../../testTranslator:
	make -C ../.. testTranslator

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../testTranslator
	@echo "Calling TEST_Objects rule: "
	../../testTranslator $(ROSE_FLAGS) -I$(srcdir)/pythonDir -c $(srcdir)/$(@:.o=.C)

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/pythonDir -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES) pythonDir

check-local:
	@echo "Tests for Python examples (tests python header files as used by SWIG)."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/PythonExample_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

