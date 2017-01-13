set(files_to_test
  abarrier-1.caf acopy-1.caf acopy-2.caf allgather-1.caf allocate-s.caf
  allocate.caf alltoall-1.caf alltoall-2.caf alltoall-3.caf assign-1.caf
  assign-2.caf assumed-size-coarray-1.caf assumed-size-coarray-2.caf
  attrs-async.caf attrs-copointer.caf attrs-external.caf
  attrs-pointer-coarray.caf basic-1.caf broadcast-3.caf broadcast-4.caf
  broadcast-5.caf broadcast-6.caf caf-allocate.caf caf-derived_type.caf
  caf-sub.caf caf-team.caf caf-topology.caf caf.caf cmdline-1.caf cmdline-2.caf
  coarray-3.caf coarray-4.caf coarray-6.caf coarray-allocation-1.caf
  coarray-bounds-1.caf coarray-bounds-2.caf coarray-decl.caf
  coarray-semantics-1.caf coarray-string.caf copointer-components.caf
  copointer-array.caf copointer-coarray.caf copointer-declarations.caf
  copointer-derived-types.caf copointer-operations.caf
  copointer-scalar-coscalar.caf copointer-team1.caf copointer-team2.caf
  coshape.caf cotarget-declarations.caf coscalar-1.caf derived-type-1.caf
  derived-type-2.caf derived-type-3.caf derived-type-4.caf derived-type-5.caf
  derived-type-6.caf derived.caf dimensions-1.caf event-1.caf event-2.caf
  event-3.caf event-4.caf event-5.caf event.caf explicit-shape-1.caf
  func-decl-1.caf func-decl-4.caf get-1.caf get-2.caf get-3.caf lockset.caf
  loopbound-1.caf image-selector.caf memory-1.caf module-1-mod1.caf
  module-2-module_table.caf pointer-1.caf pointer-coarray-derived-type.caf
  put-1.caf put-3.caf remote-read-1.caf remote-write-1.caf team.caf
  topology.caf use-only-rename.caf use-rename.caf withteam.caf
)

# the following tests currently fail
# X-copointer-operations-long.caf
# X-cotarget-declarations-long.caf
# X-test-copointer4_1.caf

set(ROSE_FLAGS
  -rose:caf2.0 -rose:skip_syntax_check -rose:skipfinalCompileStep)

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} TRUE)
endforeach()
