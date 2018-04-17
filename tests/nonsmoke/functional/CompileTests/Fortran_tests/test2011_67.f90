! Example of named labels used with EXIT statement.
   S1:  DO
           IF (X > Y ) THEN
                 Z = X
                 EXIT S1
           END IF
           CALL NEW(X)
        END DO

        N = 0
 LOOP1: DO I = 1, 10
           J= I
 LOOP2:       DO K =1, 5
                 L = K
                 N = N +1
              END DO LOOP2
        END DO LOOP1


      jmax=n-1
      outer: do i=1,n-1
        temp=1.e38
        inner: do j=1,jmax
          if(x(j).gt.x(j+1)) cycle inner
            temp=x(j)
            x(j)=x(j+1)
            x(j+1)=temp
            itemp=iy(j)
            iy(j)=iy(j+1)
            iy(j+1)=itemp
        end do inner
        if(temp.eq.1.e38) exit outer
        jmax=jmax-1
      end do outer
end
