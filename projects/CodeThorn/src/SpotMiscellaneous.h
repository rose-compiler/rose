#ifndef SPOT_MISCELLANEOUS_H
#define SPOT_MISCELLANEOUS_H

//to-do: license, author etc.

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

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"


class SpotMiscellaneous {
public:
  static std::string spinSyntax(std::string ltlFormula); 
};

#endif 
