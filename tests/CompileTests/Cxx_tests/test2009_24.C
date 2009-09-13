//! read gprof line-by-line performance profiling result text files
// #include "rose.h"
// #include <sstream>

#if 0
// include the right header for spirit classic
// need to check for the version, because there was a directory restructuring
// and a change of namespace
// introduced with boost 1.38
#include <boost/version.hpp>

#if BOOST_VERSION >= 103600
 #include <boost/spirit/include/classic_core.hpp>
 namespace boost_spirit = BOOST_SPIRIT_CLASSIC_NS;
#else
 #include <boost/spirit.hpp>
 namespace boost_spirit = boost::spirit;
#endif

// #include "rosehpct/gprof/gprof_info.hh"

using namespace boost_spirit;
// using namespace std;

// #include <vector>
#endif


// This is the simpler case that demonstates the bug
namespace X
   {
   }

namespace X_alias = X;

using namespace X_alias;

