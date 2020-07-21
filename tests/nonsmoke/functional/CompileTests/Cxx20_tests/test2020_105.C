class Point {
 int x;
 int y;
public:
 auto operator<=>(const Point&) const = default;
 // ... non-comparison functions ...
};
// compiler generates all four relational operators
Point pt1, pt2;
std::set<Point> s; // ok
s.insert(pt1); // ok
if (pt1 <= pt2) { /*...*/ } // ok, makes only a single call to <=>

