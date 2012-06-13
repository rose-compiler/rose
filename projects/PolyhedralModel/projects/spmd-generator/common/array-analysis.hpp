
#ifndef __ARRAY_ANALYSIS_HPP__
#define __ARRAY_ANALYSIS_HPP__

#include "rose/Variable.hpp"

#include <utility>
#include <vector>
#include <map>
#include <set>

class SPMD_Tree;
class SPMD_Root;
class SPMD_KernelCall;

class CommDescriptor;
class SyncDescriptor;
class NodePlacement;

class SgType;
class SgExpression;
class SgPntrArrRefExp;
class SgVarRefExp;

// Base type for Array analysis
// it doesn't consider actual partition
class ArrayPartition {
  protected:
    RoseVariable original_array;
    std::vector<unsigned> dimensions;
    SgType * type;

    SgExpression * size;

  public:
    ArrayPartition(RoseVariable & array_);
    ArrayPartition(RoseVariable & array_, std::vector<unsigned> & dimensions_, SgType * type_);
    virtual ~ArrayPartition();

    virtual std::string getUniqueName() const;

    const RoseVariable & getOriginalVariable() const;
    const std::vector<unsigned> & getDimensions() const;
    SgType * getType() const;

    SgExpression * getSize() const;

  static ArrayPartition * merge(ArrayPartition * p1, ArrayPartition * p2);
};

class ArrayAlias {
  protected:
    ArrayPartition * original_array;

  public:
    ArrayAlias(ArrayPartition * original_array_);
    virtual ~ArrayAlias();

    SgExpression * propagate(SgExpression * exp) const;

    virtual SgPntrArrRefExp * propagateArr(SgPntrArrRefExp * arr_ref) const = 0;
    virtual SgVarRefExp * propagateVar(SgVarRefExp * var_ref) const = 0;
    virtual SgInitializedName * getInitName() const = 0;
};

class ArrayAnalysis {
  protected:
    std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > accesses_map;

  public:
    ArrayAnalysis();
    virtual ~ArrayAnalysis();

    virtual void process(SPMD_Root * tree) = 0;

    void update(SPMD_KernelCall * new_node, const std::vector<SPMD_Tree *> & create_from);

    virtual std::set<CommDescriptor *> genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) = 0;
    virtual std::set<SyncDescriptor *> genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) = 0;

    const std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > & get(SPMD_Tree * tree) const;

    std::set<ArrayPartition *> * get_in(SPMD_Tree * tree) const;
    std::set<ArrayPartition *> * get_out(SPMD_Tree * tree) const;
    std::set<ArrayPartition *> * get_inout(SPMD_Tree * tree) const;
    std::set<ArrayPartition *> * get_partitions(SPMD_Tree * tree) const;

    virtual void clear();
};

#endif /* __ARRAY_ANALYSIS_HPP__ */

