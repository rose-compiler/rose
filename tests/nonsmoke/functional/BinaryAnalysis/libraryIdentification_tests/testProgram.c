// #include<stdio.h>

// int this_is_a_variable = 0xDeadBeef;
int this_is_a_variable = 0x01234567;

int
main()
   {
     int another_variable = this_is_a_variable + 7;
#if 1
     int x = 7;
     int y,z;
     y = x;
     z = y;
#endif

  // printf ("Exiting");

     return another_variable;
   }
