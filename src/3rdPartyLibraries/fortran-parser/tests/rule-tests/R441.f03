
! Testing component attr spec list
type woohoo
    integer, kind, len :: truth
    integer, len :: beauty
    integer, pointer, dimension(3) :: ugly
    real, pointer, allocatable :: lies
    real, pointer, private :: foo
    integer, len, pointer, dimension[:] ::bar 
    integer, allocatable ::this 
    integer, allocatable, private :: that
end type
end
