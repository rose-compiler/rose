#include "stdio.h"
#include "intone.h"

void main()
   {
     int intone_nprocs_01 = 0;
     printf ("Before calling in__tone_cpus_current_(); \n");

     intone_nprocs_01 = in__tone_cpus_current_();
     printf ("intone_nprocs_01 = %d \n",intone_nprocs_01);
   }

