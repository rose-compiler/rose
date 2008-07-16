! Test block-data
!      block-data  is  block-data-stmt
!                        [ specification-part ]
!                      end-block-data-stmt
!
!      block-data-stmt  is  BLOCK DATA [ block-data-name ]
!
!      end-block-data-stmt  is  END [ BLOCK DATA [ block-data-name ] ] 
!
! Not tested here: specification-part and block-data-name.

! Include none of the optional parts.
block data 
end

! Include just the optional "BLOCK DATA" part of end-block-data-stmt
block data
end block data

! Include the optional block-data-name
block data foo
end block data foo

! Include the optional specification-part
block data foo
   integer i
end block data foo
