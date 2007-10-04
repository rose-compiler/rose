dist_pkgdata_DATA= \
	sl1rd/sl1rd.set sl1rd/sl1rd.optla sl1rd/runtests \
	sl1rd/tests/rderrtest1.C sl1rd/tests/rdtest1.C      sl1rd/tests/rdtest2.C    sl1rd/tests/rdtest3.C  \
	sl2rd/tests/error1.c     sl2rd/tests/rdtest10.C     sl2rd/tests/rdtest2.C    sl2rd/tests/rdtest5.C  \
        sl2rd/tests/rdtest8a.C   sl2rd/tests/rdtest9a_c89.c sl2rd/tests/rderrtest1.C sl2rd/tests/rdtest11.C \
        sl2rd/tests/rdtest3.C    sl2rd/tests/rdtest6.C      sl2rd/tests/rdtest8b.C   sl2rd/tests/rdtest0.C  \
        sl2rd/tests/rdtest12.C   sl2rd/tests/rdtest4.C      sl2rd/tests/rdtest7.C    sl2rd/tests/rdtest8c.C \
        sl2rd/tests/rdtest9b.C   sl2rd/tests/rdtest1.C      sl2rd/tests/rdtest4b.C   sl2rd/tests/rdtest7.ps \
        sl2rd/tests/rdtest9a.C   \
        sl2rd/sl2rd.set sl2rd/sl2rd.optla sl2rd/runtests \
        constprop/constprop.set constprop/constprop.optla constprop/runtests    \
	constprop/tests/cconstants.c  constprop/tests/classes_and_returns.C  constprop/tests/demo2.C \
        constprop/tests/intval.c      constprop/tests/namespaces.C  constprop/tests/small.c  constprop/tests/typedefs.C\
	constprop/tests/ab.c      constprop/tests/classes.C  constprop/tests/deepreturn.c constprop/tests/destructor.C \
	constprop/tests/jump.c    constprop/tests/nested.c   constprop/tests/test1.C  constprop/tests/verysmall.c \
        constprop/tests/bigger.c  constprop/tests/classes2.C constprop/tests/default_args.C \
        constprop/tests/destructor2.C  constprop/tests/logical.c   constprop/tests/overload.C \
        constprop/tests/test5.C  constprop/tests/virtual.C constprop/tests/blocks.c  constprop/tests/classes3.C \
        constprop/tests/demo1.C  constprop/tests/forinit.c constprop/tests/manyargs.c constprop/tests/simpleif.c \
        constprop/tests/test6.C



testdir=executable-examples

installcheck-local:
	mkdir $(testdir)
	echo "Testing sl1rd"
	cp -ur $(top_srcdir)/examples/sl1rd $(testdir)
	chmod u+rwx -R $(testdir)/sl1rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis sl1rd
	$(MAKE) check -C $(testdir)/sl1rd

	echo "Testing sl2rd"
	cp -ur $(top_srcdir)/examples/sl2rd $(testdir)
	chmod u+rwx -R $(testdir)/sl2rd
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis sl2rd
	$(MAKE) check -C $(testdir)/sl2rd

	echo "Testing constprop"
	cp -ur $(top_srcdir)/examples/constprop $(testdir)
	chmod u+rwx -R $(testdir)/constprop
	cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis constprop
	$(MAKE) check -C $(testdir)/constprop
#	rm -rf $(testdir)
