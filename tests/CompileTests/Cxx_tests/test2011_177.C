
namespace X 
   {
     int var_A;
#if 0
     inline long abs(long __i) { return 0; }
#endif
   }

#if 1
namespace X 
   {
     int var_B = var_A;
#if 0
     inline long div(long __i, long __j) { return 0; }   
#endif
   }
#endif


