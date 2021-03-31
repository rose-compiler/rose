#include <stdio.h>

/* extern "C" int openFortranParser_main(int argc, char **argv); */
int experimental_openFortranParser_main(int argc, char **argv);

int experimental_openFortranParser_main(int argc, char **argv)
   {
  // Fortran use in ROSE is dependent upon the availablity of Jave to run the Open Fortran Parser (OFP).
     fprintf (stderr, "ROSE has not be configured to run with Fortran, use: \"--with-java\" at configure time to enable this feature (compilation of input file is ignored)\n");

     printf ("ERROR: This function is defined in dummy_openFortranParser_main.C of the src/frontend/Experimental_OpenFortranParser_ROSE_Connection directory (the experimental fortran frontend has not been enabled) \n");
     ROSE_ABORT();
   }
