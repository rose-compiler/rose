// DQ (8/31/2004)
// This tests the use of a function defined in the "std" namespace
// Untold amount of work has gone into making it pass, including 
// the use of the new gnu 3.x headr files and fixing many problems
// in the processing of those header files within SAGE III, plus
// a lot of fixes to other parts of SAGE III (particularly namespace
// handling).

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>

using namespace std;

int foo()
   {
  // DQ (11/19/2004): The current error is that the "std::" is unparsed before the "<<" operator

  // (((class std::basic_ostream< char , std::char_traits< char >  >  &)std::cerr)std::<<("test use of cerr defined in the namespace"))<<(std::endl);
  // cerr << "test use of cerr defined in the namespace" << endl;

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif


