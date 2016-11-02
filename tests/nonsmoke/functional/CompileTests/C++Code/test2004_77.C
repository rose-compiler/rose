
// This is not required since I have added it to ROSE directly
// The whole issue of how to support __restrict__ needs to be
// visited since I can't get EDG to recognize it (though it says
// it is supported since 8/2002 and I have turned on all the required
// support (I think).
// #define __restrict__ 

#include<list>
#include<vector>
#include<map>

using namespace std;

int main()
   {
  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
     list<int> integerList;

     integerList.push_back(1);

     int sumOverList = 0;
     for (list<int>::iterator i = integerList.begin(); i != integerList.end(); i++)
        {
          sumOverList += *i;
        }

     integerList.sort();

     vector<int> integerVector;

     integerVector.push_back(1);
     int sumOverVector = 0;

  // This generates very strange code:
  // for (std::vector< int , std::allocator< int >  > ::iterator i = (integerVector.begin()); i__gnu_cxx::!=((integerVector.end())); i++)
     for (vector<int>::iterator i = integerVector.begin(); i != integerVector.end(); i++)
        {
          sumOverVector += *i;
        }

  // Try to use a map to draw out error in compiling stl_map.h (error in processing Kull)
     map<int,int> xmap;

#if 0
  // This generates an error in processing ROSE generated code using backend (g++)
     xmap.insert(1,1);
#endif

     return 0;
   }


