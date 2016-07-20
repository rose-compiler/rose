
#ifndef SPOT_RENAME_VISITOR_H
#define SPOT_RENAME_VISITOR_H

#include "ltlast/atomic_prop.hh"
#include "ltlast/formula.hh"
#include "ltlast/visitor.hh"
#include "ltlvisit/clone.hh"
#include "ltlenv/defaultenv.hh"

#include "boost/unordered_map.hpp"

using namespace spot::ltl;

class SpotRenameVisitor : public clone_visitor {
public:
  SpotRenameVisitor(boost::unordered_map<std::string, std::string> nameMap);

  virtual const formula* recurse(const formula* f);

  void visit(const atomic_prop* ap);

private:
  boost::unordered_map<std::string, std::string> _newAtomicPropNames;
};

#endif // SPOT_RENAME_VISITOR_H
