// Skip testing on 64 bit systems for now!
// #ifndef __LP64__

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

#include "test2006_42.h"

//Check that we cover empty constructor block
Bar::Bar() {} /***Cov bb*/ /***Cov me*/

int Bar::moo() { /***Cov bb*/ /***Cov me*/
   try { /***Cov bb*/
      cout<<"Bar::moo()"<<endl;
      return 1;
   } catch(...) { /***Cov bb ~ */
      return 2;
   }
}

extern "C"
void bar() { /***Cov bb*/ /***Cov me*/
   Bar b;
   b.bar();
   b.moo();
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

// #else
//   #warning "Not tested on 64 bit systems"
// #endif

