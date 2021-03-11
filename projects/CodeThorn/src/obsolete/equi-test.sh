check-equivalence:
	@rm -f tmp.nsdump
	@echo ================================================================
	@echo RUNNING UPDATE SEQUENCE VERIFICATION TESTS
	@echo ================================================================
	@./equithorn --edg:no_warnings --normalize-all=off $(srcdir)/tests/jacobi-1d-imper_mod.c --dump-non-sorted=tmp.nsdump --rule-commutative-sort=no --abstraction-mode=0
#	@diff tmp.nsdump $(srcdir)/tests/jacobi-1d-imper_mod.c.nsdump
	@cat tmp.nsdump
	@rm -f tmp.nsdump
