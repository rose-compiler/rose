! DQ (1/30/2010): This is an OFP bug, I think that ROSE will handle 
! it fine once it is fixed in OFP. Until then it is still considered 
! a failing test.

! S03-continuation-spits-token.f
! OFP does not handle fixed-format continuation lines correctly.
! If a single token is split across two lines, which is legal,
! OFP treats the two halves as separate tokens.

      program p

    ! DQ (1/31/2010): This test code should have implicit none.
      implicit none

      integer hello
     1world
      helloworld = 0
      end program
