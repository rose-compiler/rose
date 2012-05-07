
#include "polydriver/polyhedral-driver.hpp"
#include "polydriver/polyhedral-placement.hpp"
#include "polydriver/polyhedral-array-analysis.hpp"

#include "common/spmd-tree.hpp"

#include "toolboxes/algebra-container.hpp"

#include "rose/Parser.hpp"

#include "polydriver/polyhedral-utils.hpp"

#define NEED_POLYDEPS 0

ScopTree * PolyDriver::getScopTree(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, ScopTree *>::const_iterator it_trees_map = trees_map.find(tree);
  assert(it_trees_map != trees_map.end());
  ScopTree * scoptree = it_trees_map->second;
  return scoptree;
}

SPMD_Tree * PolyDriver::convertScopToSPMD(ScopTree * scoptree, SPMD_Tree * parent) {
  SPMD_Tree * res = NULL;
  if (scoptree->isRoot()) {
    res = new SPMD_Root(parent);
  }
  else if (scoptree->isLoop()) {
    ScopLoop * scoploop = (ScopLoop *)scoptree;
    int stride = scoploop->getIncrement();
    const RoseVariable & iterator = scoploop->getIterator();
    LinearDomain * domain = new LinearDomain(iterator, stride);

    std::vector<std::pair<std::map<RoseVariable, int>, int> > & lb = scoploop->getLowerBound();
    std::vector<std::pair<std::map<RoseVariable, int>, int> > & ub = scoploop->getUpperBound();


    std::vector<std::pair<std::map<RoseVariable, int>, int> >::iterator it;
    for (it = lb.begin(); it != lb.end(); it++) {
      LinearExpression * linexp = new LinearExpression(it->first);
      int div = it->second;
      domain->addLowerBound(linexp, div);
    }
    for (it = ub.begin(); it != ub.end(); it++) {
      LinearExpression * linexp = new LinearExpression(it->first);
      int div = it->second;
      domain->addUpperBound(linexp, div);
    }
 
    res = new SPMD_Loop(parent, iterator, domain);
  }
  else if (scoptree->isConditinnal()) {
    ScopConditional * scopcond = (ScopConditional *)scoptree;

    SPMD_DomainRestriction * tmp = new SPMD_DomainRestriction(parent);

    for (int i = 0; i < scopcond->getNumberOfCondition(); i++) {
      const std::vector<std::pair<RoseVariable, int> > & linvect = scopcond->getConditionLinearExpression(i);
      LinearExpression * linexp = new LinearExpression(linvect);
      tmp->addRestriction(linexp, scopcond->getConditionType(i));
    }

    res = tmp;
  }
  else if (scoptree->isStatement()) {
    ScopStatement * scopstmt = (ScopStatement *)scoptree;
    res = new SPMD_NativeStmt(parent, SageInterface::deepCopy(scopstmt->getExpression()));
  }
  else assert(false);

  assert(res != NULL);

  trees_map.insert(std::pair<SPMD_Tree *, ScopTree *>(res, scoptree));

  const std::vector<ScopTree *> & scop_children = scoptree->getChilds();
  std::vector<ScopTree *>::const_iterator it_child;
  for (it_child = scop_children.begin(); it_child != scop_children.end(); it_child++)
    res->appendChild(convertScopToSPMD(*it_child, res));

  return res;
}

PolyDriver::PolyDriver(PolyPlacement * placement_) :
  SPMD_Driver(placement_, new PolyArrayAnalysis(this)),
  trees_map(),
  dependencies(),
  dymmy_stmt(SageBuilder::buildNullStatement())
{
  placement_->setPolyDriver(this);
}

PolyDriver::~PolyDriver() {}

SPMD_Root * PolyDriver::parse(SgStatement * first, SgStatement * last) {
  ScopTree * root = new ScopRoot(dymmy_stmt);
  try {
    int cnt = 0;
    SgStatement * current = first;
    while (current != NULL) {
      cnt += PolyhedricAnnotation::Traverse<SgStatement>(current, root, cnt, dymmy_stmt);
      current = current != last ? SageInterface::getNextStatement(current) : NULL;
    }

    root->Traverse();

    PolyhedricAnnotation::PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program =
        PolyhedricAnnotation::getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(dymmy_stmt);

    polyhedral_program.finalize();

    const std::vector<SgExprStatement *> & exps = polyhedral_program.getExpressions();
    std::vector<SgExprStatement *>::const_iterator it;
    for (it = exps.begin(); it != exps.end(); it++) {
      PolyhedricAnnotation::DataAccess<SgStatement, SgExprStatement, RoseVariable> & data_access =
          PolyhedricAnnotation::getDataAccess<SgStatement, SgExprStatement, RoseVariable>(*it);
      PolyhedricAnnotation::makeAccessAnnotation<SgStatement, SgExprStatement, RoseVariable>(*it, data_access);
    }
#if NEED_POLYDEPS
    std::vector<Dependency *> * deps;
    deps = PolyhedricDependency::ComputeWaR<SgStatement, SgExprStatement, RoseVariable>(polyhedral_program, false);
    dependencies.insert(dependencies.begin(), deps->begin(), deps->end());
    delete deps;
    deps = PolyhedricDependency::ComputeRaW<SgStatement, SgExprStatement, RoseVariable>(polyhedral_program, true);
    dependencies.insert(dependencies.begin(), deps->begin(), deps->end());
    delete deps;
    deps = PolyhedricDependency::ComputeWaW<SgStatement, SgExprStatement, RoseVariable>(polyhedral_program, true);
    dependencies.insert(dependencies.begin(), deps->begin(), deps->end());
    delete deps;
#endif
  }
  catch (Exception::ExceptionBase & e) {
    e.print(std::cerr);
    exit(-1);
  }

  assert(root != NULL);

  SPMD_Root * res = dynamic_cast<SPMD_Root *>(convertScopToSPMD(root));

  assert(res != NULL);

  return res;
}

std::vector<Dependency *> * PolyDriver::getDependenciesBetween(SPMD_Tree * first, SPMD_Tree * last) const {
  PolyhedricAnnotation::PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program =
      PolyhedricAnnotation::getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(dymmy_stmt);

  std::vector<Dependency *> * res = new std::vector<Dependency *>();

  assert(false); // TODO

  return res;
}

