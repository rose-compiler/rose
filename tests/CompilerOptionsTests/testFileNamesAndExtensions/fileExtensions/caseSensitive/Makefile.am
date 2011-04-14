include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

check-local:
	@echo "################################################################################"
	@echo "#########Testing Case Sensitive Files"
	@echo "################################################################################"
if OS_VENDOR_UBUNTU
#	Note that test #19 for $(MAKE) testFilenames will sometime hang for unknow reasons.
	@echo "##################################################################################"
	@echo "Testing use of filenames skipped for Ubuntu until we can investigate failing test!"
	@echo "##################################################################################"
else
	@echo "Found a NON-UBUNTU system (running tests...)"
	$(srcdir)/test_filename_suffixes.pl
endif
	@echo "********************************************************************************"
	@echo "$(srcdir): make check rule complete (terminated normally)"
	@echo "********************************************************************************"

clean-local:
	rm -f test_simple_file* a*.out rose_test_*

EXTRA_DIST = test_filename_suffixes.pl
