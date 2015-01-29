## LNP: FIXME: Insert headers!
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

POCC_INSTALL = $(top_builddir)/src/3rdPartyLibraries/pocc

DEFS = 

#############################################################################
INCLUDES = -I$(srcdir)/include	$(ROSE_INCLUDES)\
	-I$(srcdir)							\
	-I$(srcdir)/../polyopt/include					\
	-I$(POCC_INSTALL)/driver/pocc/include				\
	-I$(POCC_INSTALL)/driver/pocc-utils/include			\
	-I$(POCC_INSTALL)/optimizers/install-letsee/include		\
	-I$(POCC_INSTALL)/optimizers/install-pluto/include		\
	-I$(POCC_INSTALL)/optimizers/install-storcompacter/include	\
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
	-I$(POCC_INSTALL)/generators/install-clastannotation/include	\
	-I$(POCC_INSTALL)/generators/install-clasttools/include		\
	-I$(POCC_INSTALL)/generators/install-ptile/include		\
	-I$(POCC_INSTALL)/math/install-piplib-hybrid/include		\
	-I$(POCC_INSTALL)/math/install-piplib/include			\
	-I$(POCC_INSTALL)/math/install-polylib/include			\
	-I$(POCC_INSTALL)/math/install-fm/include			\
	-I$(POCC_INSTALL)/math/install-isl/include			\
	-I$(POCC_INSTALL)/math/external/install/include			

#############################################################################

bin_PROGRAMS		= PolyOpt


PolyOpt_SOURCES	=			\
	Main.cpp

#PolyOpt_CFLAGS		=					\
#	-DPOCC_ROOT_DIR=$(abs_top_srcdir)/pocc $(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS)

PolyOpt_CPPFLAGS	= -Wno-deprecated

PolyOpt_LDADD		= $(builddir)/../polyopt/libpolyopt.la $(ROSE_LIBS)

#############################################################################
