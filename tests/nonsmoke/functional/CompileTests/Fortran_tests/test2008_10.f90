	subroutine foo
	integer na, nonzer, niter
	double precision   shift, rcond
	parameter(  na=150000,  &
	    nonzer=15,  &
	    niter=75,  &
	    shift=110.,  &
	    rcond=1.0d-1 )
	integer    nnodes_compiled
	parameter( nnodes_compiled = 64)
	integer    num_proc_cols, num_proc_rows
	parameter( num_proc_cols=8, num_proc_rows=8 )
	logical  convertdouble
	parameter (convertdouble = .false.)
	character(11) compiletime
	parameter (compiletime='30 Nov 2004')
	character(3) npbversion
	parameter (npbversion='2.3')
	character(6) cs1
	parameter (cs1='(none)')
	character(6) cs2
	parameter (cs2='(none)')
	character(6) cs3
	parameter (cs3='(none)')
	character(6) cs4
	parameter (cs4='(none)')
	character(6) cs5
	parameter (cs5='(none)')
	character(6) cs6
	parameter (cs6='(none)')
	character(6) cs7
	parameter (cs7='randi8')
	end subroutine foo
