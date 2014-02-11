#define ER_LAST_ERROR -1132

void foobar()
   {
  // The problem is tha this is unparsed as: 
  // int ignore_error_list[--1132];
  // which implies the use of the decrement operator.
  // int ignore_error_list[-ER_LAST_ERROR];
     int ignore_error_list[-ER_LAST_ERROR];

     int x;
     x = - -x;
     x = - -5;
     x = -ER_LAST_ERROR;

     int y[- -5];
   }
