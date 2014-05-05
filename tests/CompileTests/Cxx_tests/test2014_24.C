// ROSE bug from Chombo application code.

namespace std
   { 
  // template <typename T> class template_vector;
      class vector {};
  // typedef _vector vector;
   }

// DQ: This is causing the generated code to fail.
using namespace std;

vector a;

void breakBoxes()
   {
  // DQ (3/27/2014): using declarations are missing name qualification.
  // using std::template_vector;

     vector b;

     using std::vector;
   }
