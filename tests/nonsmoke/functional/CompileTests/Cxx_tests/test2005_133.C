
#include "test2005_133.h"

#define WORK_AROUND 0

class X
   {
     public:
      // Works correctly when defined outside of the class
         X(std::vector<double>& groupBottoms)
             {
#if WORK_AROUND
                  {
#endif
                 // sort(groupBottoms.begin(), groupBottoms.end());
                 // groupBottoms.begin(); // fails
                 // groupBottoms[1];      // eats "int abc; int xyz;"
                 // groupBottoms.size();  // eats "int abc;"
                    groupBottoms.begin();
#if WORK_AROUND
                  }
#endif
#if 1
               int abc;
               int xyz;
#endif
            // for (int i = 0; i != groupBottoms.size(); i++) { }
             }
   };



