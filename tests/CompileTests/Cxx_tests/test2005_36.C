// Original Code:
//      copy(data.begin(), data.end(), ostream_iterator<string>(output, "\n"));
// Generated Code:
//      (std::copy <  > (data.begin(),data.end(),(output,("\n"))));

// Should generate:
//      (std::copy <  > (data.begin(),data.end(),ostream_iterator<string>(output,("\n"))));


// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include<string>
#include<vector>

#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

void foo( const vector<string> & data )
   {
     ofstream output;
     copy(data.begin(), data.end(), ostream_iterator<string>(output, "\n"));
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

