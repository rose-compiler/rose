#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef SPOT_RENAME_VISITOR_H
#define SPOT_RENAME_VISITOR_H

#include "ltlast/atomic_prop.hh"
#include "ltlast/formula.hh"
#include "ltlast/visitor.hh"
#include "ltlvisit/clone.hh"
#include "ltlenv/defaultenv.hh"

#include "boost/unordered_map.hpp"

using namespace spot::ltl;

/*! 
 * \brief Renames the atomic propositions of an LTL formula using SPOT's visitor pattern.
 * \author Marc Jasper
 * \date 2016, 2017.
 */
class SpotRenameVisitor : public clone_visitor {
public:
  SpotRenameVisitor(boost::unordered_map<std::string, std::string> nameMap);

  virtual const formula* recurse(const formula* f);

  void visit(const atomic_prop* ap);

private:
  boost::unordered_map<std::string, std::string> _newAtomicPropNames;
};

#endif // SPOT_RENAME_VISITOR_H

#endif // end of "#ifdef HAVE_SPOT"
