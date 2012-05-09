
#ifndef __PLACEMENT_HPP_
#define __PLACEMENT_HPP_

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <iostream>

class SPMD_Tree;
class SPMD_Root;
class SPMD_Loop;
class SPMD_KernelCall;
class SPMD_DomainRestriction;

class ComputeSystem;
class System;
class Domain;

class ArrayAnalysis;
class ArrayPartition;;

class SgExpression;

class Conditions {
  public:
    Conditions();
    Conditions(const Conditions & arg);
    virtual ~Conditions();

    virtual Conditions * new_restricted_by(SPMD_DomainRestriction * dom_restrict) const = 0;
    virtual Conditions * new_restricted_by(SPMD_KernelCall * kernel_call) const = 0;
    virtual Conditions * new_restricted_by(SPMD_Loop * loop, bool first_it) const = 0;
    virtual Conditions * new_extended_by(SPMD_Loop * loop) const = 0;
    virtual Conditions * new_extended_by(SPMD_KernelCall * kernel_call) const = 0;
    virtual Conditions * new_restricted_by(Conditions * cond) const = 0;
    virtual Conditions * new_without_first_it(SPMD_Loop * loop) const = 0;

    virtual bool equal(Conditions * cond) const = 0;

    virtual bool isTrue() const = 0;

    virtual Conditions * copy() const = 0;

    virtual SgExpression * generate() const = 0;

    virtual void print(std::ostream & out) const = 0;
};

class NodePlacement {
  protected:
    ComputeSystem * system;

  protected:
    virtual void simplify(std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map) const;
    virtual void merge(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_inout,
        const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_in
    ) const;
    virtual bool diff(
        const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map1,
        const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map2
    ) const;
    virtual void deepCopy(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_out,
        const std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map_in
    ) const;
    virtual void flush(
        SPMD_Tree * tree, std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_datas
    ) const; 
    virtual void setFirstIt(std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_Loop * loop) const;
    virtual void setLastIt(std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_Loop * loop) const;
    virtual void setNextIt(std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_Loop * loop) const;

    virtual void restrictToFirstIt(std::vector<Conditions *> conds, SPMD_Loop * loop) const;
    virtual void restrictToNext(std::vector<Conditions *> conds, SPMD_Loop * loop) const;

    virtual void collectAliasing(
        SPMD_Root * root, ArrayAnalysis & array_analysis,
        std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
    ) const; 

    virtual void generateCommSync(SPMD_Root * root, ArrayAnalysis & array_analysis) const;

    virtual void addIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
        SPMD_Loop * loop
    ) const;

    virtual void addIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
        SPMD_KernelCall * kernel_call
    ) const;

    virtual void restrictIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
        SPMD_DomainRestriction * domain_restriction
    ) const;

    virtual void restrictIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map,
        SPMD_KernelCall * kernel_call
    ) const;

    void placeCommSync(
        SPMD_Tree * tree,
        ArrayAnalysis & array_analysis,
        const std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_data,
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > original_position,
        Conditions * condition_to_reach_this_node
    ) const;

    virtual void findSources(
        ComputeSystem * destination,
        const std::map<ComputeSystem *, std::vector<Conditions *> > & current_position,
        std::vector<std::pair<std::vector<Conditions *>, ComputeSystem *> > & results_sources,
        Conditions * context
    ) const;

    void positionValidData(
        SPMD_Tree * tree,
        ArrayAnalysis & array_analysis,
        std::map<SPMD_Tree *, std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > > & valid_datas,
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > previous_position,
        Conditions * condition_to_reach_this_node
    ) const;

    virtual Conditions * genAssociatedFullCondition() const = 0;
    virtual bool areEqual(const std::vector<Conditions *> & set1, const std::vector<Conditions *> & set2) const = 0;
    virtual void simplify(std::vector<Conditions *> & set) const = 0;
    virtual void removeIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_Loop * loop
    ) const = 0;
    virtual void removeIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_KernelCall * kernel_call
    ) const = 0;
    virtual void removeFromCondSet(std::vector<Conditions *> & cond_set, const Conditions * to_be_remove) const = 0;
    virtual void removeFromCondSet(std::vector<Conditions *> & cond_set_1, const std::vector<Conditions *> & cond_set_2) const;

  public:
    NodePlacement(ComputeSystem * system_);
    virtual ~NodePlacement();

    virtual void place(SPMD_Root * root,
      ArrayAnalysis & array_analysis,
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
    ) = 0;
    virtual Domain * onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const = 0;
    virtual ComputeSystem * assigned(SPMD_Tree * tree) const = 0;

    virtual void clear();
};

#endif /* __PLACEMENT_HPP_ */

