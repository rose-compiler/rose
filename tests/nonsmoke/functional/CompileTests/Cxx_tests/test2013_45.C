
#include<vector>

using namespace std;

void
foo()
   {
     vector<int> x;

     vector<int>::iterator a,b,c;

  // This is unparsed as:
  //      x. insert < __normal_iterator< pointer , vector_type > >  (a,b,c);
  // And the __normal_iterator is not qualified.
     x.insert(a,b,c);
   }
