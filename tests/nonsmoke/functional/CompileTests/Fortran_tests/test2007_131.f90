program test

! This is unparsed as: "WRITE (UNIT=2) i,(i,i = 1, 100)"
! This is an example of why (at least in the multi-dimensional case)
! we need to have separate IR nodes for at least 2 of the three
! different types if implied-do loop constructs.
integer j,i,e,f,N
dimension A(5),B(5)

write (2) (e,f,j,i=1,100)

write (*,*) ( B(N),A(N),N=1,100 )

write (*,*) ( B(N),A(N),N=1,100 )

write (*,*) (( B(N),A(N),N=1,100 ),M=7,14)

end
