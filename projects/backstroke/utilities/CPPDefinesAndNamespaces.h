/** Do not include this file from a header file! It defines things which might pollute the global namespace! */

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH
