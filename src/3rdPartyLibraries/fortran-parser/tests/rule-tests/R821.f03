! Test select-type-construct
!      select-type-construct  is  select-type-stmt
!                                   [ type-guard-stmt 
!                                       block ] ...
!                                 end-select-type-stmt
!
!      select-type-stmt  is  [ select-construct-name : ] SELECT TYPE 
!                              ( [ associate-name => ] selector)
!
!      type-guard-stmt  is  TYPE IS ( type-spec) [ select-construct-name ]
!                       or  CLASS IS ( type-spec ) [ select-construct-name ]
!                       or  CLASS DEFAULT [ select-construct-name ]
!
!      end-select-type-stmt  is  END SELECT [ select-construct-name ]
!
! Tested here: select-type-construct, select-type-stmt, type-guard-stmt, and 
! end-select-type-stmt.
!
! Not tested here: block, select-construct-name, associate-name, selector, 
! and type-spec
select type (a => p) 
class is ( point )
   x = 1
type is (point_3d)
   x = 2
class default
   x = 3
end select 

select type (p) 
class is ( point )
   x = 1
type is (point_3d)
   x = 2
class default
   x = 3
end select 


my_select: select type (p) 
class is ( point ) my_select
   x = 1
type is (point_3d) my_select
   x = 2
class default my_select
   x = 3
end select my_select

end


