EXTRA_DIST= \
	build-examples.sh \
	sl1rd/sl1rd.set sl1rd/sl1rd.optla \
	sl1rd/tests/rdtest1.C  \
	sl1rd/tests/rdtest2.C     sl1rd/tests/rdtest3.C  \
	                          sl2rd/tests/rdtest10.C \
        sl2rd/tests/rdtest2.C     sl2rd/tests/rdtest5.C  \
        sl2rd/tests/rdtest8a.C    sl2rd/tests/rdtest9a_c89.c \
	sl2rd/tests/rdtest11.C \
        sl2rd/tests/rdtest3.C     sl2rd/tests/rdtest6.C  \
	sl2rd/tests/rdtest8b.C    sl2rd/tests/rdtest0.C  \
        sl2rd/tests/rdtest12.C    sl2rd/tests/rdtest4.C  \
	sl2rd/tests/rdtest7.C     sl2rd/tests/rdtest8c.C \
        sl2rd/tests/rdtest9b.C    sl2rd/tests/rdtest1.C  \
        sl2rd/tests/rdtest4b.C    sl2rd/tests/rdtest9a.C \
        sl2rd/tests/rdtest10all.C sl2rd/tests/rdtest10p1.C \
        sl2rd/tests/rdtest10p2.C  sl2rd/tests/rdtest10p3.C \
        sl2rd/sl2rd.set sl2rd/sl2rd.optla sl2rd/runtests \
        constprop/constprop.optla \
	constprop/tests/cconstants.c  constprop/tests/classes_and_returns.C  \
	constprop/tests/demo2.C       constprop/tests/intval.c \
	constprop/tests/namespaces.C  constprop/tests/small.c \
	constprop/tests/typedefs.C    constprop/tests/ab.c \
        constprop/tests/classes.C     constprop/tests/deepreturn.c \
        constprop/tests/jump.c \
	constprop/tests/nested.c      constprop/tests/test1.C \
	constprop/tests/verysmall.c   constprop/tests/bigger.c \
	constprop/tests/classes2.C    constprop/tests/default_args.C \
        constprop/tests/destructor2.C constprop/tests/logical.c \
	constprop/tests/overload.C    constprop/tests/test5.C  \
	constprop/tests/virtual.C     constprop/tests/blocks.c \
	constprop/tests/classes3.C    constprop/tests/demo1.C \
	constprop/tests/forinit.c     constprop/tests/manyargs.c \
	constprop/tests/simpleif.c   constprop/tests/test6.C \
	\
	nnh99_shape/nnh99_shape.set      nnh99_shape/nnh99_shape.optla \
	nnh99_shape/main.C               nnh99_shape/main.h \
	nnh99_shape/Makefile.custom \
	nnh99_shape/GraphStats.h \
	nnh99_shape/satire_legacy.h      nnh99_shape/satire_legacy.C \
	nnh99_shape/tests/nnh_xsely.C \
	nnh99_shape/tests/srw98_listreverse.C \
	nnh99_shape/tests/nnh_xysel3.C \
	nnh99_shape/tests/nnh_xselnil.C \
	nnh99_shape/tests/nnh_xy.C \
	nnh99_shape/tests/nnh_xysel2.C \
	nnh99_shape/tests/srw98_data.h \
	nnh99_shape/tests/nnh_xnil.C \
	nnh99_shape/tests/nnh_xysel3shared.C \
	nnh99_shape/tests/srw98_listinsert.C \
	\
	srw98_shape/srw98_shape.set               srw98_shape/srw98_shape.optla \
	srw98_shape/main.C                        srw98_shape/main.h \
	srw98_shape/Makefile.custom \
	srw98_shape/GraphStats.h \
	srw98_shape/satire_legacy.h      srw98_shape/satire_legacy.C \
	srw98_shape/tests/nnh_xsely.C \
	srw98_shape/tests/srw98_listreverse.C \
	srw98_shape/tests/nnh_xysel3.C \
	srw98_shape/tests/nnh_xselnil.C \
	srw98_shape/tests/nnh_xy.C \
	srw98_shape/tests/nnh_xysel2.C \
	srw98_shape/tests/srw98_data.h \
	srw98_shape/tests/nnh_xnil.C \
	srw98_shape/tests/nnh_xysel3shared.C \
	srw98_shape/tests/srw98_listinsert.C \
	\
	pointsto_live_variables/pointsto_live_variables.optla \
	pointsto_live_variables/runtests.custom \
	pointsto_live_variables/tests/lv_noalias.c \
	pointsto_live_variables/tests/lv_singletonalias.c \
	pointsto_live_variables/tests/lv_nosingletonalias.c

testdir=installcheck-executable-examples

# The tests are now split into separate targets to facilitate a
# paralellized run of the testsuite (make -j #N installcheck)

TESTS_WITH_PAG = sl1rd sl2rd nnh99_shape srw98_shape pointsto_live_variables testsuite
TESTS_WITHOUT_PAG = termite

.PHONY: tests $(TESTS_WITH_PAG) $(TESTS_WITHOUT_PAG)

installcheck-local: 
	rm -f init.token
	$(MAKE) tests
	@echo "Cleanup..."
	chmod -R u+w $(testdir)
	rm -rf $(testdir)
	rm -f init.token


if WITH_PAG
tests: $(TESTS_WITH_PAG) $(TESTS_WITHOUT_PAG)
else
tests: $(TESTS_WITHOUT_PAG)
	@echo
	@echo "========================================================================"
	@echo "The following tests were not run because SATIrE was built without PAG:"
	@echo "$(TESTS_WITH_PAG)"
	@echo "========================================================================"
endif

init.token:
	@echo "Init..."
	rm -rf $(testdir)
	mkdir -p $(testdir)
	touch init.token

%.termite: %.c
	@echo "TERMITE: performing a translation circle of" $<
	cp $< $(testdir)/TEST0.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term TEST0.c  TEST1.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.pl TEST2.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term TEST2.c  TEST3.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.pl TEST4.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term --strip-headers TEST0.c TEST1.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.pl TEST2.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term --strip-headers TEST2.c TEST3.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.pl TEST4.c
# the freshly copied TEST0.c has the permissions of the original file, which
# is read-only in distcheck mode
	chmod u+w $(testdir)/TEST0.c

%.termite: %.C
	@echo "TERMITE: performing a translation circle of" $<
	cp $< $(testdir)/TEST0.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term TEST0.C  TEST1.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.pl TEST2.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term TEST2.C  TEST3.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.pl TEST4.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term --strip-headers TEST0.C TEST1.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.pl TEST2.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term --strip-headers TEST2.C TEST3.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.pl TEST4.C
# the freshly copied TEST0.c has the permissions of the original file, which
# is read-only in distcheck mode
	chmod u+w $(testdir)/TEST0.c


termite: init.token
	@echo
	@echo "========================================================================"
	@echo "TERMITE tests"
	@echo "========================================================================"
#	cp $(top_srcdir)/examples/*/tests/* $(testdir)
	@echo "TERMITE language C:"
	$(MAKE) -j1 $(patsubst %.c,%.termite,$(wildcard $(top_srcdir)/examples/*/tests/*.c))
	@echo "TERMITE language C++:"

# known fails...
#$(MAKE) -j1 $(patsubst %.C,%.termite,$(wildcard $(top_srcdir)/examples/*/tests/*.C))

sl1rd: init.token
	@echo
	@echo "Testing sl1rd"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/sl1rd $(testdir)
	chmod u+rwx -R $(testdir)/sl1rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u sl1rd
	$(MAKE) check -C $(testdir)/sl1rd

sl2rd: init.token
	@echo
	@echo "Testing sl2rd"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/sl2rd $(testdir)
	chmod u+rwx -R $(testdir)/sl2rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u sl2rd
	$(MAKE) check -C $(testdir)/sl2rd

nnh99_shape: init.token
	@echo
	@echo "Testing nnh99_shape"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/nnh99_shape $(testdir)
	chmod u+rwx -R $(testdir)/nnh99_shape
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u nnh99_shape
	$(MAKE) check -C $(testdir)/nnh99_shape

srw98_shape: init.token
	@echo
	@echo "Testing srw98_shape"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/srw98_shape $(testdir)
	chmod u+rwx -R $(testdir)/srw98_shape
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u srw98_shape
	$(MAKE) check -C $(testdir)/srw98_shape

pointsto_live_variables: init.token
	@echo
	@echo "Testing pointsto_live_variables"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/pointsto_live_variables $(testdir)
	chmod u+rwx -R $(testdir)/pointsto_live_variables
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u pointsto_live_variables
	$(MAKE) check -C $(testdir)/pointsto_live_variables

testsuite: init.token
	@echo
	@echo "Testsuite..."
	@echo "====================="

	cp -ur $(top_srcdir)/testsuite $(testdir)
	chmod u+rwx -R $(testdir)/testsuite

	cp -ur $(top_srcdir)/examples/constprop $(testdir)
	chmod u+rwx -R $(testdir)/constprop
	##GB working around possible problems with old constprop.set files
	@if [ -e $(testdir)/constprop/constprop.set ] ; then \
		echo "info: $(top_srcdir)/examples/constprop/constprop.set no longer in CVS, ignoring your local copy" ; \
		rm -f $(testdir)/constprop/constprop.set ; \
	fi
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis constprop
	$(MAKE) -C $(testdir)/constprop
	cd $(testdir) ; sh testsuite/testsuite.sh constprop/constprop testsuite

	##cp -ur $(top_srcdir)/examples/srw98_shape $(testdir)
	##chmod u+rwx -R $(testdir)/srw98_shape
	##cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u srw98_shape
	##$(MAKE) -C $(testdir)/srw98_shape
	##cd $(testdir) ; sh testsuite/testsuite.sh srw98_shape/srw98_shape testsuite

	##cp -ur $(top_srcdir)/examples/nnh99_shape $(testdir)
	##chmod u+rwx -R $(testdir)/nnh99_shape
	##cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u nnh99_shape
	##$(MAKE) -C $(testdir)/nnh99_shape
	##cd $(testdir) ; sh testsuite/testsuite.sh nnh99_shape/nnh99_shape testsuite 

