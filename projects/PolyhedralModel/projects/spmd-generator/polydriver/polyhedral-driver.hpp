
#ifndef __POLYHEDRAL_DRIVER_HPP__
#define __POLYHEDRAL_DRIVER_HPP__

#include "rose/Variable.hpp"

#include "common/ScopTree.hpp"
#include "common/PolyhedricDependency.hpp"

#include <map>
#include <vector>

#include "rose.h"

#include "common/spmd-driver.hpp"

class SPMD_Driver;
class SPMD_Tree;

typedef ScopTree_::ScopTree<SgStatement, SgExprStatement, RoseVariable> ScopTree;
typedef ScopTree_::ScopLoop<SgStatement, SgExprStatement, RoseVariable> ScopLoop;
typedef ScopTree_::ScopConditionnal<SgStatement, SgExprStatement, RoseVariable> ScopConditional;
typedef ScopTree_::ScopStatement<SgStatement, SgExprStatement, RoseVariable> ScopStatement;
typedef ScopTree_::ScopRoot<SgStatement, SgExprStatement, RoseVariable> ScopRoot;

typedef PolyhedricDependency::Dependency<SgStatement, SgExprStatement, RoseVariable> Dependency;

class PolyPlacement;

class PolyDriver : public SPMD_Driver {
  protected:
    std::map<SPMD_Tree *, ScopTree *> trees_map;
    std::vector<Dependency *> dependencies;
    SgNullStatement * dymmy_stmt;

  protected:
    SPMD_Tree * convertScopToSPMD(ScopTree * scoptree, SPMD_Tree * parent = NULL);

  public:
    PolyDriver(PolyPlacement * placement_);
    virtual ~PolyDriver();

    virtual SPMD_Root * parse(SgStatement * first, SgStatement * last);

    std::vector<Dependency *> * getDependenciesBetween(SPMD_Tree * first, SPMD_Tree * last) const;

    ScopTree * getScopTree(SPMD_Tree * tree) const;

  friend class PolyPlacement;
  friend class PolyArrayAnalysis;
};

#endif /* __POLYHEDRAL_DRIVER_HPP__ */

