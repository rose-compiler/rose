   integer stdout
   integer i,j,k
   namelist /run_parameters/ i,j,k

 ! Error: unparsed as if(.true.) write(UNIT=stdout, FMT=run_parameters)
 ! and the use of "UNIT=" is a problem (error) for gfortran version 4.2.4
   if(.true.) write(stdout,run_parameters)
end
