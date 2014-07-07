#include "boost/shared_ptr.hpp"

typedef int LocalId;

class Patch
   {
     public:
          const LocalId& getLocalId() const;
   };

class PatchLevel
   {
     public:
          class Iterator
             {
               public:
                    Iterator();
                    const boost::shared_ptr<Patch>& operator->() const;           
             };

          typedef Iterator iterator;
   };

void Refine_postProcessFluxLimiter( double phi_floor )
   {
     PatchLevel::iterator pi;

     int mbid = pi->getLocalId();
   }


