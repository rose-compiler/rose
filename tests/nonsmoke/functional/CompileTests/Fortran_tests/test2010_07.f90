#if 0
For the attached test file (assoc.f95 [below]) I get:

/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output assoc.r2f.f95 assoc.f95
testTranslator:
/disks/utke/Argonne/svn/CodeReps/anonymous/rose/src/frontend/OpenFortranParser_SAGE_Connection/FortranParserActionROSE.C:9689:
void c_action_if_then_stmt(Token_t*, Token_t*, Token_t*, Token_t*,
Token_t*): Assertion
(*(FortranParserState::getCurrentExpressionStack())).empty() == true
failed.

no output is produced.  Please let me know if you need more info or if I
should create a bug report.

jean
#endif

program main
  real, target :: a, b
  real, pointer :: p, q
  nullify(p, q)
  if(associated(p)) then
     print *, "failed"
  end if
  a=3.
  p => a
  if(.NOT. (associated(p, a))) then
     print *, "failed"
  end if
  q => b
  nullify(q)
  if(associated(q)) then
     print *, "failed"
  else
     print *, "OK"
  end if
end program main


