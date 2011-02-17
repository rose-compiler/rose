#include <stdio.h>

/* extern "C" int openFortranParser_main(int argc, char **argv); */
int openFortranParser_main(int argc, char **argv);

int openFortranParser_main(int argc, char **argv)
  {
 // Fortran use in ROSE is dependent upon the availablity of Jave to run the Open Fortran Parser (OFP).
    fprintf (stderr, "ROSE has not be configured to run with Fortran, use: \"--with-java\" at configure time to enable this feature (compilation of input file is ignored)\n");

 // This is not considered to be an error, ROSE will return an error code of "0" to the operating system.
 // Even though this function will return an error code of "1" to the caller.
         return 1;
  }

