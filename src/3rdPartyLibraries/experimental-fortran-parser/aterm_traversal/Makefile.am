# include ../../make.inc
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SDF_GRAMMAR_DIR = $(srcdir)/../sdf_syntax

# CC  = cc
# CPC = c++

A_TERM_INCLUDE = -I$(ATERM_INSTALL_PATH)/include

# CFLAGS = -I. -I$(ATERM_ROOT)/include
CFLAGS = -I. -I$(A_TERM_INCLUDE)
# LIBS = -L$(ATERM_ROOT)/lib -lATerm
LIBS = -L$(ATERM_INSTALL_PATH)/lib -lATerm

# TRAVERSE_OBJS = UntypedNodes.o OFPNodes.o OFPExpr.o OFPUnparser.o UntypedASTBuilder.o traversal.o ofp_traverse_productions.o

INCLUDES = $(A_TERM_INCLUDE) $(ROSE_INCLUDES) $(SDF_GRAMMAR_DIR)

noinst_LTLIBRARIES = libexperimentalOFP.la
libexperimentalOFP_la_SOURCES = $(libexperimentalOFP_Sources)

# libexperimentalOFP_Sources =$(srcdir)/traversal.cpp $(srcdir)/OFPUnparser.cpp $(srcdir)/ofp_traverse_productions.cpp $(srcdir)/OFPExpr.cpp $(srcdir)/UntypedASTBuilder.cpp $(srcdir)/OFPNodes.cpp
libexperimentalOFP_Sources =$(srcdir)/traversal.cpp $(srcdir)/OFPUnparser.cpp $(srcdir)/ofp_traverse_productions.cpp $(srcdir)/OFPExpr.cpp $(srcdir)/OFPNodes.cpp

# bin_PROGRAMS = traverse
# traverse_SOURCES = $(srcdir)/traverse.cpp $(srcdir)/traversal.cpp $(srcdir)/OFPUnparser.cpp \
#                   $(srcdir)/ofp_traverse_productions.cpp $(srcdir)/OFPExpr.cpp \
#                   $(srcdir)/UntypedASTBuilder.cpp $(srcdir)/OFPNodes.cpp

EXTRA_DIST = traversal.hpp OFPNodes.hpp OFPExpr.hpp UntypedNodes.h OFPExpr.hpp

check: traverse
	$(SGLRI) -i testfile.f90 -p $(SDF_GRAMMAR_DIR)/Fortran.tbl | ../trans/ofp-simplify  -o testfile.f90.aterm
	traverse -i testfile.f90.aterm

clean:
	rm -f *.o *.lo *.la traverse
	rm -rf .libs
