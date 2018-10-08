#include "rose_config.h"
#ifdef HAVE_SPOT

#include "SpotRenameVisitor.h"

using namespace spot::ltl;

SpotRenameVisitor::SpotRenameVisitor(boost::unordered_map<std::string, std::string> nameMap) : 
  _newAtomicPropNames(nameMap) {
}

const formula* SpotRenameVisitor::recurse(const formula* f) {
  f->accept(*this);
  return result_;
}
 
void SpotRenameVisitor::visit(const atomic_prop* ap) {
  std::string newName = _newAtomicPropNames[ap->name()];
  result_ = default_environment::instance().require(newName);
}

#endif // end of "#ifdef HAVE_SPOT"

