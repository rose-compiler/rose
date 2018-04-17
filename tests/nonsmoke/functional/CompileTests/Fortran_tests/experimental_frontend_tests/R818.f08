!! R818 loop-control
!    is [ , ] do-variable = scalar-int-expr, scalar-int-expr [ , scalar-int-expr ]
!    or [ , ] WHILE ( scalar-logical-expr )
!    or [ , ] CONCURRENT forall-header
!
! from NOTE 8.10

real :: A(10), B(10)

   DO CONCURRENT (I = 1:N)
      A(I) = B(I)
   END DO

end
