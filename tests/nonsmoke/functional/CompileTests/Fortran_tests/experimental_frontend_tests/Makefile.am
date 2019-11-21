include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -rose:experimental_fortran_frontend -I$(srcdir)

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

F90_TESTCODES = \
 tiny.f90   \
 R1101a.f90 \
 R1101b.f90 \
 R1101c.f90 \
 R1101d.f90 \
 R1101e.f90 \
 R1101f.f90 \
 R1101g.f90 \
 R1120a.f90 \
 R404_a.f90 \
 R521.f90   \
 R545.f90   \
 R611a.f90  \
 R719a.f90  \
 R832.f90   \
 R837.f90   \
 R854.f90   \
 R856.f90   \
 contiguous_attr.f90 \
 test2014_02.f90

F2018_TESTCODES = \
 R507-F2015-N2007.f90 \
 R511-F2015-N2007.f90 \
 R1123-F2018.f90 \
 R1164-F2018.f90 \
 R1166-F2018.f90 \
 R1168-F2018.f90 \
 R1169-F2018.f90 \
 R1179-F2018.f90 \
 R1181-F2018.f90

F90_TESTCODES_WORKING_ON = \
 issue-38.f90 \
 R404.f90   \
 R1001.f90 \
 R1109.f90 \
 R1111.f90 \
 R1120b.f90 \
 R1201.f90 \
 R1203.f90 \
 R1204.f90 \
 R1205.f90 \
 R1206.f90 \
 R1207.f90 \
 R1208.f90 \
 R1209.f90 \
 R1210.f90 \
 R1211.f90 \
 R1214.f90 \
 R1218.f90 \
 R1234.f90 \
 R1241.f90 \
 R524.f90 \
 R526.f90 \
 R533.f90 \
 R611.f90 \
 R620.f90 \
 R626.f90 \
 R640.f90 \
 R701.f90 \
 R704.f90 \
 R705.f90 \
 R706.f90 \
 R710.f90 \
 R713.f90 \
 R719.f90 \
 R720.f90 \
 R721.f90 \
 R743.f90 \
 R757.f90 \
 R804.f90 \
 R807.f90 \
 R831.f90 \
 R838.f90 \
 R850.f90 \
 R851.f90 \
 R852.f90 \
 R853.f90 \
 R902.f90 \
 R904.f90 \
 R905.f90 \
 R908.f90 \
 R909.f90 \
 R911.f90 \
 R922.f90 \
 R926.f90 \
 R927.f90 \
 R928.f90 \
 R929.f90 \
 R930.f90

# moved R816.f90 from working to failing (should be looked into) [Rasmussen, 2019-01-24]

FAILING_TESTS = \
   pause_stmt.f77 \
   R432.f90 \
   R452.f90 \
   R816.f90 \
   R858.f08 \
   R859.f08 \
   R862.f08

NON_FORTRAN_FAILING_TESTS = \
   R1116.fxx \
   R1118.fxx \
   R1220.fxx \
   R1237.fxx \
   R426.fxx \
   R427.fxx \
   R702.fxx \
   R722.fxx \
   R723.fxx \
   R733.fxx 

TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
TEST_F90_Sources = ${F90_TESTCODES:.f90=.f90.passed}

TEST_F2018_Objects = ${F2018_TESTCODES:.f90=.o}
TEST_F2018_Sources = ${F2018_TESTCODES:.f90=.f90.passed}

PASSING_TEST_Objects = $(TEST_F90_Objects)

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../../testTranslator:
	cd ../../..; $(MAKE) testTranslator

testTranslator: ../../../testTranslator

# DQ (9/14/2014): This forces the relinking of the testTranslator, which is a work 
# around until the OFP and ATerm support is included in ROSE using shared libraries.
recompile_translator:
	rm -f ../../../testTranslator
	$(MAKE) ../../../testTranslator

$(TEST_F90_Objects): recompile_translator ../../../testTranslator
#	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	../../../testTranslator -rose:experimental_fortran_frontend -c $(srcdir)/$(@:.o=.f90)
	../../../testTranslator -rose:experimental_fortran_frontend -rose:experimental_fortran_frontend_OFP_test -c $(srcdir)/$(@:.o=.f90)
## make sure output file is not empty
#
	@test -n "$$(find rose_$(@:.o=.f90) -size +0c)"
endif

$(TEST_F2018_Objects): recompile_translator ../../../testFrontEnd
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
	../../../testFrontEnd -rose:experimental_fortran_frontend -rose:skip_syntax_check $(srcdir)/$(@:.o=.f90)
endif

clean-local:
	rm -f rose_*.* *.aterm *.mod *.o

check-local:
	@echo "Tests for experimental Fortran frontend."
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests/experimental_frontend_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"
else
	@echo "*******************************************************************************************"
	@echo "***** Skipping tests of experimental Fortran frontend (must be configured explicitly) *****"
	@echo "*******************************************************************************************"
endif

