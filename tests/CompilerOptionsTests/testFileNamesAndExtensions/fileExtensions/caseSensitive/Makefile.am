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
	@echo "#########Testing Case Sensitive Files"
	@echo "################################################################################"
	$(srcdir)/test_filename_suffixes.pl
	@echo "********************************************************************************"
	@echo "$(srcdir): make check rule complete (terminated normally)"
	@echo "********************************************************************************"

clean-local:
	rm -f test_* a*.out rose_test_*

EXTRA_DIST = clean_generated_files test_filename_suffixes.pl \
        test_simple_file_suffix_test_01.c \
        test_simple_file_suffix_test_02.c++ \
        test_simple_file_suffix_test_03.C \
        test_simple_file_suffix_test_04.C++ \
        test_simple_file_suffix_test_05.cc \
        test_simple_file_suffix_test_06.CC \
        test_simple_file_suffix_test_07.cp \
        test_simple_file_suffix_test_08.CP \
        test_simple_file_suffix_test_09.cpp \
        test_simple_file_suffix_test_10.CPP \
        test_simple_file_suffix_test_11.cxx \
        test_simple_file_suffix_test_12.CXX \
        test_simple_file_suffix_test_13.i \
        test_simple_file_suffix_test_14.I \
        test_simple_file_suffix_test_15.ii \
        test_simple_file_suffix_test_16.II \
        test_simple_file_suffix_test_17.h \
        test_simple_file_suffix_test_18.H \
        test_simple_file_suffix_test_19.hh \
        test_simple_file_suffix_test_20.HH
