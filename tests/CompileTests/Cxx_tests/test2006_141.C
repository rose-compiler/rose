
// Include some file that will force lots of stuff to be included
#include<string>
// But if we define size_t directory then it is an error to references it via the std namespace!
// typedef unsigned int size_t;

// Plum hall thinks this is a valid C++ type!
std:: size_t size1;
static std:: size_t size1_static;

size_t size2;

class X
   {
     public:
          std:: size_t sizeX;
   };


