!! R636 allocate-coarray-spec
!    is [ allocate-coshape-spec-list , ] [ lower-bound-expr : ] *
!
allocate(A1[*])
allocate(A2[3:*])
allocate(A3[1,*])
allocate(A4[3,13,4:*])

end
