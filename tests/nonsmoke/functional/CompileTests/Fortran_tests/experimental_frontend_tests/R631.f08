!! R631 allocation
!    is allocate-object [ ( allocate-shape-spec-list ) ]
!                       [ lbracket allocate-co-array-spec rbracket ]

allocate (this, that, theother)

allocate( T[3,*] )

! NOTE 6.18
ALLOCATE(T[*])
ALLOCATE(T%AAC(N))
ALLOCATE(T[Q]%AAC(N))    ! MUST be executed on image Q to be correct

   ALLOCATE(T(9))

end
