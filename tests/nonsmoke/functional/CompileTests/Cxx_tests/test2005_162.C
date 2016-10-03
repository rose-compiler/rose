// Test problem with setprecision between g++ 3.3.2 and g++ 3.4.3.
// The other point that Tom made was that we might not want to output
// values using std::ios::showpoint since it would not capture
// small numbers (Avagadro's number, for example); scientific notation 
// would be better.  This was not done initially since it would not
// permit us to accurately count the characters output (required for 
// the GUI interface stuff to work).

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
#include <iomanip>
#include <sstream>

int
main(int argc, char **argv)
   {
     const double a = 235;
     std::ostringstream out;
     std::cout << std::setiosflags(std::ios::showpoint) << std::setprecision(24) << a << std::endl;
     out << std::setiosflags(std::ios::showpoint) << std::setprecision(24) << a ;
     std::cout << out.str() << std::endl;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

