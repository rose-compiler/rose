// DQ (8/29/2014): This test code fails for older compilers (e.g. g++ 4.2.4).
#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 4) )

#include <set>

namespace XXX {

class Factorization;

class RatioFactorization {};

}

// using std::set;
// using std::vector;
using XXX::Factorization;
using XXX::RatioFactorization;

namespace {

bool compareSolutions(const RatioFactorization& lhs,const RatioFactorization& rhs);

  RatioFactorization factorToRatio3D(const unsigned int num,
        const double lowRatio, const double hiRatio,
        const Factorization& factorization, unsigned int curFactorIndex,
        unsigned int factor1, unsigned int factor2, unsigned int factor3) 
   {
     RatioFactorization bestSolution;

 // This unparses to be: 
 //      std::set< class XXX::RatioFactorization  , bool (*)(const ::XXX::RatioFactorization&, const ::XXX::RatioFactorization&) , class std::allocator< class XXX::RatioFactorization  >  > 
 //      triedSolutions(set< class XXX::RatioFactorization  , bool (*)(const ::XXX::RatioFactorization&, const ::XXX::RatioFactorization&) , class std::allocator< class XXX::RatioFactorization  >  > );
 // The second reference to "set" (in "triedSolutions<>") should be "std::set"
    ::std::set<RatioFactorization, bool(*)(const RatioFactorization&,const RatioFactorization&)> triedSolutions(compareSolutions);

    return bestSolution;
   }

}

#endif
