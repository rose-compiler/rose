
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
          uint64_t getSize() const { return 0; }
   };

uint64_t getNumberOfZones(const vector<Block>& blocks) 
   {
     uint64_t numZones = 0;

     BOOST_FOREACH(const Block& block, blocks) 
        {
          numZones += block.getSize();
        }

     return numZones;
   }


} // namespace



