#if 0
// Original Code:
   vector<int>::const_iterator beginningOfZ = 
      find_if(mValidTransportZAs.begin(), 
              mValidTransportZAs.end(), 
              bind2nd(greater<int>(), nextLowestZA));

// Generated Code:
  std::vector < int , std::allocator< int > > ::const_iterator beginningOfZ = 
          (std::find_if <> ((this) -> mValidTransportZAs.begin(),(this) -> mValidTransportZAs.end(),std::bind2nd <> (std::greater <int> ,nextLowestZA)));
// Should be generated as:
  std::vector < int , std::allocator< int > > ::const_iterator beginningOfZ = 
          (std::find_if <> ((this) -> mValidTransportZAs.begin(),(this) -> mValidTransportZAs.end(),std::bind2nd <> (std::greater <int>*()* ,nextLowestZA)));
#endif

#include<functional>
#include<algorithm>
#include<vector>

void foo()
   {
     std::greater<int> X();

     std::bind2nd(std::greater<int>(), 42);
   }


