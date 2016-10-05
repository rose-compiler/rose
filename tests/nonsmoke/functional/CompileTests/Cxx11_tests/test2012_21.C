// Unrestricted unions

// In C++03, there are restrictions on what types of objects can be members of a union. For example, 
// unions cannot contain any objects that define a non-trivial constructor. C++11 lifts some of these 
// restrictions.[3]

// This is a simple example of a union permitted in C++:

//for placement new
#include <new>
 
struct Point  {
    Point() {}
    Point(int x, int y): x_(x), y_(y) {}
    int x_, y_;
};
union U {
    int z;
    double w;
    Point p;  // Illegal in C++03; point has a non-trivial constructor.  However, this is legal in C++11.
    U() { new( &p ) Point(); } // No nontrivial member functions are implicitly defined for a union;
                               // if required they are instead deleted to force a manual definition.
};

// The changes will not break any existing code since they only relax current rules.
