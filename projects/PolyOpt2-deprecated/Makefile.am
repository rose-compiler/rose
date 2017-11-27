include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=\
	polyopt\
	src

POCC_INSTALL = $(top_builddir)/src/3rdPartyLibraries/pocc

#############################################################################
ACLOCAL_AMFLAGS		= -I config

CONFIGURE_DEPENDENCIES	=			\
	$(srcdir)/config/ax_boost.m4		\
	$(srcdir)/config/ax_boost_base.m4	\
	$(srcdir)/config/ax_pocc.m4

m4datadir		= $(datadir)/aclocal

AUX_DIST		=			\
	$(ac_aux_dir)/config.guess		\
	$(ac_aux_dir)/config.sub		\
	$(ac_aux_dir)/install-sh		\
	$(ac_aux_dir)/ltmain.sh			\
	$(ac_aux_dir)/missing			\
	$(ac_aux_dir)/depcomp

check-local:
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/polyopt2: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
