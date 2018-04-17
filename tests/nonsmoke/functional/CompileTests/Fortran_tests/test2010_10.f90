#if 0
For the attached test file when run like this:

/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output caseMultChoice.r2f.f95
caseMultChoice.f90
testTranslator:
/disks/utke/Argonne/svn/CodeReps/anonymous/rose/src/frontend/OpenFortranParser_SAGE_Connection/fortran_support.C:4651:
void generateAssignmentStatement(Token_t*, bool): Assertion
`(*(FortranParserState::getCurrentExpressionStack())).empty() == true'
failed.

Jean
#endif

program main
integer n
integer x
n=3
select case(n)
case(0)
x=1
case(1,3,5)
x=2
case default
x=3
end select
if( x.EQ.2) then
  print *, "OK"
else
 print *, "failed",X
end if   
end program main


