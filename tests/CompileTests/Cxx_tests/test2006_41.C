
// DQ (12/4/2006): I think that this issue is specific to 3.4.x and not anything specific to 64 bit issues.
// Skip testing on 64 bit systems for now!
// #ifndef __LP64__

namespace A
   {
     void foo();
   }

namespace A
   {
     void foo();
   }

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
// Skip version 3.4.x gnu compilers
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )

namespace A
   {
     void foo() {};
   }

#else
// #warning "Not tested on gnu 4.0 or greater versions"
  #warning "Case not tested for version 3.4 and higher."
#endif

// #else
//   #warning "Not tested on 64 bit systems"
// #endif

