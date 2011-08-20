include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

check-local:
	@echo "Test for DataFaultTolerance transformation"
	@rm -rf *.out
	@./$(top_srcdir)/projects/DataFaultTolerance/test/array/transformation/run.sh $(top_srcdir)/projects/DataFaultTolerance $(top_builddir)/projects/DataFaultTolerance
	@echo "*********************************************************************************************************************************"
	@echo "****** ROSE/projects/DataFaultTolerance/array/transformation/tests: make check rule complete (terminated normally) **************"
	@echo "*********************************************************************************************************************************"

EXTRA_DIST = 

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.gif *.log

	


