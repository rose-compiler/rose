   integer stdout
   integer i,j,k
   namelist /run_parameters/ i,j,k

   i=1; j=2; k=3;

   stdout = 6
   if(.true.) write(stdout,run_parameters)

end
