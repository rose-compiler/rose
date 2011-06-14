    program caf

    integer,allocatable::titi(:)[*]
    double precision::toto(100)[*]
    team team1, team2
    titi = toto

    with team  team2
    allocate(titi(100)[@team1])
    end with team

    titi = toto

    end program caf
