#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef SPOT_STATE_H
#define SPOT_STATE_H

#include <cassert>

//CodeThorn includes
#include "codethorn.h"
#include "EState.h"
#include "SpotTgba.h"
#include "SpotSuccIter.h"

//SPOT includes
#include "tgba/tgba.hh"
#include "tgba/succiter.hh"
#include "ltlvisit/apcollect.hh"
#include "tgba/bdddict.hh"
#include "bdd.h"
#include "tgba/state.hh"
#include "tgba/succiter.hh"

using namespace std;

namespace CodeThorn {
  /*! 
   * \brief A state in the SPOT tgba. Uses an EState from Codethorn as its adaptee (see SpotConnection).
   * \author Marc Jasper
   * \date 2014, 2015, 2016, 2017.
   */
  class SpotState : public spot::state {
  public:
    SpotState(const EState& estate);
    //returns a negative value if "this" is smaller than "other", 0 if they are the same and
    // a value greater zero if "this" is comparably greater than "other"
    int compare(const spot::state* other) const;
    //returns a hash value for this state
    size_t hash() const;
    spot::state* clone() const;
    //use destroy instead of the destructor. This fucntion is implemented in order to adhere to SPOT's interface.
    // currently only calls the destructor.
    void destroy() const;
    std::string toString() const;
    const EState& getEState() const;

  private:
    //the EState object from CodeThorn encapsulated by this adapter
    const EState& estate;
  };
}  // end of namespace CodeThorn

#endif

#endif // end of "#ifdef HAVE_SPOT"
