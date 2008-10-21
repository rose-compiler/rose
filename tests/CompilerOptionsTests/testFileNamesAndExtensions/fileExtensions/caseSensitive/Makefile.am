include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

check-local:
	@echo "################################################################################"
	@echo "#########Testing Case Sensitive Files"
	@echo "################################################################################"
	$(srcdir)/test_filename_suffixes.pl
	@echo "********************************************************************************"
	@echo "$(srcdir): make check rule complete (terminated normally)"
	@echo "********************************************************************************"

clean-local:
	rm -f test_simple_file* a*.out rose_test_*

EXTRA_DIST = test_filename_suffixes.pl
