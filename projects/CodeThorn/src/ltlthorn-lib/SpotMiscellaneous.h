#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef SPOT_MISCELLANEOUS_H
#define SPOT_MISCELLANEOUS_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>

//SPOT includes
#include "ltlparse/public.hh"
#include "ltlvisit/destroy.hh"
#include "tgba/tgbaexplicit.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaparse/public.hh"
#include "tgbaalgos/save.hh"
#include "ltlast/allnodes.hh"
#include "tgbaalgos/scc.hh"
#include "tgbaalgos/cutscc.hh"
#include "ltlparse/ltlfile.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"
#include "misc/timer.hh"
#include "ltlast/formula.hh"
#include "tgbaalgos/replayrun.hh"
#include "tgbaalgos/projrun.hh"
#include "ltlvisit/simplify.hh"
#include "ltlvisit/relabel.hh"
//#include "ltlast/atomic_prop.hh"

// custom SPOT extensions
#include "SpotRenameVisitor.h"

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"

/*! 
 * \brief Miscellaneous functions that rely on implementations of the SPOT library.
 * \author Marc Jasper
 * \date 2016, 2017.
 */
class SpotMiscellaneous {
public:
  static std::string spinSyntax(std::string ltlFormula); 
};

#endif 

#endif // end of "#ifdef HAVE_SPOT"
