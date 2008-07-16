

! Testing binding attr list.
type truth
contains
	procedure, nopass, deferred :: foo
	procedure, private, nopass :: beauty
	procedure, non_overridable :: ugly
	procedure, pass :: lies
	procedure, pass (woohoo) :: bar
	procedure, public :: that
	procedure, private :: theother
end type truth
end
