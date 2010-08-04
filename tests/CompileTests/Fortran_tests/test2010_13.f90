#if 0
Hi Dan,

I owe you a couple of test cases I have mentioned to you in the past.
Attached is one with an "implied do loop" that gives:

Warning: implied do loop variable is not availble in OFP
Warning: implied do loop variable is not availble in OFP
Warning: encountered a case where an array type did not decay to a
pointer type
  parentExpr = 0x2aaaacc5cdb0 = SgAssignOp, expression = 0x2aaaac8fe378
= SgVarRefExp, parentType = 0x2aaaaca5e4a0 = SgArrayType, type =
0x2aaaaca5e4a0 = SgArrayType
Warning, implied do loop index variable not found (unavailalbe in OFP
for initilizers and data statements) lookinf for one to use in the
object_list
Searching for implied do loop variable, but object_list contains non
SgPntrArrRefExp entry i = SgImpliedDo
testTranslator:
/disks/utke/Argonne/svn/CodeReps/anonymous/rose/src/backend/unparser/FortranCodeGeneration/unparseFortran_expressions.C:1146:
virtual void
FortranCodeGeneration_locatedNode::unparseImpliedDo(SgExpression*,
SgUnparse_Info&): Assertion `indexVariable != __null' failed.

when I run it like this:
/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output arrays3.r2f.f95 arrays3.f90

The output in arrays3.r2f.f95  is incomplete.
The Rose I am using is clean build/install from an SVN checkout done on
Monday.

Are shortcomings of OFP to be solved by somebody in your group or how is it
supposed to be tackled?

Thanks,

Jean

#endif

program arrays3
  real :: x(2,3), y(2)
  DATA ((x (i,j), i=1,2), j=1,3) / 6 * 2.0 /
  y = x(1:2,2)
  if (y(1).eq. 2.0) then 
    print *, 'OK'
  else
    print *, y
  endif 
end program
