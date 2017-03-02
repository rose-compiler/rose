
#include <vector>

#include "boost/foreach.hpp"
#include "boost/cstdint.hpp"

using std::vector;

namespace {

class Block 
   {
     public:
       // DQ (7/14/2014): It appears that this function is required to be defined.
       // uint64_t getSize() const;

       // DQ (3/1/2017): This is a GNU 4.8 issue that for specific versions of Boost and/or STL uint64_t is not defined).
       // So fix this by making an exception for the GNU 4.8 compiler support in ROSE.
       // This issue was identified in ROSE Matrix testing for 2/28/2017.
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ == 8))
          unsigned long getSize() const { return 0; }
#else
          uint64_t getSize() const { return 0; }
#endif
   };

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ == 8))
uint64_t 
#else
unsigned long
#endif
getNumberOfZones(const vector<Block>& blocks) 
   {
     uint64_t numZones = 0;

     BOOST_FOREACH(const Block& block, blocks) 
        {
          numZones += block.getSize();
        }

     return numZones;
   }


} // namespace

