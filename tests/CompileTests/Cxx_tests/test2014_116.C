typedef struct Extents_s 
   {
     int imin;
   } Extents_t ;

namespace XXX {
namespace {

bool isValidDomainSize(int domain_min)
   {
     return false;
   }

}  // unnamed namespace, back in XXX namespace


bool isValidDomainSize(Extents_t const & domain_extents)
   {
  // Bug: error: invalid initialization of reference of type 'const Extents_t&' from expression of type 'const int'
  // Unparses to: bool is_valid = XXX::isValidDomainSize(domain_extents . Extents_s::imin);
     bool is_valid = isValidDomainSize(domain_extents.imin);

     return is_valid;
   }

} /* namespace XXX */
