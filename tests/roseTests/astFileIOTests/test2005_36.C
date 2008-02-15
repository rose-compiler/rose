// Original Code:
//      copy(data.begin(), data.end(), ostream_iterator<string>(output, "\n"));
// Generated Code:
//      (std::copy <  > (data.begin(),data.end(),(output,("\n"))));

// Should generate:
//      (std::copy <  > (data.begin(),data.end(),ostream_iterator<string>(output,("\n"))));


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
