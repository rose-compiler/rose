typedef struct Extents_s 
   {
     int imin;
     int imax;
   } Extents_t ;

namespace XXX {
namespace {

bool isValidDomainSize(int domain_min, int domain_max,int block_min, int block_max)
   {
     return false;
   }

}  // unnamed namespace, back in XXX namespace


bool isValidDomainSize(Extents_t const &domain_extents,Extents_t const &block_extents)
   {
     bool is_valid = isValidDomainSize(domain_extents.imin,domain_extents.imax, block_extents.imin, block_extents.imax);
     return is_valid;
   }

} /* namespace XXX */
