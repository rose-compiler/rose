#include <stdio.h>

/* extern "C" int openFortranParser_main(int argc, char **argv); */
int openFortranParser_main(int argc, char **argv);

int openFortranParser_main(int argc, char **argv)
  {
    fprintf (stderr, "ROSE has not be configured to run with Fortran -- use --with-java at configure time to enable this feature\n");
 /* ROSE_ASSERT(0); */

	 return 1;
  }

