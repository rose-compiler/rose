include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ATERM_ROOT = /usr/local/stratego-debug-32
# ATERM_ROOT = /home/dquinlan/bin/aterm-2.5-install-64bit
ATERM_ROOT = $(ATERM_INSTALL_PATH)
ATERM_INCLUDE = -I$(ATERM_ROOT)/include
AM_CPPFLAGS = -I$(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/rose_traverse $(ROSE_INCLUDES) $(ATERM_INCLUDE)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# libmstlSources = PDFGeneration.C DOTGraphInterface.C

# noinst_LTLIBRARIES = libofp.la

# DQ (9/10/2014): DISABLE THIS LIBRARY
# lib_LTLIBRARIES = libofp.la
# libofp_la_SOURCES = OFPNodes.C OFPExpr.C OFPUnparser.cpp UntypedASTBuilder.cpp traversal.cpp ofp_traverse_productions.C

SDF_GRAMMAR_DIR = $(srcdir)/../syntax

CC  = cc
CPC = c++

## -m32 flag used in aterm build on Mac OS X
#
OSX_ARCH = -m32
OSX_ERROR_LIMIT = -ferror-limit=5

ARCH =
ERROR_LIMIT =

# CFLAGS = -g $(ARCH) -I. -I$(ATERM_ROOT)/include $(ERROR_LIMIT)
# CFLAGS = -g $(ARCH) -I. $(ROSE_INCLUDES) $(ATERM_INCLUDE) $(ERROR_LIMIT)
CFLAGS = -g $(ARCH) -I. $(ROSE_INCLUDES) -I$(ATERM_ROOT)/include $(ERROR_LIMIT)
LIBS = -L$(ATERM_ROOT)/lib -lATerm

OBJS = ofp_builder.o

# TRAVERSE_OBJS = UntypedNodes.o OFPNodes.o OFPExpr.o OFPUnparser.o UntypedASTBuilder.o traversal.o ofp_traverse_productions.o
TRAVERSE_OBJS = OFPNodes.o OFPExpr.o OFPUnparser.o UntypedASTBuilder.o traversal.o ofp_traverse_productions.o

EXES = build_classes build_includes build_traversals \
       traverse traverse_sig check_cons_name

%.o: %.cpp
	@echo 'Compiling file: $<'
	@$(CPC) -c $(CFLAGS) -o $@ $<

%.o: %.C
	@echo 'Compiling file: $<'
	@$(CPC) -c $(CFLAGS) -o $@ $<

%.o: %.c
	@echo 'Compiling file: $<'
	@$(CC) -c $(CFLAGS) -o $@ $<

# all: $(OBJS) traverse
all: $(OBJS)

traverse: traverse.o $(TRAVERSE_OBJS)
#	$(CPC) -o $@ $(ARCH) $< $(TRAVERSE_OBJS) $(LIBS)

traverse_sig: traverse_sig.o traversal_f.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traversal_f.o $(OBJS) $(LIBS)

build_includes: build_includes.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

build_classes: build_classes.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

build_traversals: build_traversals.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

build_list_traversals: build_list_traversals.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

build_productions: build_productions.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

check_cons_name: check_cons_name.o traverse_sig.o $(OBJS)
	$(CC) -o $@ $(ARCH) $< traverse_sig.o $(OBJS) $(LIBS)

support.o: support.c support.h
traversal.o: traversal.cpp traversal.h OFPNodes.h OFPExpr.h UntypedNodes.h ofp_traverse_productions.o OFPExpr.o \
             OFPUnparser.hpp OFPUnparser.cpp
ofp_traverse_productions.o: ofp_traverse_productions.C traversal.h OFPNodes.h UntypedNodes.h
OFPExpr.o: OFPExpr.C OFPExpr.h UntypedNodes.h
OFPUnparser.o: OFPUnparser.cpp OFPUnparser.hpp
traversal_f.o: traversal_f.c traversal.h

# DQ (3/8/2014): We can't have a executable be built as part of compiling the source tree.
# check: traverse
#	sglri -i testfile.f90 -p $(SDF_GRAMMAR_DIR)/Fortran.tbl -o testfile.f90.aterm
#	traverse testfile.f90.aterm

clean:
	rm -f *.o *.lo *.la $(EXES) $(OBJS)
	rm -rf .libs
