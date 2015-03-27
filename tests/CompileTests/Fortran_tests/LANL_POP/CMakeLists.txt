set(ROSE_FLAGS -rose:verbose 0)

function(LANL_POP_test file_to_compile)
  add_test(
    NAME ${file_to_compile}
    COMMAND testAnalysis ${ROSE_FLAGS}
            -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_BINARY_DIR}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_compile})
endfunction()

set(files_to_compile
  advection.F90
  baroclinic.F90
  barotropic.F90
  blocks.F90
  boundary.F90
  broadcast.F90
  communicate.F90
  constants.F90
  diagnostics.F90
  distribution.F90
  domain.F90
  domain_size.F90
  exit_mod.F90
  forcing.F90
  forcing_ap.F90
  forcing_coupled.F90
  forcing_pt_interior.F90
  forcing_s_interior.F90
  forcing_sfwf.F90
  forcing_shf.F90
  forcing_tools.F90
  forcing_ws.F90
  gather_scatter.F90
  global_reductions.F90
  grid.F90
  history.F90
  hmix_aniso.F90
  hmix_del2.F90
  hmix_del4.F90
  hmix_gm.F90
  horizontal_mix.F90
  ice.F90
  initial.F90
  io.F90
  io_binary.F90
  io_netcdf.F90
  #expected failure: io_netcdf_testcode_01.F90
  #expected failure: io_netcdf_testcode_02.F90
  io_types.F90
  kinds_mod.F90
  movie.F90
  netcdf.F90
  operators.F90
  output.F90
  pressure_grad.F90
  prognostic.F90
  restart.F90
  solvers.F90
  state_mod.F90
  step_mod.F90
  surface_hgt.F90
  tavg.F90
  test_constants.F90
  test_forcing.F90
  test_kinds_mod.F90
  time_management.F90
  timers.F90
  topostress.F90
  typeSizes.F90
  vertical_mix.F90
  vmix_const.F90
  vmix_kpp.F90
  vmix_rich.F90
  xdisplay.F90
  POP.F90
)

foreach(file_to_compile ${files_to_compile})
  LANL_POP_test(${file_to_compile})
endforeach()

# set up dependencies between tests

# Main file to compile (includes all other files as modules)
set_tests_properties(POP.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90;domain.F90;timers.F90;time_management.F90;step_mod.F90;initial.F90;xdisplay.F90;diagnostics.F90;output.F90;solvers.F90")

set_tests_properties(advection.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;blocks.F90;distribution.F90;grid.F90;diagnostics.F90;state_mod.F90;operators.F90;hmix_gm.F90")

set_tests_properties(baroclinic.F90 PROPERTIES DEPENDS
  "domain.F90;prognostic.F90;broadcast.F90;boundary.F90;communicate.F90;advection.F90;pressure_grad.F90;horizontal_mix.F90;vertical_mix.F90;vmix_kpp.F90;diagnostics.F90;state_mod.F90;ice.F90;time_management.F90;io_types.F90;tavg.F90;forcing.F90;forcing_shf.F90;forcing_pt_interior.F90;forcing_s_interior.F90;exit_mod.F90")

set_tests_properties(barotropic.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;prognostic.F90;boundary.F90;solvers.F90;operators.F90;grid.F90;time_management.F90;global_reductions.F90;forcing.F90;forcing_ap.F90;tavg.F90")

set_tests_properties(blocks.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;exit_mod.F90;domain_size.F90")

set_tests_properties(boundary.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90;constants.F90;blocks.F90;distribution.F90;exit_mod.F90")

set_tests_properties(broadcast.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90")

set_tests_properties(communicate.F90 PROPERTIES DEPENDS kinds_mod.F90)

set_tests_properties(constants.F90 PROPERTIES DEPENDS kinds_mod.F90)

set_tests_properties(diagnostics.F90 PROPERTIES DEPENDS
  "prognostic.F90;time_management.F90;solvers.F90;forcing.F90;timers.F90")

set_tests_properties(distribution.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90;blocks.F90;exit_mod.F90")

set_tests_properties(domain.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;communicate.F90;broadcast.F90;blocks.F90;distribution.F90;exit_mod.F90;io_types.F90;boundary.F90;domain_size.F90")

set_tests_properties(domain_size.F90 PROPERTIES DEPENDS kinds_mod.F90)

set_tests_properties(exit_mod.F90 PROPERTIES DEPENDS
  "communicate.F90;constants.F90")

set_tests_properties(forcing.F90 PROPERTIES DEPENDS
  "forcing_ap.F90;forcing_coupled.F90;forcing_pt_interior.F90;forcing_sfwf.F90;forcing_shf.F90;forcing_s_interior.F90;forcing_tools.F90;forcing_ws.F90;tavg.F90")

set_tests_properties(forcing_ap.F90 PROPERTIES DEPENDS forcing_tools.F90)

set_tests_properties(forcing_coupled.F90 PROPERTIES DEPENDS
  "ice.F90;forcing_shf.F90;forcing_sfwf.F90;timers.F90")

set_tests_properties(forcing_pt_interior.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;domain.F90;constants.F90;broadcast.F90;boundary.F90;io.F90;forcing_tools.F90;time_management.F90;prognostic.F90;grid.F90;exit_mod.F90")

set_tests_properties(forcing_sfwf.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;boundary.F90;grid.F90;io.F90;global_reductions.F90;forcing_tools.F90;forcing_shf.F90;time_management.F90;prognostic.F90;exit_mod.F90")

set_tests_properties(forcing_shf.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;boundary.F90;grid.F90;io.F90;forcing_tools.F90;time_management.F90;prognostic.F90;exit_mod.F90")

set_tests_properties(forcing_s_interior.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;boundary.F90;io.F90;forcing_tools.F90;time_management.F90;prognostic.F90;grid.F90;exit_mod.F90")

set_tests_properties(forcing_tools.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;io.F90;io_types.F90;grid.F90;time_management.F90;exit_mod.F90")

set_tests_properties(forcing_ws.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;blocks.F90;distribution.F90;domain.F90;boundary.F90;io.F90;forcing_tools.F90;time_management.F90;grid.F90;exit_mod.F90")

set_tests_properties(gather_scatter.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90;constants.F90;blocks.F90;distribution.F90;domain.F90;domain_size.F90;exit_mod.F90")

set_tests_properties(global_reductions.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;communicate.F90;constants.F90;blocks.F90;distribution.F90;domain_size.F90;domain.F90")

set_tests_properties(grid.F90 PROPERTIES DEPENDS
  "domain.F90;io.F90;global_reductions.F90")

set_tests_properties(history.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;domain.F90;constants.F90;prognostic.F90;grid.F90;io.F90;broadcast.F90;time_management.F90;forcing.F90;forcing_shf.F90;exit_mod.F90")

set_tests_properties(hmix_aniso.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;communicate.F90;distribution.F90;domain.F90;constants.F90;broadcast.F90;global_reductions.F90;gather_scatter.F90;diagnostics.F90;time_management.F90;grid.F90;io.F90;exit_mod.F90")

set_tests_properties(hmix_del2.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;communicate.F90;distribution.F90;domain_size.F90;domain.F90;broadcast.F90;boundary.F90;constants.F90;topostress.F90;diagnostics.F90;io.F90;global_reductions.F90;grid.F90;exit_mod.F90")

set_tests_properties(hmix_del4.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;communicate.F90;distribution.F90;domain_size.F90;domain.F90;constants.F90;grid.F90;broadcast.F90;io.F90;global_reductions.F90;boundary.F90;diagnostics.F90;exit_mod.F90")

set_tests_properties(hmix_gm.F90 PROPERTIES DEPENDS
  "vertical_mix.F90;vmix_kpp.F90")

set_tests_properties(horizontal_mix.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;communicate.F90;time_management.F90;broadcast.F90;grid.F90;io_types.F90;hmix_del2.F90;hmix_del4.F90;hmix_gm.F90;hmix_aniso.F90;topostress.F90;tavg.F90;timers.F90;exit_mod.F90;io_netcdf.F90")

set_tests_properties(ice.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;constants.F90;broadcast.F90;io_types.F90;time_management.F90;exit_mod.F90")

set_tests_properties(initial.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;communicate.F90;broadcast.F90;prognostic.F90;solvers.F90;grid.F90;io.F90;io_types.F90;baroclinic.F90;barotropic.F90;pressure_grad.F90;surface_hgt.F90;vertical_mix.F90;horizontal_mix.F90;advection.F90;diagnostics.F90;state_mod.F90;time_management.F90;topostress.F90;ice.F90;xdisplay.F90;output.F90;tavg.F90;forcing.F90;forcing_coupled.F90;exit_mod.F90;restart.F90;global_reductions.F90;timers.F90")

set_tests_properties(io_types.F90 PROPERTIES DEPENDS broadcast.F90)

set_tests_properties(io.F90 PROPERTIES DEPENDS "io_netcdf.F90;io_binary.F90")

set_tests_properties(io_binary.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;domain_size.F90;domain.F90;constants.F90;boundary.F90;communicate.F90;broadcast.F90;gather_scatter.F90;exit_mod.F90;io_types.F90")

set_tests_properties(io_netcdf.F90 PROPERTIES DEPENDS
  "gather_scatter.F90;netcdf.F90")

#EXPECTED FAILURES:
# DQ (9/17/2010): This is a test code to demonstrate a bug in the ROSE F90 support.
#set_tests_properties(io_netcdf_testcode_01.F90 PROPERTIES DEPENDS
#  "gather_scatter.F90;netcdf.F90")

# DQ (9/17/2010): This is a test code to demonstrate a bug in the ROSE F90 support.
#set_tests_properties(io_netcdf_testcode_02.F90 PROPERTIES DEPENDS
#  "gather_scatter.F90;netcdf.F90")

set_tests_properties(io_types.F90 PROPERTIES DEPENDS broadcast.F90)

set_tests_properties(movie.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;prognostic.F90;grid.F90;io.F90;broadcast.F90;time_management.F90;forcing.F90;exit_mod.F90;operators.F90")

# DQ (8/23/2010): Added support for NetCDFlib
set_tests_properties(netcdf.F90 PROPERTIES DEPENDS typeSizes.F90)

set_tests_properties(operators.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain_size.F90;domain.F90;constants.F90;grid.F90;")

set_tests_properties(output.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;restart.F90;history.F90;movie.F90;tavg.F90")

set_tests_properties(pressure_grad.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;constants.F90;operators.F90;grid.F90;broadcast.F90;communicate.F90;io.F90;io_types.F90;state_mod.F90;time_management.F90;exit_mod.F90")

set_tests_properties(prognostic.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;blocks.F90;distribution.F90;domain.F90;domain_size.F90")

set_tests_properties(restart.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;domain.F90;constants.F90;blocks.F90;prognostic.F90;broadcast.F90;boundary.F90;communicate.F90;operators.F90;grid.F90;io.F90;io_types.F90;time_management.F90;ice.F90;forcing.F90;forcing_ap.F90;forcing_ws.F90;forcing_shf.F90;forcing_sfwf.F90;forcing_pt_interior.F90;forcing_s_interior.F90;exit_mod.F90")

set_tests_properties(solvers.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;boundary.F90;global_reductions.F90;gather_scatter.F90;broadcast.F90;grid.F90;io.F90;time_management.F90;exit_mod.F90")

set_tests_properties(state_mod.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;grid.F90;io.F90;broadcast.F90;time_management.F90;exit_mod.F90")

set_tests_properties(step_mod.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;prognostic.F90;boundary.F90;timers.F90;grid.F90;io.F90;diagnostics.F90;state_mod.F90;time_management.F90;xdisplay.F90;baroclinic.F90;barotropic.F90;surface_hgt.F90;tavg.F90;forcing.F90;forcing_coupled.F90;ice.F90")

set_tests_properties(surface_hgt.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain.F90;constants.F90;prognostic.F90;forcing.F90;grid.F90;time_management.F90;tavg.F90")

set_tests_properties(tavg.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;blocks.F90;distribution.F90;domain.F90;prognostic.F90;grid.F90;time_management.F90;global_reductions.F90;broadcast.F90;io.F90;exit_mod.F90")

set_tests_properties(test_constants.F90 PROPERTIES DEPENDS test_kinds_mod.F90)

set_tests_properties(test_forcing.F90 PROPERTIES DEPENDS test_constants.F90)

set_tests_properties(time_management.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;blocks.F90;domain.F90;domain_size.F90;broadcast.F90;grid.F90;io.F90;exit_mod.F90")

set_tests_properties(timers.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;constants.F90;domain.F90;global_reductions.F90;io.F90;exit_mod.F90")

set_tests_properties(topostress.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;domain.F90;blocks.F90;distribution.F90;constants.F90;io.F90;grid.F90;broadcast.F90;boundary.F90;exit_mod.F90")

set_tests_properties(vertical_mix.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;grid.F90;state_mod.F90;broadcast.F90;io.F90;timers.F90;tavg.F90;time_management.F90;diagnostics.F90;vmix_kpp.F90;vmix_const.F90;vmix_rich.F90;exit_mod.F90")

set_tests_properties(vmix_const.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain_size.F90;domain.F90;constants.F90;grid.F90;broadcast.F90;io.F90;state_mod.F90;exit_mod.F90")

set_tests_properties(vmix_kpp.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain_size.F90;domain.F90;constants.F90;grid.F90;broadcast.F90;io.F90;state_mod.F90;exit_mod.F90;forcing_shf.F90")

set_tests_properties(vmix_rich.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;distribution.F90;domain.F90;constants.F90;grid.F90;broadcast.F90;io.F90;state_mod.F90;time_management.F90;exit_mod.F90")

set_tests_properties(xdisplay.F90 PROPERTIES DEPENDS
  "kinds_mod.F90;blocks.F90;domain_size.F90;domain.F90;constants.F90;communicate.F90;io_types.F90;broadcast.F90;prognostic.F90;grid.F90;exit_mod.F90")

