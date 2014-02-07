
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST =
bin_PROGRAMS =
MOSTLYCLEANFILES =

# Tests
TEST_DIR = $(srcdir)
meta_SPECIMEN = $(TEST_DIR)/Meta1 \
		$(TEST_DIR)/Meta2 \
		$(TEST_DIR)/Meta3

test_SPECIMEN = $(TEST_DIR)/test1.cpp \
		$(TEST_DIR)/test2.cpp \
		$(TEST_DIR)/test3.cpp

EXTRA_DIST += $(meta_SPECIMEN) $(test_SPECIMEN)

TALC_EXE=$(top_builddir)/projects/TALCDataLayout/src/main/talc

talc.passed:
	for testno in 1 2 3 ; do \
	echo " Test No: $${testno} " ; \
	$(TALC_EXE) -rose:skipfinalCompileStep -I$(srcdir)/../src/runtime -extend --meta $(srcdir)/Meta$${testno} -module:aos $(srcdir)/test$${testno}.cpp ; \
	done

talc.fail:
	for testno in 1 2 ; do \
        echo " Test No: $${testno} " ; \
        $(TALC_EXE) -rose:skipfinalCompileStep -I$(srcdir)/../src/runtime -check -extend --meta $(srcdir)/MetaFail$${testno} -module:aos $(srcdir)/testFail$${testno}.cpp ; \
        done

check-local: talc.passed

clean-local:
	rm -rf rose_* *.ti *.dot 

