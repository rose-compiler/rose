    program caf

    integer,allocatable::titi(:)[*]
    integer::toto(100)[*]
    team::team1, team2

    allocate(titi(100)[@team1])
    call somebody(titi[1])
    toto = titi[2@team2]
    deallocate(titi)

    end program caf
