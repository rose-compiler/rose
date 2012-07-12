// Name qualification bug submitted by Faizur
#include <vector>
#include <algorithm>

using namespace std;


std::vector<int> neighbors;


void test()
   {
  // These all work, only the find command appears to be an issue.
     std::vector<int>::iterator i = neighbors.begin();
     std::vector<int>::iterator j = neighbors.end();
     bool b = (i != j);

  // This is not a name qualification bug, the wrong type is used 
  // (typedefs are stripped too aggressively in the EDG/ROSE translation).
  // It appears as a name qualification bug because the previous name 
  // qualification was never applied to more than the input files (not 
  // headers).  This is a bug in ROSE, but nothing to do with name 
  // qualification.
  // std::vector< int ,std::allocator< int  > > ::iterator k = std::find< __gnu_cxx::__normal_iterator< std::vector< int ,std::allocator< int  > > ::pointer  , std::vector< int ,std::allocator< int  > > ::vector_type  >  , int  > (neighbors. begin (),neighbors. end (),(5));
     std::vector<int>::iterator k = find (neighbors.begin (), neighbors.end (), 5);

  // (find (neighbors.begin (), neighbors.end (), 5) != neighbors.end ());
   }

