typedef struct Extents_s {
   int imin;
   int imax;
   int jmin;
   int jmax;
   int kmin;
   int kmax;

} Extents_t ;

namespace XXX {
namespace {

bool isValidDomainSize(int domain_min, int domain_max,
        int block_min, int block_max)
{
    int domain_size = domain_max - domain_min;
    if (domain_size > 1) {
        return true;
    }

    int block_size = block_max - block_min;
    return domain_size == block_size;
}

}  // unnamed namespace, back in XXX namespace


bool isValidDomainSize(Extents_t const &domain_extents,
        Extents_t const &block_extents)
{
    bool is_valid = isValidDomainSize(domain_extents.imin,
            domain_extents.imax, block_extents.imin, block_extents.imax);
    is_valid = is_valid && isValidDomainSize(domain_extents.jmin,
            domain_extents.jmax, block_extents.jmin, block_extents.jmax);
    is_valid = is_valid && isValidDomainSize(domain_extents.kmin,
            domain_extents.kmax, block_extents.kmin, block_extents.kmax);
    return is_valid;
}

} /* namespace XXX */
