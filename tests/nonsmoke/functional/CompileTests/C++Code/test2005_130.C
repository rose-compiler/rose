// problem code:

#include <vector>

class X
   {
     public:
          void set( std::vector<double> & data ) const
             {
#if 1
            // int nz = data.size();
               int nz = 1;
               for (int z = 0; z < nz; z++)
                  {
                    data[z] = 0.0;
                  }
#endif
             }
   };

