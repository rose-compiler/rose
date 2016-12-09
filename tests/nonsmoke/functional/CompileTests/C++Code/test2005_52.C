// This test code demonstrates a problem with the use of the GNU g++ 3.4 header files
// When using the g++ 3.3.2 header files it works fine and the correct code is generated
// and the case of premature exit from the namespace definition is not taken.
// While with the g++ 3.4 header files the case of premature exit from the namespace 
// definition is taken AND the generated code is incorrect (function "main" is not 
// represented in the AST correctly).

// This problem orginally occured with compiling fstream.

// Appears to be a problem if the namespace is empty
namespace std
   {
  // int x;
  // template <typename T> class X {};
   }

// using namespace std;

int main()
   {
     return 0;
   }
