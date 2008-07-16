
! Testing proc component attr spec list
! This one goes hand-in-hand with R445.
type truth
contains
	procedure, pass(beauty) :: foo
end type truth
end
