
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
############################################################################
## These are Fortran testcodes from the LANL POP code (mostly F77 and F90) #
############################################################################
#
## #######################
##      PASSING TESTS
## #######################
#
#F90_TESTCODES_REQUIRED_TO_PASS = 
#
## Make all the code dependent upon the kinds_mod.F90 file
## advection.o : kinds_mod.o
#
#F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS = 
#
#F03_TESTCODES_REQUIRED_TO_PASS = 
#
#
## #######################
##      FAILING TESTS
## #######################
#
#F90_TESTCODES_CURRENTLY_FAILING = 
#
#F90_EXTRA_TESTS = \
#   advection.F90 \
#   baroclinic.F90 \
#   barotropic.F90 \
#   blocks.F90 \
#   current_meters.F90 \
#   diagnostics.F90 \
#   distribution.F90 \
#   domain.F90 \
#   drifters.F90 \
#   exit_mod.F90 \
#   fix_64.C \
#   forcing_ap.F90 \
#   forcing_coupled.F90 \
#   forcing.F90 \
#   forcing_pt_interior.F90 \
#   forcing_sfwf.F90 \
#   forcing_shf.F90 \
#   forcing_s_interior.F90 \
#   forcing_tools.F90 \
#   forcing_ws.F90 \
#   grid.F90 \
#   history.F90 \
#   hmix_aniso.F90 \
#   hmix_del2.F90 \
#   hmix_del4.F90 \
#   hmix_gm.F90 \
#   horizontal_mix.F90 \
#   hydro_sections.F90 \
#   ice.F90 \
#   initial.F90 \
#   io_binary.F90 \
#   io.F90 \
#   io_netcdf.F90 \
#   io_types.F90 \
#   movie.F90 \
#   operators.F90 \
#   output.F90 \
#   POP.F90 \
#   pressure_grad.F90 \
#   prognostic.F90 \
#   restart.F90 \
#   solvers.F90 \
#   state_mod.F90 \
#   step_mod.F90 \
#   surface_hgt.F90 \
#   tavg.F90 \
#   time_management.F90 \
#   timers.F90 \
#   topostress.F90 \
#   vertical_mix.F90 \
#   vmix_const.F90 \
#   vmix_kpp.F90 \
#   vmix_rich.F90 \
#   xdisplay.F90
#
#F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = 
#
#F03_TESTCODES_CURRENTLY_FAILING = 
#
#
#
#
#
#
#
#
#F77_TESTCODES_REQUIRED_TO_PASS = 
#
#F77_TESTCODES_CURRENTLY_FAILING = 
#
#noinst_headers =
#
## Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
#F90_TESTCODES = \
#$(F90_TESTCODES_REQUIRED_TO_PASS)
#
#F77_TESTCODES = \
#$(F77_TESTCODES_REQUIRED_TO_PASS)
#
#F77_FIXED_FORMAT_TESTCODES = \
#$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS)
#
#F03_TESTCODES = \
#$(F03_TESTCODES_REQUIRED_TO_PASS)
#
## QMTest allows both passing and failing tests.
#ALL_F90_TESTCODES = \
#$(F90_TESTCODES_REQUIRED_TO_PASS) \
#$(F90_TESTCODES_CURRENTLY_FAILING)
#
#ALL_F77_TESTCODES = \
#$(F77_TESTCODES_REQUIRED_TO_PASS) \
#$(F77_TESTCODES_CURRENTLY_FAILING)
#
#ALL_F77_FIXED_FORMAT_TESTCODES = \
#$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS) \
#$(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
#
#ALL_F03_TESTCODES = \
#$(F03_TESTCODES_REQUIRED_TO_PASS) \
#$(F03_TESTCODES_CURRENTLY_FAILING)
#
## "if" for DQ_DEVELOPER_TESTS (AutoMake conditional block)
#if DQ_DEVELOPER_TESTS
#
#if ROSE_USE_OPEN_FORTRAN_PARSER
#
#../../preprocessor:
#	cd ../..; $(MAKE) preprocessor
#
#preprocessor: ../../preprocessor
#
## File option to accumulate performance information about the compilation
#PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/FORTRAN_ROSE_PERFORMANCE_DATA.csv
#
## Vebose setting of 3 is required for all stack information to be output.
## ROSE_FLAGS = -rose:verbose 0 -rose:output_warnings -rose:output_parser_actions -rose:exit_after_parser
## ROSE_FLAGS = -rose:verbose 3 -rose:output_warnings -rose:output_parser_actions
## ROSE_FLAGS = -rose:verbose 0 -rose:output_warnings -rose:output_parser_actions -rose:compileFixed
## ROSE_FLAGS = -rose:verbose 3 -rose:output_warnings -rose:output_parser_actions -rose:compileFixed
## ROSE_FLAGS = -rose:verbose 0 -rose:compileFixed
## ROSE_FLAGS = -rose:verbose 3
#ROSE_FLAGS = -rose:verbose 3 -rose:output_warnings -rose:output_parser_actions
## ROSE_FLAGS = -rose:verbose 0
#
## VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
## VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
## VALGRIND = valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
#PASSING_TEST_F90_Objects = ${F90_TESTCODES:.F90=.o}
#PASSING_TEST_F77_Objects = ${F77_TESTCODES:.F77=.o}
## PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_TESTCODES:.F=.o}
#PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_FIXED_FORMAT_TESTCODES:.F=.o}
#PASSING_TEST_F03_Objects = ${F03_TESTCODES:.F03=.o}
#TEST_F90_Objects = ${ALL_F90_TESTCODES:.F90=.o}
#TEST_F77_Objects = ${ALL_F77_TESTCODES:.F77=.o}
#TEST_F77_FIXED_FORMAT_Objects = ${ALL_F77_FIXED_FORMAT_TESTCODES:.F=.o}
#TEST_F03_Objects = ${ALL_F03_TESTCODES:.F03=.o}
#
#PASSING_TEST_Objects = $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 
#
#
#kinds_mod.o:  ../../preprocessor
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)
#
#constants.o:  ../../preprocessor kinds_mod.o
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)
#
## Example of how to call the OpenFortranParser
##	java parser.java.FortranMain --class parser.java.FortranParserActionJNI $(srcdir)/$(@:.o=.f90)
#
## $(TEST_F90_Objects): ../preprocessor ${ALL_TESTCODES}
#$(TEST_F90_Objects): ../../preprocessor kinds_mod.o constants.o
##	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f90)
##	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f90)
##	$(VALGRIND) ../preprocessor $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)
#
#$(TEST_F77_Objects): ../../preprocessor
##	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
##	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f77)
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.F77)
#
#$(TEST_F77_FIXED_FORMAT_Objects): ../../preprocessor
##	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
##	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f)
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.F)
#
## Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
##       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
##       to avoid processing the generated code (which would have the same problem).  It is easier to just use
##       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
##       on versions of gfortran 4.2 and 4.3.
#$(TEST_F03_Objects): ../../preprocessor
##	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f03)
##	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f03)
#	$(VALGRIND) ../../preprocessor $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.F03)
##	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
#	touch $@
#
#
#QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}
#
## Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): preprocessor $(srcdir)/$(@:.qmt=.c)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../preprocessor NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)
#
## Include makefile rules specific to QMTest
#include $(top_srcdir)/config/QMTest_makefile.inc
#
#
## This will output the example testcodes that are failing
#listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
#	egrep -n "*"  $(srcdir)/$(?)
#
#
#check-local:
#	@echo "Dan Quinlan's development tests for Fortran."
##  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
##	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects)
#	@echo "********************************************************************************************************************"
#	@echo "****** ROSE/developersScratchSpace/Dan/Fortran_tests/LANL_POP: make check rule complete (terminated normally) ******"
#	@echo "********************************************************************************************************************"
#endif
#
#copyFiles:
#	echo "LANL POP files not copied to ROSE/tests directory..."
#
#else
#check-local:
#	@echo "\nDan Quinlan's Fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
#	@echo "**********************************************************************************************************"
#	@echo "******************* make check not run here: configure --enable-dq-developer-tests ***********************"
#	@echo "**********************************************************************************************************"
#endif
#
#EXTRA_DIST = $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES)
#
#clean-local:
#	rm -f *.o *.s *.mod rose_*.C rose_*.f rose_*.F* rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 *_postprocessed.* *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock
#	rm -rf QMTest
#
