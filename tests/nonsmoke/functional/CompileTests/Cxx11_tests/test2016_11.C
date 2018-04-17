
// Testing the C++11 initialization list support in ROSE.

#include <vector>

class XYZ { public: XYZ(int x); };

std::vector<XYZ> abc = { 1, 2, 3 };

