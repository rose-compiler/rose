#include <string>
#include <cstring>
#include <iostream>

//#include "test-01.h"

#ifndef __test_01__
#define __test_01__

#include <string>

namespace SAMRAI {
   namespace tbox {
      class ParallelBuffer
         {
         public:
            ParallelBuffer();
         private:
            std::string hello;
         };
   };
};


#endif //__test_01__


namespace SAMRAI {
   namespace tbox {
      ParallelBuffer::ParallelBuffer()
      {
         hello = std::string();
      }
   }
}

int main(int argc, char** argv)
{
   std::cout << "Howdy!" << std::endl;
}
