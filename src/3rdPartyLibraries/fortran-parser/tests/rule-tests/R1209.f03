! Test import-stmt
!      import-stmt  is  IMPORT [ [::] import-name-list ]
!
! Not tested here: import-name-list
! 
! import-stmt is only allowed in an interface-body (C1210).
interface
   subroutine sub()
     ! No optional parts.
     import
   end subroutine sub

   subroutine sub1()
     ! Optional import-name-list
     import a, b
   end subroutine sub1

   subroutine sub2()
     ! Optional import-name-list with :: separater.
     import :: a, b, c
   end subroutine sub2
end interface

end

