 ! Can we get the type of an implict declaration from the data statement?

 ! This is an error in gfortran.
 ! DATA LINES/625/, FREQ/50.0/, NAME/'PAL'/

 ! This works fine in gfortran (and ROSE).
   DATA LINES/625/, FREQ/50.0/
end
