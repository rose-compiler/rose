// problem code:
// preprocessor: /home/dquinlan/ROSE/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sageSupport.C:759:
//    Sg_File_Info* generateFileInfo(a_source_sequence_entry*, constructPosition): Assertion `s != __null' failed.

#include <vector>

int z = 0;

class X
   {
     public:
         void set( std::vector<double> & data )
             {
//             for (z = 0; false; z++)
                  {
                    data[z] = 0.0;
                  }
             }
   };
