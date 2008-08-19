include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

genFileNames:
	sh -c '\
		for f in `find $(srcdir) -maxdepth 1 -type f -name "test_*"` ;\
		do \
			ln -f -s $${f} `basename $${f}` ;\
		done ;\
	'

check-local: genFileNames
	@echo "################################################################################"
	@echo "#########Testing Case Insensitive Files"
	@echo "################################################################################"
	$(srcdir)/test_filename_suffixes.pl
	@echo "********************************************************************************"
	@echo "$(srcdir): make check rule complete (terminated normally)"
	@echo "********************************************************************************"

clean-local:
	rm -f test_* a*.out rose_test_*

EXTRA_DIST = test_filename_suffixes.pl clean_generated_files \
        test_simple_file_suffix_test_01.c \
        test_simple_file_suffix_test_02.c++ \
        test_simple_file_suffix_test_05.cc \
        test_simple_file_suffix_test_07.cp \
        test_simple_file_suffix_test_09.cpp \
        test_simple_file_suffix_test_11.cxx \
        test_simple_file_suffix_test_13.i \
        test_simple_file_suffix_test_15.ii \
        test_simple_file_suffix_test_17.h \
        test_simple_file_suffix_test_19.hh 
