files = omp.incl opt.pi opt.pt utils.incl ExpStmt.incl Loops.incl Cfront.code Ffront.code C2F.code F2C.code TranslatorDriver.pt F95front.code PAPI_C.code LoopBlocking.pt ReplExp.pt ArrayCopying.pt BlockingCleanup.pt OptHighLevel.pt peephole_opt.pt pthread.code LoopParallel.pt auto_tune.code analysis.pi analysis.pt normalize.pt

EXTRA_DIST = $(files)

install-data-hook:
	test -e $(DESTDIR)$(libdir) || mkdir $(DESTDIR)$(libdir);
	for each in $(files); do cp $(srcdir)/$${each} $(DESTDIR)$(libdir); done

uninstall-hook:
	for each in $(files); do chmod 644 $(DESTDIR)$(libdir)/$${each}; rm $(DESTDIR)$(libdir)/$${each}; done

include ../Make.inc
