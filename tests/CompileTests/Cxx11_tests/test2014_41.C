// #include <new>
 
struct Point  {
    Point() {}
    Point(int x, int y): x_(x), y_(y) {}
    int x_, y_;
};
union U {
    int z;
    double w;
    Point p;  // Illegal in C++03; point has a non-trivial constructor.  However, this is legal in C++11.
 // U() { new( &p ) Point(); } // No nontrivial member functions are implicitly defined for a union;
                               // if required they are instead deleted to force a manual definition.
};
