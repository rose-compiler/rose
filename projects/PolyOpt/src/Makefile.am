## LNP: FIXME: Insert headers!

#############################################################################
SUBDIRS 		=

#############################################################################
MAINTAINERCLEANFILES	= Makefile.in

POCC_INSTALL = @pocc_install_prefix@

AM_CPPFLAGS		=						\
	-I$(top_builddir)						\
	-I$(top_builddir)/polyopt/include				\
	-I$(top_builddir)/src						\
	-I$(POCC_INSTALL)/driver/pocc/include				\
	-I$(POCC_INSTALL)/driver/pocc-utils/include			\
	-I$(POCC_INSTALL)/optimizers/install-letsee/include		\
	-I$(POCC_INSTALL)/optimizers/install-pluto/include		\
	-I$(POCC_INSTALL)/optimizers/install-vectorizer/include		\
	-I$(POCC_INSTALL)/ir/install-scoplib/include			\
	-I$(POCC_INSTALL)/ir/install-scoptools/include			\
	-I$(POCC_INSTALL)/ir/install-past/include			\
	-I$(POCC_INSTALL)/ir/install-irconverter/include		\
	-I$(POCC_INSTALL)/analyzers/install-clan/include		\
	-I$(POCC_INSTALL)/analyzers/install-candl/include		\
	-I$(POCC_INSTALL)/analyzers/install-polyfeat/include		\
	-I$(POCC_INSTALL)/generators/install-cloog/include		\
	-I$(POCC_INSTALL)/generators/install-pragmatize/include		\
	-I$(POCC_INSTALL)/generators/install-clasttools/include		\
	-I$(POCC_INSTALL)/generators/install-ptile/include		\
	-I$(POCC_INSTALL)/math/external/install/include			\
	-I$(POCC_INSTALL)/math/install-piplib-hybrid/include		\
	-I$(POCC_INSTALL)/math/install-piplib/include			\
	-I$(POCC_INSTALL)/math/install-polylib/include			\
	-I$(POCC_INSTALL)/math/install-fm/include			\
	-I$(top_srcdir)							\
	-I$(top_srcdir)/polyopt/include					\
	-I$(top_srcdir)/src						\
	-I$(top_srcdir)/pocc/driver/pocc/include			\
	-I$(top_srcdir)/pocc/driver/pocc-utils/include			\
	-I$(top_srcdir)/pocc/optimizers/install-letsee/include		\
	-I$(top_srcdir)/pocc/optimizers/install-pluto/include		\
	-I$(top_srcdir)/pocc/optimizers/install-vectorizer/include	\
	-I$(top_srcdir)/pocc/ir/install-scoplib/include			\
	-I$(top_srcdir)/pocc/ir/install-scoptools/include		\
	-I$(top_srcdir)/pocc/ir/install-past/include			\
	-I$(top_srcdir)/pocc/ir/install-irconverter/include		\
	-I$(top_srcdir)/pocc/analyzers/install-clan/include		\
	-I$(top_srcdir)/pocc/analyzers/install-candl/include		\
	-I$(top_srcdir)/pocc/analyzers/install-polyfeat/include		\
	-I$(top_srcdir)/pocc/generators/install-cloog/include		\
	-I$(top_srcdir)/pocc/generators/install-pragmatize/include	\
	-I$(top_srcdir)/pocc/generators/install-clasttools/include	\
	-I$(top_srcdir)/pocc/generators/install-ptile/include		\
	-I$(top_srcdir)/pocc/math/external/install/include		\
	-I$(top_srcdir)/pocc/math/install-piplib-hybrid/include		\
	-I$(top_srcdir)/pocc/math/install-piplib/include		\
	-I$(top_srcdir)/pocc/math/install-polylib/include		\
	-I$(top_srcdir)/pocc/math/install-fm/include			\
	$(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

#############################################################################

EXTRA_DIST		 =

#############################################################################

noinst_HEADERS		=

bin_PROGRAMS		= PolyRose


PolyRose_SOURCES	=			\
	Main.cpp

PolyRose_CFLAGS		=					\
	-DPOCC_ROOT_DIR=$(abs_top_srcdir)/pocc $(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

PolyRose_CPPFLAGS	= -Wno-deprecated

PolyRose_LDADD		= $(top_builddir)/polyopt/libpolyopt.la $(ROSE_ROOT)/lib/librose.la

#############################################################################
