class Extents_t
   {
     public:
          int imin;
   };

class Outer_Extents_t : public Extents_t
   {
     public:
          int imin;
  // Extents_t ex;
   };

// namespace XXX {
namespace {

bool isValidDomainSize(int domain_min)
   {
     return false;
   }

}  // unnamed namespace, back in XXX namespace

void isValidDomainSize(Outer_Extents_t const & domain_extents)
   {
  // Bug: error: invalid initialization of reference of type 'const Extents_t&' from expression of type 'const int'
  // Unparses to: bool is_valid = XXX::isValidDomainSize(domain_extents . Extents_s::imin);
  // isValidDomainSize(domain_extents.imin);
     isValidDomainSize(domain_extents.Extents_t::imin);
   }

// } /* namespace XXX */
