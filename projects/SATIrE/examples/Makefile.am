EXTRA_DIST= \
	build-examples.sh \
	sl1rd/sl1rd.set sl1rd/sl1rd.optla sl1rd/runtests \
	sl1rd/tests/rderrtest1.C  sl1rd/tests/rdtest1.C  \
	sl1rd/tests/rdtest2.C     sl1rd/tests/rdtest3.C  \
	sl2rd/tests/error1.c      sl2rd/tests/rdtest10.C \
        sl2rd/tests/rdtest2.C     sl2rd/tests/rdtest5.C  \
        sl2rd/tests/rdtest8a.C    sl2rd/tests/rdtest9a_c89.c \
	sl2rd/tests/rderrtest1.C  sl2rd/tests/rdtest11.C \
        sl2rd/tests/rdtest3.C     sl2rd/tests/rdtest6.C  \
	sl2rd/tests/rdtest8b.C    sl2rd/tests/rdtest0.C  \
        sl2rd/tests/rdtest12.C    sl2rd/tests/rdtest4.C  \
	sl2rd/tests/rdtest7.C     sl2rd/tests/rdtest8c.C \
        sl2rd/tests/rdtest9b.C    sl2rd/tests/rdtest1.C  \
        sl2rd/tests/rdtest4b.C    sl2rd/tests/rdtest9a.C \
        sl2rd/tests/rdtest10all.C sl2rd/tests/rdtest10p1.C \
        sl2rd/tests/rdtest10p2.C  sl2rd/tests/rdtest10p3.C \
        sl2rd/sl2rd.set sl2rd/sl2rd.optla sl2rd/runtests \
        constprop/constprop.set constprop/constprop.optla \
	constprop/tests/cconstants.c  constprop/tests/classes_and_returns.C  \
	constprop/tests/demo2.C       constprop/tests/intval.c \
	constprop/tests/namespaces.C  constprop/tests/small.c \
	constprop/tests/typedefs.C    constprop/tests/ab.c \
        constprop/tests/classes.C     constprop/tests/deepreturn.c 
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
	srw98_shape/tests/nnh_xsely.C \
	srw98_shape/tests/srw98_listreverse.C \
	srw98_shape/tests/nnh_xysel3.C \
	srw98_shape/tests/nnh_xselnil.C \
	srw98_shape/tests/nnh_xy.C \
	srw98_shape/tests/nnh_xysel2.C \
	srw98_shape/tests/srw98_data.h \
	srw98_shape/tests/nnh_xnil.C \
	srw98_shape/tests/nnh_xysel3shared.C \
	srw98_shape/tests/srw98_listinsert.C 

testdir=installcheck-executable-examples

installcheck-local:
	rm -rf $(testdir)
	mkdir -p $(testdir)

	@echo
	@echo "========================================================================"
	@echo "TERMITE tests"
	@echo "========================================================================"

	cp $(top_srcdir)/examples/constprop/tests/bigger.c $(testdir)/TEST0.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/rose2term TEST0.c  TEST1.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2rose TEST1.pl TEST2.c
	cd $(testdir) ; $(DESTDIR)/$(bindir)/rose2term TEST2.c  TEST3.pl
	cd $(testdir) ; $(DESTDIR)/$(bindir)/term2rose TEST3.pl TEST4.c

	@echo "========================================================================"
	@echo "PAG tests"
	@echo "========================================================================"

	@echo 
	@echo "Testing sl1rd"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/sl1rd $(testdir)
	chmod u+rwx -R $(testdir)/sl1rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u sl1rd
	$(MAKE) check -C $(testdir)/sl1rd

	@echo
	@echo "Testing sl2rd"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/sl2rd $(testdir)
	chmod u+rwx -R $(testdir)/sl2rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u sl2rd
	$(MAKE) check -C $(testdir)/sl2rd

	@echo
	@echo "Testing nnh99_shape"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/nnh99_shape $(testdir)
	chmod u+rwx -R $(testdir)/nnh99_shape
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u nnh99_shape
	$(MAKE) check -C $(testdir)/nnh99_shape

	@echo
	@echo "Testing srw98_shape"
	@echo "====================="
	cp -ur $(top_srcdir)/examples/srw98_shape $(testdir)
	chmod u+rwx -R $(testdir)/srw98_shape
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis -u srw98_shape
	$(MAKE) check -C $(testdir)/srw98_shape

	@echo
	@echo "Testsuite..."
	@echo "====================="

	cp -ur $(top_srcdir)/testsuite $(testdir)
	chmod u+rwx -R $(testdir)/testsuite

	cp -ur $(top_srcdir)/examples/constprop $(testdir)
	chmod u+rwx -R $(testdir)/constprop
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

	chmod -R u+w $(testdir)
	rm -rf $(testdir)
