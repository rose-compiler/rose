## LNP: FIXME: Headers!
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

POCC_INSTALL = $(top_builddir)/src/3rdPartyLibraries/pocc

DEFS = 
#############################################################################
AM_CPPFLAGS = -I$(srcdir)/include	$(ROSE_INCLUDES)\
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

pkginclude_HEADERS 	=					\
	$(srcdir)/include/polyopt/PoCCOptimize.hpp		\
	$(srcdir)/include/polyopt/PolyoptModule.hpp		\
	$(srcdir)/include/polyopt/AffineConverter.hpp		\
	$(srcdir)/include/polyopt/ClastToSage.hpp		\
	$(srcdir)/include/polyopt/PastToSage.hpp		\
	$(srcdir)/include/polyopt/PolyDDV.hpp			\
	$(srcdir)/include/polyopt/PolyOpt.hpp			\
	$(srcdir)/include/polyopt/PolyRoseOptions.hpp		\
	$(srcdir)/include/polyopt/SageNodeAnnotation.hpp	\
	$(srcdir)/include/polyopt/SageTools.hpp			\
	$(srcdir)/include/polyopt/ScopExtractor.hpp		\
	$(srcdir)/include/polyopt/ScopExtractorVisitor.hpp	\
	$(srcdir)/include/polyopt/ScopParser.hpp

lib_LTLIBRARIES 	= libpolyopt.la


libpolyopt_la_SOURCES 	=			\
	PolyoptModule.cpp			\
	AffineConverter.cpp			\
	ClastToSage.cpp				\
	PastToSage.cpp				\
	PoCCOptimize.cpp			\
	PolyDDV.cpp				\
	PolyOpt.cpp				\
	PolyRoseOptions.cpp			\
	SageNodeAnnotation.cpp			\
	SageTools.cpp				\
	ScopExtractor.cpp			\
	ScopExtractorVisitor.cpp		\
	ScopParser.cpp

#libpolyopt_la_CPPFLAGS	=
#	-DHAVE_LIBPOCC_UTILS $(ROSE_INCLUDES) 
#
libpolyopt_la_LIBADD	=					\
	$(POCC_INSTALL)/driver/install-pocc/lib/libpocc.la

libpolyopt_la_LDFLAGS = $(ROSE_LIBS)

## LNP: FIXME: Remove this: Rose is deprecated.
CPPFLAGS += -Wno-deprecated
