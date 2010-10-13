/** Do not include this file from a header file! It defines things which might pollute the global namespace! */

#include <boost/foreach.hpp>

// It's better not to put the following statements into header files (or else we have to make sure this header
// file is included at last).
//using namespace boost;
//using namespace std;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH
