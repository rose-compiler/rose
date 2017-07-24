#
# Makefile.am: this file is part of the PACE/PolyOpt project.
#
# Author:
# Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
#


#############################################################################
SUBDIRS 		=

#############################################################################
MAINTAINERCLEANFILES    = Makefile.in

TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

EXTRA_DIST              = scripts/checker.sh
#############################################################################
check_SCRIPTS		=			\
	scripts/checker_passthru.sh		

# check_SCRIPTS		=			\
# 	scripts/checker_passthru.sh		\
# 	scripts/checker_plutotile.sh		\
# 	scripts/checker_plutotilepar.sh		\
# 	scripts/checker_plutotileparunroll.sh	\
# 	scripts/checker_ptpup.sh


TEST_SUITE		=			\
	$(top_srcdir)/tests/unitary/classicloop.c	\
	$(top_srcdir)/tests/unitary/simpleloop.c	\
	$(top_srcdir)/tests/unitary/basicnest.c		\
	$(top_srcdir)/tests/unitary/empty.c		\
	$(top_srcdir)/tests/unitary/simplecond.c	\
	$(top_srcdir)/tests/unitary/twoconds.c		\
	$(top_srcdir)/tests/unitary/twocondincond.c	\
	$(top_srcdir)/tests/unitary/complexcond.c	\
	$(top_srcdir)/tests/unitary/multiparenthcond.c	\
	$(top_srcdir)/tests/unitary/maxloopsimple.c	\
	$(top_srcdir)/tests/unitary/maxloopcomplex.c	\
	$(top_srcdir)/tests/unitary/minloopsimple.c	\
	$(top_srcdir)/tests/unitary/minloopcomplex.c	\
	$(top_srcdir)/tests/unitary/minmax.c		\
	$(top_srcdir)/tests/unitary/negassign.c		\
	$(top_srcdir)/tests/unitary/binop.c		\
	$(top_srcdir)/tests/unitary/complexop.c		\
	$(top_srcdir)/tests/unitary/complexprog.c	\
	$(top_srcdir)/tests/unitary/noloop.c		\
	$(top_srcdir)/tests/unitary/noloop2.c		\
	$(top_srcdir)/tests/unitary/ternary.c		\
	$(top_srcdir)/tests/unitary/functioncall.c	\
	$(top_srcdir)/tests/unitary/functioncall2.c	\
	$(top_srcdir)/tests/unitary/redefiter.c		\
	$(top_srcdir)/tests/unitary/comments.c		\
	$(top_srcdir)/tests/unitary/underscore.c	\
	$(top_srcdir)/tests/unitary/parenthesis.c	\
	$(top_srcdir)/tests/unitary/intaddit.c		\
	$(top_srcdir)/tests/unitary/assigniter.c	\
	$(top_srcdir)/tests/unitary/assigniter2.c	\
	$(top_srcdir)/tests/unitary/increment.c		\
	$(top_srcdir)/tests/unitary/field.c		\
	$(top_srcdir)/tests/unitary/object.c		\
	$(top_srcdir)/tests/unitary/float.c

TEST_SUITE_REF_PASSTHRU    	= $(TEST_SUITE:.c=.passthru.c)
TEST_SUITE_REF_PLUTOTILE    	= $(TEST_SUITE:.c=.plutotile.c)
TEST_SUITE_REF_PLUTOTILEPAR    	= $(TEST_SUITE:.c=.plutotilepar.c)
TEST_SUITE_REF_PLUTOTILE_MF    	= $(TEST_SUITE:.c=.plutotilemf.c)
TEST_SUITE_REF_PLUTOTILEPAR_MF	= $(TEST_SUITE:.c=.plutotileparmf.c)
TEST_SUITE_REF_PLUTOTILEPARUNR 	= $(TEST_SUITE:.c=.plutotileparunroll.c)
TEST_SUITE_REF_PTPUP	 	= $(TEST_SUITE:.c=.plutotileparunrollprev.c)

TEST_SUITE_CLEAN_PASSTHRU    	= $(TEST_SUITE:.c=.passthru.test.c)
TEST_SUITE_CLEAN_PLUTOTILE    	= $(TEST_SUITE:.c=.plutotile.test.c)
TEST_SUITE_CLEAN_PLUTOTILEPAR   = $(TEST_SUITE:.c=.plutotilepar.test.c)
TEST_SUITE_CLEAN_PLUTOTILE_MF   = $(TEST_SUITE:.c=.plutotilemf.test.c)
TEST_SUITE_CLEAN_PLUTOTILEPAR_MF= $(TEST_SUITE:.c=.plutotileparmf.test.c)
TEST_SUITE_CLEAN_PLUTOTILEPARUNR= $(TEST_SUITE:.c=.plutotileparunroll.test.c)
TEST_SUITE_CLEAN_PTPUP		= $(TEST_SUITE:.c=.plutotileparunrollprev.test.c)

TEST_SUITE_CLEAN		=		\
	$(TEST_SUITE_CLEAN_PASSTHRU)		\
	$(TEST_SUITE_CLEAN_PLUTOTILE)		\
	$(TEST_SUITE_CLEAN_PLUTOTILEPAR)	\
	$(TEST_SUITE_CLEAN_PLUTOTILE_MF)	\
	$(TEST_SUITE_CLEAN_PLUTOTILEPAR_MF)	\
	$(TEST_SUITE_CLEAN_PLUTOTILEPARUNR)	\
	$(TEST_SUITE_CLEAN_PTPUP)


## Replace by program names when needed
# check_PROGRAMS = ...
TESTS				= $(check_SCRIPTS)

CLEANFILES			= $(TEST_SUITE_CLEAN)

EXTRA_DIST	+=				\
	$(TEST_SUITE)				\
	$(check_SCRIPTS)

TESTS_ENVIRONMENT +=							\
	CHECKER="$(top_srcdir)/tests/scripts/checker.sh"		\
	TEST_SUITE="$(TEST_SUITE)"
#############################################################################
