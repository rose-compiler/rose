!! R637 allocate-coshape-spec
!    is [ lower-bound-expr : ] upper-bound-expr
!
allocate(A1[3,*])
allocate(A2[2:3,*])
allocate(A3[2,3:4,*])

end
