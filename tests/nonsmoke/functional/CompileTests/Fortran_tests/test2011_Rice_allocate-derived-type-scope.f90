! Rice test
! Error: SgScopeStatement::append in a scope such as SgClassDefinition must provide a SgDeclarationStatement = SgAllocateStatement 
! testAnalysis: Cxx_Grammar.C:63733: void SgScopeStatement::append_statement(SgStatement*): Assertion `declaration != __null' failed.
 
program allocate_derived_type_scope

   type :: t2
      integer ::  id
   end type
   
   type:: t1
      type (t2), dimension(:), pointer  :: x
   end type

   type (t1) :: a 
   allocate (a%x(1:3))  ! Assertion `declaration != __null' failed

end program
