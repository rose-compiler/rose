include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EmptyFileNames = \
	test_empty_cplusplus_file.C \
	test_empty_c_file.c

FileNames = \
	test_simple_file.C \
	"test simple file with spaces in name .C" \
	'test_simple_file\\\"\\\'.C' \
	'test_simple_\\\'file.C' \
	'test_simple_file_dollar\\\$$.C' \
	'test_simple_\\\*.C' \
	'test_simple_file_\\\ \#1.C' \
	test_simple_file_ending_with_dot. \
	test_simple_file_no_suffix

genFileNames:
	@echo -e "\nGenerating test files\n" && sh -c '\
		for f in $(EmptyFileNames) ;\
		do \
			touch ./"$${f}" ;\
		done ;\
	'

check-local: genFileNames
	@echo "################################################################################"
	@echo "#########Testing use of UNIX Meta Characters"
	@echo "################################################################################"
	$(srcdir)/test_filenames_unix_meta_chars.pl
	@echo "********************************************************************************"
	@echo "$(builddir): make check rule complete (terminated normally)"
	@echo "********************************************************************************"

clean-local:
	@echo "################################################################################"
	@echo "##########Cleaning $(builddir)"
	@echo "################################################################################"
	@sh -c '\
		for f in $(FileNames) $(EmptyFileNames);\
		do \
			rm -f ./"$${f}" ;\
		done ;\
	'
	rm -f rose_* a*.out *.o

EXTRA_DIST = test_filenames_unix_meta_chars.pl
