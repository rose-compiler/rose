
// DQ (7/22/2020): EDG 6.0 fails to handle the STL set include file.
#include<set>

#if 1
class Point 
   {
     int x;
     int y;
     public:
          auto operator<=>(const Point&) const = default;
       // ... non-comparison functions ...
   };


// DQ (7/21/2020): Moved function calls into a function.
void foobar()
   {
  // compiler generates all four relational operators
     Point pt1, pt2;
     std::set<Point> s; // ok
     s.insert(pt1); // ok
     if (pt1 <= pt2) { /*...*/ } // ok, makes only a single call to <=>
   }
#endif
