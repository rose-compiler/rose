## Process this file with automake to create Makefile.in.

.PHONY: docs
docs:
	$(MAKE) SATIRE_VERSION=$(VERSION) satire_manual.pdf -C docs

SUBDIRS= src tools examples
DISTCHECK_CONFIGURE_FLAGS=--with-rosedir=$(ROSEDIR) --with-boostdir=$(BOOSTDIR) CXX=$(CXX)

if WITH_PAG
DISTCHECK_CONFIGURE_FLAGS += --with-pagdir=$(PAGDIR)
else
DISTCHECK_CONFIGURE_FLAGS += --without-pag
endif

EXTRA_DIST= README LICENSE ChangeLog KNOWN_BUGS BugLog \
        docs/satire_cfgconstruction.pdf \
	docs/satire_overview.pdf \
	docs/satire_attributes_auxiliary_functions \
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
