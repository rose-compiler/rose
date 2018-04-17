    program caf

    integer,allocatable::iall(:)[*], ibll(:)[*]
    integer::iarr(100)[*]
    integer::i
    team::team3,team4
    
    iarr(1) = 100

    with team team4
      allocate(iall(100)[@team3])
      iall = iarr(10)[iarr(1)[2]+2@team3]
    end withteam team4
  
    iall[2] = iarr
    iall(:)[2] = iarr
    iall(:)[2+i] = iarr
    iall(2)[3@team3] = iarr(3)
    iall(:)[i+3@team4] = iarr(3)

    deallocate(iall)

    end program caf
