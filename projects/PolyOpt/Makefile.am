#
# Makefile.am: this file is part of the PACE project.
# <INSERT-LICENSE>
#
# Author:
# Louis-Noel Pouchet <Louis-Noel.Pouchet@inria.fr>
#

#############################################################################
SUBDIRS			=			\
	benchmarks				\
	doc					\
	polyopt					\
	src					\
	tests



#############################################################################
ACLOCAL_AMFLAGS		= -I config

CONFIGURE_DEPENDENCIES	=			\
	$(srcdir)/config/ax_boost.m4		\
	$(srcdir)/config/ax_boost_base.m4 \
	$(srcdir)/config/ax_pocc.m4

m4datadir		= $(datadir)/aclocal

AUX_DIST								=			\
	$(ac_aux_dir)/config.guess		\
	$(ac_aux_dir)/config.sub		\
	$(ac_aux_dir)/install-sh		\
	$(ac_aux_dir)/ltmain.sh			\
	$(ac_aux_dir)/missing			\
	$(ac_aux_dir)/depcomp


EXTRA_DIST		=				\
	README


MAINTAINERCLEANFILES	=			\
	Makefile.in				\
	aclocal.m4				\
	configure				\
	polyopt/include/polyopt/Config-hpp.in	\
	$(AUX_DIST)

dist-hook:
	(cd $(distdir) && mkdir -p $(ac_aux_dir))
	for file in $(AUX_DIST); do \
		cp $$file $(distdir)/$$file; \
	done

build-pocc:
	(cd pocc && bin/pocc-util buildall)

update-pocc:
	(cd pocc && svn up && ./install.sh)

update-polyrose: update-pocc
	(svn up && make)

sanity-check:
	(cd tests/sanity && sh run-sanity-check.sh)


#############################################################################
