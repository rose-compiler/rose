include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###########################################################################
# These are Fortran testcodes from the LANL POP code (mostly F77 and F90) #
###########################################################################

# #######################
#      PASSING TESTS
# #######################

F90_TESTCODES_REQUIRED_TO_PASS = $(F90_PASSING_TESTS)

# Make all the code dependent upon the kinds_mod.F90 file
# advection.o : kinds_mod.o

F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS = 

F03_TESTCODES_REQUIRED_TO_PASS = 


# #######################
#      FAILING TESTS
# #######################

F90_TESTCODES_CURRENTLY_FAILING = $(F90_EXTRA_TESTS)

F90_PASSING_TESTS = POP.F90 

# This are the POP source files (excluding module files)
F90_EXTRA_TESTS = 

# These modules are not used!
# current_meters.F90
# drifters.F90
# hydro_sections.F90

# NET CDF library source files (from the web)
NET_CDF_FILES = \
   netcdf.F90 \
   netcdf3_file.f90 \
   netcdf_attributes.f90 \
   netcdf_constants.f90 \
   netcdf_dims.f90 \
   netcdf_eightbyte.f90 \
   netcdf_expanded.f90 \
   netcdf_externals.f90  \
   netcdf_file.f90 \
   netcdf_overloads.f90 \
   netcdf_text_variables.f90 \
   netcdf_variables.f90 \
   netcdf_visibility.f90 \
   typeSizes.F90

# These file are required as dependent modules and will be compiled as part
# of compiling the other files.  They should not be listed as explicit files
# to be compiled else we will generate circular makefile dependencies.
# fix_64.C 
F90_INCLUDED_MODULES = \
   advection.F90 \
   baroclinic.F90 \
   barotropic.F90 \
   broadcast.F90 \
   boundary.F90 \
   kinds_mod.F90 \
   constants.F90 \
   communicate.F90 \
   exit_mod.F90 \
   domain.F90 \
   blocks.F90 \
   domain_size.F90 \
   distribution.F90 \
   forcing.F90 \
   forcing_ap.F90 \
   forcing_coupled.F90 \
   forcing_sfwf.F90 \
   forcing_shf.F90 \
   forcing_pt_interior.F90 \
   forcing_s_interior.F90 \
   forcing_tools.F90 \
   forcing_ws.F90 \
   grid.F90 \
   history.F90 \
   hmix_gm.F90 \
   hmix_aniso.F90 \
   hmix_del2.F90 \
   hmix_del4.F90 \
   horizontal_mix.F90 \
   ice.F90 \
   initial.F90 \
   io_types.F90 \
   io.F90 \
   io_binary.F90 \
   io_netcdf.F90 \
   gather_scatter.F90 \
   global_reductions.F90 \
   diagnostics.F90 \
   movie.F90 \
   output.F90 \
   prognostic.F90 \
   restart.F90 \
   time_management.F90 \
   solvers.F90 \
   step_mod.F90 \
   surface_hgt.F90 \
   tavg.F90 \
   timers.F90 \
   topostress.F90 \
   state_mod.F90 \
   operators.F90 \
   pressure_grad.F90 \
   vertical_mix.F90 \
   vmix_kpp.F90 \
   vmix_const.F90 \
   vmix_rich.F90 \
   xdisplay.F90 \
   $(NET_CDF_FILES)


F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = 

F03_TESTCODES_CURRENTLY_FAILING = 

F77_TESTCODES_REQUIRED_TO_PASS = 

F77_TESTCODES_CURRENTLY_FAILING = 

noinst_headers =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS)

F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS)

F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS)

F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS) \
$(F90_TESTCODES_CURRENTLY_FAILING)

ALL_F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS) \
$(F77_TESTCODES_CURRENTLY_FAILING)

ALL_F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS) \
$(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)

ALL_F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS) \
$(F03_TESTCODES_CURRENTLY_FAILING)

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

# DQ (8/23/2010): This tests only the ability to read the Fortran code and generate the AST.
TRANSLATOR_EXECUTABLE=testAnalysis
# TRANSLATOR_EXECUTABLE=testParser

TRANSLATOR= ../../../$(TRANSLATOR_EXECUTABLE)
# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
$(TRANSLATOR):
	cd ../../..; $(MAKE) $(TRANSLATOR_EXECUTABLE)

# $(TRANSLATOR_EXECUTABLE): $(TRANSLATOR)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/FORTRAN_ROSE_PERFORMANCE_DATA.csv

# Vebose setting of 3 is required for all stack information to be output.
# ROSE_FLAGS = -rose:verbose 0 -rose:output_warnings -rose:output_parser_actions -rose:exit_after_parser
# ROSE_FLAGS = -rose:verbose 3 -rose:output_warnings -rose:output_parser_actions
# ROSE_FLAGS = -rose:verbose 0 -rose:output_warnings -rose:output_parser_actions -rose:compileFixed
# ROSE_FLAGS = -rose:verbose 3 -rose:output_warnings -rose:output_parser_actions -rose:compileFixed
# ROSE_FLAGS = -rose:verbose 0 -rose:compileFixed
# ROSE_FLAGS = -rose:verbose 3
# ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = -rose:verbose 0 -rose:output_warnings -rose:output_parser_actions
ROSE_FLAGS = -rose:verbose 0

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_F90_Objects = ${F90_TESTCODES:.F90=.o}
PASSING_TEST_F77_Objects = ${F77_TESTCODES:.F77=.o}
# PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_TESTCODES:.F=.o}
PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_FIXED_FORMAT_TESTCODES:.F=.o}
PASSING_TEST_F03_Objects = ${F03_TESTCODES:.F03=.o}
TEST_F90_Objects = ${ALL_F90_TESTCODES:.F90=.o}
TEST_F77_Objects = ${ALL_F77_TESTCODES:.F77=.o}
TEST_F77_FIXED_FORMAT_Objects = ${ALL_F77_FIXED_FORMAT_TESTCODES:.F=.o}
TEST_F03_Objects = ${ALL_F03_TESTCODES:.F03=.o}

PASSING_TEST_Objects = $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 


kinds_mod.o: $(TRANSLATOR)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

# constants.o:  $(TRANSLATOR) kinds_mod.o blocks.o
constants.o: $(TRANSLATOR) kinds_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

# DQ (8/5/2010): This test needs access to MPI (not a requirement for ROSE, so it is copied to this directory).
communicate.o: $(TRANSLATOR) kinds_mod.o 
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

exit_mod.o: $(TRANSLATOR) communicate.o constants.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

domain.o: $(TRANSLATOR) kinds_mod.o constants.o communicate.o broadcast.o blocks.o distribution.o exit_mod.o io_types.o boundary.o domain_size.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

blocks.o: $(TRANSLATOR) kinds_mod.o exit_mod.o domain_size.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

domain_size.o: $(TRANSLATOR) kinds_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

distribution.o: $(TRANSLATOR) kinds_mod.o communicate.o blocks.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

grid.o: $(TRANSLATOR) domain.o io.o global_reductions.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

io_types.o: $(TRANSLATOR) broadcast.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

broadcast.o: $(TRANSLATOR) kinds_mod.o communicate.o 
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

boundary.o: $(TRANSLATOR) kinds_mod.o communicate.o constants.o blocks.o distribution.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

io.o: $(TRANSLATOR) io_netcdf.o io_binary.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

io_binary.o: $(TRANSLATOR) kinds_mod.o domain_size.o domain.o constants.o boundary.o communicate.o broadcast.o gather_scatter.o exit_mod.o io_types.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

io_netcdf.o: $(TRANSLATOR) gather_scatter.o netcdf.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

gather_scatter.o: $(TRANSLATOR) kinds_mod.o communicate.o constants.o blocks.o distribution.o domain.o domain_size.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

global_reductions.o: $(TRANSLATOR) kinds_mod.o communicate.o constants.o blocks.o distribution.o domain_size.o domain.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

diagnostics.o: $(TRANSLATOR) prognostic.o time_management.o solvers.o forcing.o timers.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

prognostic.o: $(TRANSLATOR) kinds_mod.o constants.o blocks.o distribution.o domain.o domain_size.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

time_management.o: $(TRANSLATOR) kinds_mod.o constants.o blocks.o domain.o domain_size.o broadcast.o grid.o io.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

solvers.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o boundary.o global_reductions.o gather_scatter.o broadcast.o grid.o io.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing.o: $(TRANSLATOR) forcing_ap.o forcing_coupled.o forcing_pt_interior.o forcing_sfwf.o forcing_shf.o forcing_s_interior.o forcing_tools.o forcing_ws.o tavg.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_ap.o: $(TRANSLATOR) forcing_tools.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_coupled.o: $(TRANSLATOR) ice.o forcing_shf.o forcing_sfwf.o timers.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_pt_interior.o: $(TRANSLATOR) kinds_mod.o domain.o constants.o broadcast.o boundary.o io.o forcing_tools.o time_management.o prognostic.o grid.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_sfwf.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o boundary.o grid.o io.o global_reductions.o forcing_tools.o forcing_shf.o time_management.o prognostic.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_shf.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o boundary.o grid.o io.o forcing_tools.o time_management.o prognostic.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_s_interior.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o boundary.o io.o forcing_tools.o time_management.o prognostic.o grid.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_tools.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o io.o io_types.o grid.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

forcing_ws.o: $(TRANSLATOR) kinds_mod.o constants.o blocks.o distribution.o domain.o boundary.o io.o forcing_tools.o time_management.o grid.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

tavg.o: $(TRANSLATOR) kinds_mod.o constants.o blocks.o distribution.o domain.o prognostic.o grid.o time_management.o global_reductions.o broadcast.o io.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

timers.o: $(TRANSLATOR) kinds_mod.o constants.o domain.o global_reductions.o io.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

ice.o: $(TRANSLATOR) kinds_mod.o blocks.o constants.o broadcast.o io_types.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

state_mod.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o grid.o io.o broadcast.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

operators.o: $(TRANSLATOR) kinds_mod.o blocks.o domain_size.o domain.o constants.o grid.o 
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

pressure_grad.o: $(TRANSLATOR) vertical_mix.o vmix_kpp.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

hmix_gm.o: $(TRANSLATOR) vertical_mix.o vmix_kpp.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

vertical_mix.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o grid.o state_mod.o broadcast.o io.o timers.o tavg.o time_management.o diagnostics.o vmix_kpp.o vmix_const.o vmix_rich.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

vmix_kpp.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain_size.o domain.o constants.o grid.o broadcast.o io.o state_mod.o exit_mod.o forcing_shf.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

vmix_const.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain_size.o domain.o constants.o grid.o broadcast.o io.o state_mod.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

vmix_rich.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o grid.o broadcast.o io.o state_mod.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

horizontal_mix.o: $(TRANSLATOR) kinds_mod.o blocks.o distribution.o domain.o constants.o communicate.o time_management.o broadcast.o grid.o io_types.o hmix_del2.o hmix_del4.o hmix_gm.o hmix_aniso.o topostress.o tavg.o timers.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

pressure_grad.o: $(TRANSLATOR) kinds_mod.o blocks.o constants.o operators.o grid.o broadcast.o communicate.o io.o io_types.o state_mod.o time_management.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

hmix_del2.o: $(TRANSLATOR) kinds_mod.o blocks.o communicate.o distribution.o domain_size.o domain.o broadcast.o boundary.o constants.o topostress.o diagnostics.o io.o global_reductions.o grid.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

topostress.o: $(TRANSLATOR) kinds_mod.o domain.o blocks.o distribution.o constants.o io.o grid.o broadcast.o boundary.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

output.o: $(TRANSLATOR) kinds_mod.o restart.o history.o movie.o tavg.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

restart.o: $(TRANSLATOR) kinds_mod.o domain.o constants.o blocks.o prognostic.o broadcast.o boundary.o communicate.o operators.o grid.o io.o io_types.o time_management.o ice.o forcing.o forcing_ap.o forcing_ws.o forcing_shf.o forcing_sfwf.o forcing_pt_interior.o forcing_s_interior.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

movie.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o prognostic.o grid.o io.o broadcast.o time_management.o forcing.o exit_mod.o operators.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

hmix_del4.o: $(TRANSLATOR) kinds_mod.o blocks.o communicate.o distribution.o domain_size.o domain.o constants.o grid.o broadcast.o io.o global_reductions.o boundary.o diagnostics.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

hmix_aniso.o: $(TRANSLATOR) kinds_mod.o blocks.o communicate.o distribution.o domain.o constants.o broadcast.o global_reductions.o gather_scatter.o diagnostics.o time_management.o grid.o io.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

step_mod.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o prognostic.o boundary.o timers.o grid.o io.o diagnostics.o state_mod.o time_management.o xdisplay.o baroclinic.o barotropic.o surface_hgt.o tavg.o forcing.o forcing_coupled.o ice.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

xdisplay.o: $(TRANSLATOR) kinds_mod.o blocks.o domain_size.o domain.o constants.o communicate.o io_types.o broadcast.o prognostic.o grid.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

advection.o: $(TRANSLATOR) kinds_mod.o constants.o blocks.o distribution.o grid.o diagnostics.o state_mod.o operators.o hmix_gm.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

baroclinic.o: $(TRANSLATOR) domain.o prognostic.o broadcast.o boundary.o communicate.o advection.o pressure_grad.o horizontal_mix.o vertical_mix.o vmix_kpp.o diagnostics.o state_mod.o ice.o time_management.o io_types.o tavg.o forcing.o forcing_shf.o forcing_pt_interior.o forcing_s_interior.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

barotropic.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o prognostic.o boundary.o solvers.o operators.o grid.o time_management.o global_reductions.o forcing.o forcing_ap.o tavg.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

history.o: $(TRANSLATOR) kinds_mod.o domain.o constants.o prognostic.o grid.o io.o broadcast.o time_management.o forcing.o forcing_shf.o exit_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

initial.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o communicate.o broadcast.o prognostic.o solvers.o grid.o io.o io_types.o baroclinic.o barotropic.o pressure_grad.o surface_hgt.o vertical_mix.o horizontal_mix.o advection.o diagnostics.o state_mod.o time_management.o topostress.o ice.o xdisplay.o output.o tavg.o forcing.o forcing_coupled.o exit_mod.o restart.o global_reductions.o timers.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

surface_hgt.o: $(TRANSLATOR) kinds_mod.o blocks.o domain.o constants.o prognostic.o forcing.o grid.o time_management.o tavg.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

# Main file to compile (includes all other files as modules)
POP.o: $(TRANSLATOR) kinds_mod.o communicate.o domain.o timers.o time_management.o step_mod.o initial.o xdisplay.o diagnostics.o output.o solvers.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)




# DQ (8/23/2010): Added support for NetCDFlib
netcdf.o: $(TRANSLATOR) typeSizes.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

typeSizes.o: $(TRANSLATOR)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)


# DQ (9/17/2010): This is a test code to demonstrate a bug in the ROSE F90 support.
io_netcdf_testcode_01.o: $(TRANSLATOR) gather_scatter.o netcdf.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

# DQ (9/17/2010): This is a test code to demonstrate a bug in the ROSE F90 support.
io_netcdf_testcode_02.o: $(TRANSLATOR) gather_scatter.o netcdf.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

test_kinds_mod.o:$(TRANSLATOR)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

test_constants.o:$(TRANSLATOR) test_kinds_mod.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

test_forcing.o:$(TRANSLATOR) test_constants.o
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I. -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)







# Example of how to call the OpenFortranParser
#	java parser.java.FortranMain --class parser.java.FortranParserActionJNI $(srcdir)/$(@:.o=.f90)

# $(TEST_F90_Objects): ../preprocessor ${ALL_TESTCODES}
$(TEST_F90_Objects): $(TRANSLATOR) $(DEPENDENT_MODULE_FILES)
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f90)
#	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f90)
#	$(VALGRIND) ../preprocessor $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.F90)

$(TEST_F77_Objects): $(TRANSLATOR)
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
#	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f77)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.F77)

$(TEST_F77_FIXED_FORMAT_Objects): $(TRANSLATOR)
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
#	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.F)

# Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
#       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
#       to avoid processing the generated code (which would have the same problem).  It is easier to just use
#       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
#       on versions of gfortran 4.2 and 4.3.
$(TEST_F03_Objects): $(TRANSLATOR)
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f03)
#	java fortran.ofp.FrontEnd --dump $(srcdir)/$(@:.o=.f03)
	$(VALGRIND) $(TRANSLATOR) $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.F03)
#	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
	touch $@

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
	egrep -n "*"  $(srcdir)/$(?)

check-parsing:
	@echo "First make POP code using testParser."
	@$(MAKE) TRANSLATOR_EXECUTABLE=testParser $(PASSING_TEST_Objects)
	@echo "Test of check-parsing rule terminated normally..."

check-analysis:
	@echo "Second make POP code using testAnalysis."
	@$(MAKE) TRANSLATOR_EXECUTABLE=testAnalysis $(PASSING_TEST_Objects)
	@echo "Test of check-analysis rule terminated normally..."

check-local:
	@echo "Tests for Fortran (LANL POP)."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
if !ROSE_USING_GFORTRAN_VERSION_4_0
#	@$(MAKE) check-parsing
#	@$(MAKE) clean
	@$(MAKE) check-analysis
else
	@echo "Skipping POP code tests when using the gfortran version 4.0.x compiler (fails to compile POP code)."
endif
	@echo "********************************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests/LANL_POP: make check rule complete (terminated normally using $(TRANSLATOR)) ******"
	@echo "********************************************************************************************************************************"
else
check-local:
	@echo "\nROSE Fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
	@echo "******************************************************************************************"
	@echo "******************* make check not run here: configure --with-java ***********************"
	@echo "******************************************************************************************"
endif

copyFiles:
	echo "LANL POP files not copied to ROSE/tests directory..."

EXTRA_DIST = CMakeLists.txt mpif.h $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES) $(F90_INCLUDED_MODULES)

clean-local:
	rm -f *.o *.s *.mod *.rmod rose_*.C rose_*.f rose_*.F* rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 *_postprocessed.* *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock
	rm -rf QMTest

