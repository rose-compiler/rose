
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############




#original Makefile.am contents follow:

## This is just a makefile for testing acmacros.
## $Id: Makefile.am,v 1.2 2008/01/10 18:14:20 dquinlan Exp $
#
#ACLOCAL = @ACLOCAL@ -I .
#AUTOMAKE = @AUTOMAKE@ -a
#
#show: show-acsubst show-config
#
#
## Rule to show configure substitutions
#show-acsubst: Makefile
#	@echo "========== Configure substitutions ==========";	\
#	pattern=`echo $(SUBST_SHOW) | @SED@ 's/[ 	]\{1,\}/\|/g'`; \
#	: echo "pattern is '$$pattern'"; \
#	@EGREP@ "^($$pattern)[ 	]+=" Makefile | sort
#
#
## Rule to show configure macros
#show-config: config.h
#	@echo "========== config.h macros ==========";	\
#	pattern=`echo $(CONFIG_SHOW) | @SED@ 's/[ 	]\{1,\}/\|/g'`; \
#	: echo "pattern is '$$pattern'"; \
#	@EGREP@ "[[:space:]]*#(define|undef)[ 	]+($$pattern)" config.h
#
## Rule to force rerunning aclocal.
#acloc:
#	cd $(top_srcdir) && $(ACLOCAL)
#
## Rule to rerun preconf
#pre:
#	cd $(top_srcdir) && ./preconf
#
## Rule to reconfigure.
#reconf:
#	cd $(top_builddir) && ./config.status --recheck
#
## Remove .psrc links files.
#distclean-local:
#	$(btng_remove_auto_psrc_links)
#
#EXTRA_DIST =	\
#aux/.psrc.in t4/parallel-serial-runner.in	\
#arg-with-environment.m4	\
#check-typename.m4	\
#choose-mp-interface.m4	\
#compiler-ar.m4	\
#compiler-id.m4	\
#compiling-boolean.m4	\
#compiling-namespace.m4	\
#compiling-stl.m4	\
#configure-log.m4	\
#find-all-progs.m4	\
#link-to-source.m4	\
#specify-ar.m4	\
#support-blaslapack.m4	\
#support-dl.m4	\
#support-fortran.m4	\
#support-hdf5.m4	\
#support-hypre.m4	\
#support-ieee-float.m4	\
#support-insure.m4	\
#support-lam.m4	\
#support-libz.m4	\
#support-mpich.m4	\
#support-mpi.m4	\
#support-nsl.m4	\
#support-petsc.m4	\
#support-restrict.m4	\
#support-rpath.m4	\
#support-samrai.m4	\
#support-serpa-run.m4	\
#support-socket.m4	\
#variable-header-filenames.m4
#
