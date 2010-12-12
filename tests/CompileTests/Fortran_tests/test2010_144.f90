 ! namelist /run_parameters/ numberpe,mi,mgrid,mid_theta,mtdiag,delr,delt,ulength,utime,gyroradius

   integer numberpe,mi,mgrid
   namelist /run_parameters/ numberpe,mi,mgrid
   integer stdout

 ! Error: unparsed as if(.true.) write(UNIT=stdout, FMT=run_parameters)
   if(.true.) write(stdout,run_parameters)
end
