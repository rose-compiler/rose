// In a problem related to test2005_141.C from the following code:
//      const Face &FBC = *(S.faceBegin() + fid);
// we generated:
//      const Face &FBC = *(S.faceBegin())+fid;
// where "fid" is of integer type

#include <vector>
#include <list>
#include <map>
#include <string>

using namespace std;

vector<int>::iterator getIterator( const vector<int> & X );


class myVector
   {
     public:
          const myVector & operator*() const;

          myVector();

       // Presence of explicit copy constructor causes code generation to be "value = b-c.norm();"
          myVector (const class myVector &x);

       // An explicit operator= appears to have no effect!
       // myVector operator= (const class vector &x) const;
   };

double min (double x, double y);

void foo()
   {
     myVector *a = NULL;

  // Problem code
     myVector & b = *(a + 1);

     int offset;

#if 0
     vector<int> x;

  // using iterators
  // int & value_x = *(x.begin() + 1);
     int & value_x = *(x.begin() + offset);

     int & value_x2 = *(getIterator(x) + offset);
#endif

#if 0
     list<int> y;

  // using iterators
  // int & value_y = *(y.begin() + offset);
     int & value_y = *(y.begin());

     map<int,string> z;

  // using iterators
  // pair<int,string> & value_z = *(z.begin() + offset);
     const pair<int,string> & value_z = *(z.begin());
#endif
   }

