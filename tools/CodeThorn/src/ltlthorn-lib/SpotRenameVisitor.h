#ifdef HAVE_SPOT

#ifndef SPOT_RENAME_VISITOR_H
#define SPOT_RENAME_VISITOR_H

#include "ltlast/atomic_prop.hh"
#include "ltlast/formula.hh"
#include "ltlast/visitor.hh"
#include "ltlvisit/clone.hh"
#include "ltlenv/defaultenv.hh"

#include <unordered_map>

/*! 
 * \brief Renames the atomic propositions of an LTL formula using SPOT's visitor pattern.
 * \author Marc Jasper
 * \date 2016, 2017.
 */
class SpotRenameVisitor : public spot::ltl::clone_visitor {
public:
  SpotRenameVisitor(std::unordered_map<std::string, std::string> nameMap);

  virtual const spot::ltl::formula* recurse(const spot::ltl::formula* f);

  void visit(const spot::ltl::atomic_prop* ap);

private:
  std::unordered_map<std::string, std::string> _newAtomicPropNames;
};

#endif // SPOT_RENAME_VISITOR_H

#endif // end of "#ifdef HAVE_SPOT"
