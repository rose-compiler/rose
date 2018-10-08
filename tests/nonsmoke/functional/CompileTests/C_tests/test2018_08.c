
void foobar()
   {
#pragma STDC FENV_ACCESS ON
     int n = 0;

#pragma STDC FP_CONTRACT ON
#pragma STDC FP_CONTRACT OFF
     int x = 0;

#pragma STDC FP_CONTRACT OFF
     int y = 0;

   }
