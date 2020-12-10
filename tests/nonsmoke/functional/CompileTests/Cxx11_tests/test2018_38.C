
// DQ (1/24/2017): Added declaration of this type that comes from the rose_edg_required_macros_and_functions.h file.
// Note that this is only required within ROSE is using the GNU 5.1 compiler.  Not yet clear why that is.
// typedef unsigned int __mmask32;

template <typename S, typename T>
class ParseAndSetMember
{};

template <typename S, typename T>
ParseAndSetMember<S,T> createParseAndSetMember( T S::*mptr)
{}

// DQ (7/23/2020): GNU 10 STL requires that we include the STL string header file explicitly.
#include <string>

#include <map>
namespace LEOS {
    enum functionsType {
        FT_Bcv,  
        FT_Cs,
        FT_Bltemp
    };
    typedef unsigned int L8UINT;
    typedef L8UINT functionType_t;
    const functionType_t FT_LEOS(FT_Bltemp + 1);
    typedef std::string  L8STRING;
    using std::map;
    typedef map<functionType_t, L8STRING> genericNamesMap_t;

#if 1
    //BELOW NEEDED FOR ERROR
    const genericNamesMap_t::value_type genericNamesData[] = {
            //IF STRINGS ARE NULL BELOW, FAILS DIFFERENTLY:
            genericNamesMap_t::value_type(FT_Bcv,    "curve name: Phase_Curve"),
    };
#endif

}
