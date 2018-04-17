
// Example of namespace using declaration to test new aliasing symbol support.

// using namespace std;

namespace X
   {
     int x;
     void foobar();
   }

int main()
   {
     using namespace X;

     return x;
   }
