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
	\
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
	pointsto_live_variables/pointsto_live_variables.optla \
	pointsto_live_variables/runtests.custom \
	pointsto_live_variables/tests/lv_noalias.c \
	pointsto_live_variables/tests/lv_singletonalias.c \
	pointsto_live_variables/tests/lv_nosingletonalias.c


testdir=installcheck-executable-examples

# The tests are now split into separate targets to facilitate a
# paralellized run of the testsuite (make -j #N installcheck)

TESTS_WITH_PAG = sl1rd sl2rd pointsto_live_variables testsuite interval satire_driver
TESTS_WITHOUT_PAG = termite

if WITH_SWI_PROLOG
TESTS_WITH_PAG += itvl_stresstest loopbounds
endif


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
	@echo "================================================================"
	@echo "The following tests were not run because SATIrE was built without PAG:"
	@echo "$(TESTS_WITH_PAG)"
	@echo "================================================================"
endif

init.token:
	@echo "Init..."
	rm -rf $(testdir)
	mkdir -p $(testdir)
	touch init.token

%.termite: %.c
	@echo "TERMITE: performing a translation circle of" $<
	cp $< $(testdir)/TEST0.c
# the freshly copied TEST0.c has the permissions of the original file, which
# is read-only in distcheck mode
	chmod u+w $(testdir)/TEST0.c
# some test cases need header files...
	find `dirname $<` -name '*.h' -exec cp '{}' $(testdir) ';'
	find $(testdir) -name '*.h' -exec chmod u+w '{}' ';'

	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term -I$(shell dirname $<) TEST0.c -o TEST1.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.term -o TEST2.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term -I$(shell dirname $<) TEST2.c -o TEST3.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.term -o TEST4.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/termite_lint TEST1.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/termite_lint TEST3.term

%.termite: %.C
	@echo "TERMITE: performing a translation circle of" $<
	cp $< $(testdir)/TEST0.C
	chmod u+w $(testdir)/TEST0.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term -I$(shell dirname $<) TEST0.C -o TEST1.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST1.term -o TEST2.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/c2term -I$(shell dirname $<) TEST2.C -o TEST3.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2c TEST3.term -o TEST4.C
	cd $(testdir) ; $(DESTDIR)/$(bindir)/termite_lint TEST1.term
	cd $(testdir) ; $(DESTDIR)/$(bindir)/termite_lint TEST3.term

termite: init.token
	@echo
	@echo "================================================================"
	@echo "TERMITE tests"
	@echo "================================================================"
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
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -p sl1rd_ -u sl1rd
	$(MAKE) check -C $(testdir)/sl1rd

sl2rd: init.token
	@echo
	@echo "Testing sl2rd"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/sl2rd $(testdir)
	chmod u+rwx -R $(testdir)/sl2rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -p sl2rd_ -u sl2rd
	$(MAKE) check -C $(testdir)/sl2rd

pointsto_live_variables: init.token
	@echo
	@echo "Testing pointsto_live_variables"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/pointsto_live_variables $(testdir)
	chmod u+rwx -R $(testdir)/pointsto_live_variables
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -p ptlv_ -u pointsto_live_variables
	$(MAKE) check -C $(testdir)/pointsto_live_variables

%.interval.run: %.c
	@echo "Testing the correctness of the interval analysis on" $<
	@cp $< $(testdir)/interval/intvltest.c
	find `dirname $<` -name '*.h' -exec cp '{}' $(testdir)/interval ';'
	@chmod u+w $(testdir)/interval/intvltest.c
	@cd $(testdir)/interval; ./interval --run-pointsto-analysis --callstringlength=2 --output-term=intvltest.term intvltest.c
	@echo "testing termite -> icfgbuilder..."
	@cd $(testdir)/interval; ./interval --run-pointsto-analysis --output-pointsto-graph=`basename $< .c`_pointsto --callstringlength=2 --input-termite-ast=intvltest.term
	@echo "generating assertions..."
	@cd $(testdir)/interval; export TERMITE_LIB=$(datadir)/termite; $(SWI_PL) -q -O -t main -s validator.pl < intvltest.term > intvltest1.term
	@cd $(testdir)/interval; $(DESTDIR)/$(bindir)/termite_lint intvltest1.term; if [ $$? -ne 0 ]; then echo "*** termite_lint was not quite satisfied with that term!"; fi
	@cd $(testdir)/interval; $(DESTDIR)/$(bindir)/term2c intvltest1.term -o intvlassert.c
	@echo "vvv from $< vvv"
	@cd $(testdir)/interval; cat intvlassert.c
	@echo "^^^ from $< ^^^"
	@cd $(testdir)/interval; \
	($(CC) -std=c99 -DINCLUDE_ASSERT_H -o intvlassert intvlassert.c \
	 && bash -c \
   "echo 'executing with 3sec timeout...' && ulimit -t 3 && ./intvlassert" \
	 && echo "... success") \
	|| (echo "** FAILED: $<" && echo && true)
if WITH_CBMC
	@echo "Performing model checking on" $<
	@cd $(testdir)/interval; \
	$(CBMC) --show-claims intvlassert.c &>cbmc-output-claims.log; \
	ulimit -t 30; ($(CBMC) --no-bounds-check --unwind 1024 intvlassert.c \
	2>cbmc-output.log \
	| grep -q "^VERIFICATION SUCCESSFUL$$") \
	|| (echo "** VERIFICATION FAILED: $<" && echo && true)
endif
#($(CC) -std=c99 -E -I$(BLASTHOME)/test/headers intvlassert.c > intvlassert.i;
#$(BLAST) -clp intvlassert.i -main main)


interval: init.token
	@echo
	@echo "Testing interval"
	@echo "====================="
	cp -ur $(top_srcdir)/tools/interval $(testdir)
	chmod u+rwx -R $(testdir)/interval
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -p intvl_ interval
	$(MAKE) check -C $(testdir)/interval
# cd $(testdir) ; bash testsuite/testsuite.sh interval/interval testsuite
if WITH_SWI_PROLOG
	@echo "Correctness tests..."
	$(MAKE) -j1 $(patsubst %.c,%.interval.run, \
           $(wildcard $(top_srcdir)/examples/*/tests/*.c \
		              $(top_srcdir)/tools/interval/pto_tests/*.c))
endif

itvl_stresstest: interval
	@echo
	@echo "Testing interval (the strict way)"
	@echo "====================="
	@cp -ur $(top_srcdir)/tools/interval/stresstest/itvl_stresstest.c $(testdir)/interval
	@cd $(testdir)/interval; ./interval --run-pointsto-analysis --resolve-funcptr-calls --output-term=itvl_stresstest.term itvl_stresstest.c > itvl_stresstest.out 2>&1
	@cd $(testdir)/interval; cat itvl_stresstest.out
	@cd $(testdir)/interval; if [ "x`tail -1 itvl_stresstest.out`" != x"performing analysis interval ... done" ]; then \
		echo -n "* treating any analyzer messages as errors"; \
		echo    "(told you this was strict)"; \
		exit 1; \
	fi
	@cd $(testdir)/interval; $(DESTDIR)/$(bindir)/termite_lint itvl_stresstest.term
	@cd $(testdir)/interval; export TERMITE_LIB=$(datadir)/termite; $(SWI_PL) -q -O -t main -s validator.pl < itvl_stresstest.term > itvl_stresstest_assert.term
	@cd $(testdir)/interval; $(DESTDIR)/$(bindir)/term2c itvl_stresstest_assert.term -o itvl_stresstest_assert.c
	@cd $(testdir)/interval; $(CC) -std=c99 -DINCLUDE_ASSERT_H -o itvl_stresstest_assert itvl_stresstest_assert.c
	@cd $(testdir)/interval; ./itvl_stresstest_assert

%.loop.term: %.c
	@echo "Analyzing loops in" $<
	@cp $< $(testdir)/loopbounds/looptest.c
	@chmod u+w $(testdir)/loopbounds/looptest.c
	cd $(testdir)/loopbounds ; $(bindir)/loopbounds looptest.c

loopbounds: interval
	@echo
	@echo "Testing loopbounds"
	@echo "====================="
	cp -ur $(top_srcdir)/tools/loopbounds $(testdir)
	chmod +w $(testdir)/loopbounds
	chmod u+rwx -R $(testdir)/loopbounds
	$(MAKE) -j1 $(patsubst %.c,%.loop.term,$(wildcard $(top_srcdir)/testsuite/mtrc/success/*.c))
	@echo "Sanity check"
	$(bindir)/loopbounds $(top_srcdir)/testsuite/base/success/bsort.c \
	  |unparse | grep wcet_constraint

satire_driver: init.token
	@echo
	@echo "Testing satire_driver with analysis interval"
	@echo "============================================"
	rm -rf $(testdir)/driver_tests
	cp -ur $(top_srcdir)/tools/interval/tests $(testdir)/driver_tests
	cd $(testdir)
	for f in $(testdir)/driver_tests/*; do \
		rm -rf driver_out; \
		$(bindir)/satire_driver --analysis=interval --run-pointsto-analysis --callstringlength=2 --output-term=driver_out/interval/`basename $$f .c`.term $$f; \
		echo "testing existence of term file driver_out/interval/`basename $$f .c`.term ..."; \
		test -s driver_out/interval/`basename $$f .c`.term || exit 1; \
		wc driver_out/interval/`basename $$f .c`.term; \
		echo; \
	done
	rm -rf driver_out

testsuite: init.token
	@echo
	@echo "Testsuite..."
	@echo "====================="

	cp -ur $(top_srcdir)/testsuite $(testdir)
	chmod u+rwx -R $(testdir)/testsuite

	cp -ur $(top_srcdir)/examples/constprop $(testdir)
	chmod u+rwx -R $(testdir)/constprop
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -p cp_ constprop
	$(MAKE) -C $(testdir)/constprop
	cd $(testdir) ; bash testsuite/testsuite.sh constprop/constprop testsuite
