program p
    real(4) :: donor_variability(5,6,7)
    real(4) :: temperature4(5,6,7)

    ! The following call is incorrect because F90 requires that
    ! positional arguments come before keyword arguments.
    ! However, OFP accepts it and ROSE fails an assertion on it.
    call g(k=0, *100)  ! assertion failure: 'keyword' is null
100 continue

    temperature4 = eoshift(donor_variability, SHIFT=1, BOUNDARY=zero, DIM=id)
end program
