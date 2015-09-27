include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  MultiLevelMemory
# ------------------------------------------------------------------------------

DEFS =
POCC_INSTALL = $(top_builddir)/src/3rdPartyLibraries/pocc

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)/../../PolyOpt2/polyopt/include \
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

bin_PROGRAMS = mlm

mlm_SOURCES=\
	mlm.cpp\
	mlmapi.h\
	main.cpp

mlm_LDADD= $(top_builddir)/projects/PolyOpt2/polyopt/libpolyopt.la $(ROSE_LIBS)
include_HEADERS=
