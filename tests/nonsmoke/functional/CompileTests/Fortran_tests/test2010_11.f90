#if 0
For the original interface block

          interface
countarray                                                             
             module procedure c1, c2,
cn                                                   
          end interface          

The unparsed output  has only

INTERFACE countarray
MODULE PROCEDURE c1
END INTERFACE


when running the attached example like this:

--
/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran
-rose:skipfinalCompileStep -rose:output carray.r2f.f95 carray.f95
procedure_name = c1
Fixup functionName = c1 in interfaceName = countarray
Found symbol for function = c1 in interfaceName = countarray
Found symbol for function = c2 in interfaceName = countarray
Found symbol for function = cn in interfaceName = countarray
SgAggregateInitializer::get_type(): default case
SgAggregateInitializer::get_type(): default case
Warning: encountered a case where an array type did not decay to a
pointer type
  parentExpr = 0x2aaaacdb8940 = SgAssignOp, expression = 0x2aaaac8fe3b0
= SgVarRefExp, parentType = 0x2aaaaca77ac0 = SgArrayType, type =
0x2aaaaca77ac0 = SgArrayType
SgAggregateInitializer::get_type(): default case
Warning: encountered a case where an array type did not decay to a
pointer type
  parentExpr = 0x2aaaacdb89a8 = SgAssignOp, expression = 0x2aaaac8fe408
= SgVarRefExp, parentType = 0x2aaaaca77b30 = SgArrayType, type =
0x2aaaaca77b30 = SgArrayType
SgAggregateInitializer::get_type(): default case
SgAggregateInitializer::get_type(): default case
--

jean

#endif


        module carray
          interface countarray
             module procedure c1, c2, cn
          end interface
          contains
          subroutine c1(f,cs)
            real, dimension(:), intent(in) :: f
            integer, intent(out) :: cs

            cs=size(f)
          end subroutine c1

          subroutine c2(f,cs)
            real, dimension(:,:), intent(in) :: f
            integer, intent(out) :: cs

            cs=size(f)
          end subroutine c2

          subroutine cn(f,cs)
            real, intent(in) :: f
            integer, intent(out) :: cs

            cs=0
          end subroutine cn
        end module carray

        program mc
         use carray
         real  :: g0
         real, dimension (3) :: g1
         real, dimension (2,2) :: g2
         integer :: sn, s1, s2
         g0=0.
         g1=(/1., 2., 3./)
         g2=reshape((/1., 2., 3., 4./),(/2,2/))
         call countarray(g0, sn)
         call countarray(g1, s1)
         call countarray(g2, s2)
         if((sn.EQ.0).AND.(s1.EQ.3).AND.(s2.EQ.4)) then
           print *, "OK"
         else
           print *, "failed"
         end if
        end program mc  


