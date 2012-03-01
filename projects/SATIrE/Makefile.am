## Process this file with automake to create Makefile.in.

bin_SCRIPTS = $(builddir)/satire-libtool

$(builddir)/satire-libtool: $(builddir)/libtool
	cp $< $@

ACLOCAL_AMFLAGS = -I m4

SUBDIRS= src tools examples #docs

DIST_SUBDIRS = src tools examples #docs

DISTCHECK_CONFIGURE_FLAGS=--with-rosedir=$(ROSEDIR) --with-boostdir=$(BOOSTDIR) --with-boost-compiler-string=$(BOOST_COMPILER_STRING) CXX=$(CXX)

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

# clang connection is currently not maintained and may be discontinued
clang: $(top_srcdir)/src/termite/clang/clang-termite.patch $(top_builddir)/clang/build/Makefile
	$(MAKE) -C $(top_builddir)/clang/build
	$(MAKE) install -C $(top_builddir)/clang/build

$(top_builddir)/clang/build/Makefile:
	cd $(top_builddir)/src/termite/clang && sh BUILD-clang.sh

EXTRA_DIST= README COPYING ChangeLog KNOWN_BUGS BugLog \
	\
	testsuite/testsuite.sh \
	testsuite/base/success/ab.c \
	testsuite/base/success/bigger.c \
	testsuite/base/success/blocks.c \
	testsuite/base/success/cconstants.c \
	testsuite/base/success/classes.C \
	testsuite/base/success/classes3.C \
	testsuite/base/success/classes_and_returns.C \
	testsuite/base/success/deepreturn.c \
	testsuite/base/success/default_args.C \
	testsuite/base/success/demo1.C \
	testsuite/base/success/demo2.C \
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

# MS 2012: This list intentionally circumvents the ROSE policy /scripts/policies/UnusedSources.pl
# most of these files are test files and iterated on by a test script, but
# not installed. There is no (good) reason to mention those in a Makefile

dummytarget:
	ls -l./projects/SATIrE/examples/constprop/tests/destructor.C
	ls -l./projects/SATIrE/examples/constprop/tests/initializers/initializer1.C
	ls -l./projects/SATIrE/examples/constprop/tests/initializers/initializer2.C
	ls -l./projects/SATIrE/examples/constprop/tests/initializers/initializer3.C
	ls -l./projects/SATIrE/examples/constprop/tests/initializers/initializer4.C
	ls -l./projects/SATIrE/examples/constprop/tests/struct2.c
	ls -l./projects/SATIrE/examples/sl2rd/tests/rdtest13.C
	ls -l./projects/SATIrE/examples/sl2rd/tests/rdtest14.C
	ls -l./projects/SATIrE/examples/sl2rd/tests/rdtest15.C
	ls -l./projects/SATIrE/src/analyzer/icfgbuilder/obsolete_CFGFixUp.C
	ls -l./projects/SATIrE/src/analyzer/icfgbuilder/satire/obsolete_CFGFixUp.h
	ls -l./projects/SATIrE/src/analyzer/provided/aslan/aslan.cpp
	ls -l./projects/SATIrE/src/aral/src/tests/compress.c
	ls -l./projects/SATIrE/src/aral/src/tests/demo.c
	ls -l./projects/SATIrE/src/aral/src/tests/fft1.c
	ls -l./projects/SATIrE/src/aral/src/tests/fib_for.c
	ls -l./projects/SATIrE/src/aral/src/tests/fibcall.c
	ls -l./projects/SATIrE/src/aral/src/tests/recursion2.c
	ls -l./projects/SATIrE/src/aral/src/tests/shorttest1.c
	ls -l./projects/SATIrE/src/experimental/specs/nielson/main-support.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/main.h
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/artificialListCreation.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/artificialListCreationCyclic.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/listTraversal1.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xnil.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xselnil.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xsely.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xy.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xysel2.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/nielson_xysel3.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm1.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm1_direct.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm1x.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm2.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm2x.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm3.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/norm4.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/normalization_test1.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/normalization_test2.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/reverse1.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/reverse2.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/srw98create.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/srw98insert.C
	ls -l./projects/SATIrE/src/experimental/specs/nielson/tests/srw98reverse.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/main-support.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/main.h
	ls -l./projects/SATIrE/src/experimental/specs/pred/tests/graphtest1.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/tests/graphtest2.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/tests/graphtest3.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/tests/graphtest4.C
	ls -l./projects/SATIrE/src/experimental/specs/pred/tests/subtypetest.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/main-support.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/main.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOdemo1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOsrw98insert1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest1a.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest1a.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest1b.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest1b.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest2.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest2b.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest2b.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest3.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest4.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest4.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest5.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest5.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6b.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6b.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6c.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6c.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6d.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/OOtest6d.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/artificialListCreation.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/artificialListCreationCyclic.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/bug1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/heap4.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/listTraversal1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/listdescruction_test1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xnil.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xselnil.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xsely.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xy.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xysel2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xysel3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/nielson_xysel3shared.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/norm1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/norm2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/norm2x.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/normalization_test1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/normalization_test2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/printList.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/printList.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/problem1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/reverse1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/reverse2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98_example_xselsel_null.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98_example_xselsel_null2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98create.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98create_ctor.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98insert.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98reverse.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98reverse2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/srw98reverse3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack4.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack5.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack6.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/stack_example_localvarlist3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/test2.h
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/vivutest1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/wcettests/bubble.c
	ls -l./projects/SATIrE/src/experimental/specs/shape/tests/wcettests/wcet.h
	ls -l./projects/SATIrE/src/experimental/specs/shape2/AliasStatistics.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/PagAttributes.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/ShapeAnalysisDriver.h
	ls -l./projects/SATIrE/src/experimental/specs/shape2/ShapeAnalyzerOptions.h
	ls -l./projects/SATIrE/src/experimental/specs/shape2/ShapeCommandLineParser.h
	ls -l./projects/SATIrE/src/experimental/specs/shape2/ShapeImplementation.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/nnh99_main.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/satire_legacy.h
	ls -l./projects/SATIrE/src/experimental/specs/shape2/shapetool_main.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/alias_twoIntoNamedNode.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/alias_twoIntoSummaryNode.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/artificialListCreation.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/artificialListCreationCyclic.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/artificialSumWithDS.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/001-artificialListCreation.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/005-artificialSumWithDS.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/005-artificialSumWithDS_unrolled.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/007-srw98create.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/008-srw98insert.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/009-srw98reverse.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/100-independent-lists.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/110-semi-independent-lists.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/120-append-if.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/200-common-tail.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/400-RecSplice.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/500-crosstalk.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/501-crosstalk_if.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/benchmark/510-pathjoin.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/conversion_3x_shared.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/garbage_loop.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/BinarySortedTree.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/CyclicNode.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/CyclicSingleLinkedList.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/DSW.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/DoubleLinkedList.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/javaexamples/SingleLinkedList.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/listTraversal1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/materialisation_3x_internal.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/materialisation_3x_internal_2x_goingin.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xnil.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xselnil.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xsely.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xy.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xysel2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/nielson_xysel3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm1_direct.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm1x.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm2x.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/norm4.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/normalization_test1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/normalization_test2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/reverse1.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/reverse2.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/100_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/100_create_recur.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/200_delall_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/200_delall_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/200_delall_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/200_delall_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/300_insert_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/300_insert_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/300_insert_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/300_insert_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/400_remove_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/400_remove_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/400_remove_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/400_remove_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/500_search_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/500_search_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/500_search_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/500_search_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/600_append_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/600_append_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/600_append_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/600_append_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/700_merge_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/700_merge_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/700_merge_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/700_merge_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/800_reverse_iter_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/800_reverse_iter_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/800_reverse_recur_create_fixed.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/sas2010/800_reverse_recur_create_iter.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/srw98create.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/srw98insert.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/srw98reverse.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/tests/srw98reverse_unrolled3.C
	ls -l./projects/SATIrE/src/experimental/specs/shape2/visualisation.c
	ls -l./projects/SATIrE/src/experimental/specs/shape2/visualisation.h
	ls -l./projects/SATIrE/src/experimental/specs/typeexpr/tests/ints.C
	ls -l./projects/SATIrE/src/experimental/specs/typeexpr/tests/ints.c
	ls -l./projects/SATIrE/src/experimental/specs/typeexpr/tests/ints2.C
	ls -l./projects/SATIrE/src/experimental/specs/typeexpr/tests/subtypetest.C
	ls -l./projects/SATIrE/src/termite/pdfgen.C
	ls -l./projects/SATIrE/testsuite/base/failure/destructor.C
	ls -l./projects/SATIrE/testsuite/base/success/bound_t.c
	ls -l./projects/SATIrE/testsuite/mrtc/failure/des.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/adpcm.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/bs.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/bsort100.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/compress.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/cover.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/crc.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/duff.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/edn.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/expint.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/fac.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/fdct.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/fft1.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/fibcall.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/fir.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/insertsort.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/janne_complex.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/jfdctint.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/lcdnum.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/lms.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/loop3.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/ludcmp.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/minver.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/ndes.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/ns.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/nsichneu.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/qsort-exam.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/qurt.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/recursion.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/select.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/sqrt.c
	ls -l./projects/SATIrE/testsuite/mrtc/success/statemate.c
	ls -l./projects/SATIrE/testsuite/shape/success/artificalsum.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xnil.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xselnil.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xsely.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xy.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xysel2.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xysel3.C
	ls -l./projects/SATIrE/testsuite/shape/success/nnh_xysel3shared.C
	ls -l./projects/SATIrE/testsuite/shape/success/pathjoin.C
	ls -l./projects/SATIrE/testsuite/shape/success/srw98_data.h
	ls -l./projects/SATIrE/testsuite/shape/success/srw98_listreverse.C
	ls -l./projects/SATIrE/tools/interval/tests/pointerloop.c
	ls -l./projects/SATIrE/tools/interval/tests/sum.c
# end of list of unlisted SATIrE test files
