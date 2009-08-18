## Process this file with automake to create Makefile.in.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS= src tools examples docs
DISTCHECK_CONFIGURE_FLAGS=--with-rosedir=$(ROSEDIR) --with-boostdir=$(BOOSTDIR) CXX=$(CXX)

if WITH_PAG
DISTCHECK_CONFIGURE_FLAGS += --with-pagdir=$(PAGDIR)
else
DISTCHECK_CONFIGURE_FLAGS += --without-pag
endif

if !WITH_SWI_PROLOG
DISTCHECK_CONFIGURE_FLAGS += --without-swi-prolog
endif

.PHONY: docs clang

docs:
	$(MAKE) -C docs docs

clang: $(top_srcdir)/src/termite/clang/clang-termite.patch $(top_builddir)/clang/build/Makefile
	$(MAKE) -C $(top_builddir)/clang/build
	$(MAKE) install -C $(top_builddir)/clang/build

$(top_builddir)/clang/build/Makefile:
	cd $(top_builddir)/src/termite/clang && sh BUILD-clang.sh

EXTRA_DIST= README LICENSE ChangeLog KNOWN_BUGS BugLog \
        docs/satire_cfgconstruction.pdf \
	docs/satire_overview.pdf \
	docs/satire_attributes_auxiliary_functions \
	docs/termite.pdf \
	\
	testsuite/testsuite.sh \
	testsuite/base/success/ab.c \
	testsuite/base/success/bigger.c \
	testsuite/base/success/blocks.c \
	testsuite/base/success/cconstants.c \
	testsuite/base/success/classes.C \
	testsuite/base/success/classes2.C \
	testsuite/base/success/classes_and_returns.C \
	testsuite/base/success/deepreturn.c \
	testsuite/base/success/default_args.C \
	testsuite/base/success/demo1.C \
	testsuite/base/success/demo2.C \
	testsuite/base/success/destructor.C \
	testsuite/base/success/destructor2.C \
	testsuite/base/success/destructor_simple.C \
	testsuite/base/success/forinit.c \
	testsuite/base/success/for_return1.c \
	testsuite/base/success/intval.c \
	testsuite/base/success/jump.c \
	testsuite/base/success/logical.c \
	testsuite/base/success/manyargs.c \
	testsuite/base/success/namespaces.C \
	testsuite/base/success/nested.c \
	testsuite/base/success/overload.C \
	testsuite/base/success/simpleif.c \
	testsuite/base/success/small.c \
	testsuite/base/success/test1.C \
	testsuite/base/success/test5.C \
	testsuite/base/success/test6.C \
	testsuite/base/success/typedefs.C \
	testsuite/base/success/verysmall.c \
	testsuite/base/success/virtual.C \
	testsuite/base/success/while_call1.c
