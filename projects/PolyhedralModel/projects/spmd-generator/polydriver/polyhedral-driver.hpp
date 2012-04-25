
#ifndef __POLYHEDRAL_DRIVER_HPP__
#define __POLYHEDRAL_DRIVER_HPP__

#include "rose/Variable.hpp"

#include "common/ScopTree.hpp"

#include <map>

#include "rose.h"

#include "common/spmd-driver.hpp"

class NodePlacement;
class SPMD_Driver;
class SPMD_Tree;

typedef ScopTree_::ScopTree<SgStatement, SgExprStatement, RoseVariable> ScopTree;
typedef ScopTree_::ScopLoop<SgStatement, SgExprStatement, RoseVariable> ScopLoop;
typedef ScopTree_::ScopConditionnal<SgStatement, SgExprStatement, RoseVariable> ScopConditional;
typedef ScopTree_::ScopStatement<SgStatement, SgExprStatement, RoseVariable> ScopStatement;
typedef ScopTree_::ScopRoot<SgStatement, SgExprStatement, RoseVariable> ScopRoot;

class PolyDriver : public SPMD_Driver {
  protected:
    std::map<SPMD_Tree *, ScopTree *> trees_map;

    SPMD_Tree * convertScopToSPMD(ScopTree * scoptree, SPMD_Tree * parent = NULL);

  public:
    PolyDriver(NodePlacement * placement_);
    virtual ~PolyDriver();

    virtual SPMD_Root * parse(SgStatement * first, SgStatement * last);
    virtual SPMD_Root * transform(SPMD_Root * tree);
};

#endif /* __POLYHEDRAL_DRIVER_HPP__ */

