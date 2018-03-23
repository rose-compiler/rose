!! R521 assumed-size-spec
!    [ explicit-shape-spec , ] ... [ lower-bound : ] *

subroutine assumed_shape(bar, truth)
  integer bar(2:6,5:*)
3 Real truth(31,41,*)
end subroutine
