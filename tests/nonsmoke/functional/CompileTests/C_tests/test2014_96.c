double* __attribute((orange)) global_orange_ptr = 0L;
double* __attribute((blue))   global_blue_ptr   = 0L;
double* __attribute((green))  global_green_ptr  = 0L;

void foobar()
   {
     double* __attribute((orange)) local_orange_ptr;
     double* __attribute((blue))   local_blue_ptr;
     double* __attribute((green))  local_green_ptr;

     local_orange_ptr = global_orange_ptr;
     local_blue_ptr   = global_blue_ptr;
     local_green_ptr  = global_green_ptr;
   }
