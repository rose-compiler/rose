!     C.4.5     An example of a FORALL construct containing a WHERE construct

     INTEGER :: A(5,5)

     FORALL (I = 1:5)
        WHERE (A(I,:) == 0)
           A(:,I) = I
        ELSEWHERE (A(I,:) > 2)
           A(I,:) = 6
        END WHERE
     END FORALL

end
