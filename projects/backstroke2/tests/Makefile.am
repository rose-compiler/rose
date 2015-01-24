
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST =
bin_PROGRAMS =
MOSTLYCLEANFILES =

# Tests
TEST_DIR = $(srcdir)

test_SPECIMEN = $(TEST_DIR)/test1.cpp \
                $(TEST_DIR)/test2.cpp

EXTRA_DIST += $(test_SPECIMEN)

BACKSTROKE2_EXE=$(top_builddir)/projects/backstroke2/src/backstroke2

backstroke2.passed:
	for testno in 1 2; do \
	echo " Test No: $${testno} " ; \
	$(BACKSTROKE2_EXE) -rose:skipfinalCompileStep -I$(srcdir)/../src/runtime $(srcdir)/test$${testno}.cpp ; \
	done

backstroke2.fail:
	for testno in 1 2 ; do \
        echo " Test No: $${testno} " ; \
        $(BACKSTROKE2_EXE) -rose:skipfinalCompileStep -I$(srcdir)/../src/runtime -check $(srcdir)/testFail$${testno}.cpp ; \
        done

check-local: backstroke2.passed

clean-local:
	rm -rf rose_* *.ti *.dot 
