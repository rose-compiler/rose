
#ifndef __POLYHEDRAL_PLACEMENT_HPP__
#define __POLYHEDRAL_PLACEMENT_HPP__

#include "rose/Variable.hpp"

#include "common/placement.hpp"

#include "maths/PPLUtils.hpp"

class PolyDriver;

class SPMD_Loop;
class SPMD_DomainRestriction;
class SPMD_KernelCall;
class SPMD_Loop;

class ComputeSystem;

class LinearExpression;

class PolyConditions : public Conditions {
  protected:
    // In the polyhedral model, the condition are linear expressions
    std::vector<std::pair<LinearExpression *, bool> > conditions;
    bool is_empty;

  protected:
    PolyConditions * simplify();
    std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> genPolyhedron() const;
    void regenFromPoly(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & polyhedron);
    void remove(const RoseVariable & it);

    PolyConditions(std::pair<Polyhedron *, std::map<unsigned, RoseVariable> *> & polyhedron);

  public:
    PolyConditions();
    PolyConditions(const PolyConditions & arg);
    virtual ~PolyConditions();

    virtual PolyConditions * new_restricted_by(SPMD_DomainRestriction * dom_restrict) const;
    virtual PolyConditions * new_restricted_by(SPMD_KernelCall * kernel_call) const;
    virtual PolyConditions * new_restricted_by(SPMD_Loop * loop, bool first_it) const;
    virtual PolyConditions * new_extended_by(SPMD_Loop * loop) const;
    virtual PolyConditions * new_extended_by(SPMD_KernelCall * kernel_call) const;
    virtual PolyConditions * new_restricted_by(Conditions * cond) const;
    virtual PolyConditions * new_without_first_it(SPMD_Loop * loop) const;

    virtual bool equal(Conditions * cond) const;

    virtual bool isTrue() const;

    virtual PolyConditions * copy() const;

    virtual SgExpression * generate() const;

    virtual void print(std::ostream & out) const;

  friend class PolyPlacement;
};

class PolyPlacement : public NodePlacement {
  protected:
    PolyDriver * polydriver;

  protected:
    virtual bool isParallel(SPMD_Loop * loop);

    virtual Conditions * genAssociatedFullCondition() const;
    virtual bool areEqual(const std::vector<Conditions *> & set1, const std::vector<Conditions *> & set2) const;
    virtual void simplify(std::vector<Conditions *> & set) const;
    virtual void removeIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_Loop * loop
    ) const;
    virtual void removeIterator(
        std::map<ArrayPartition *, std::map<ComputeSystem *, std::vector<Conditions *> > > & map, SPMD_KernelCall * kernel_call
    ) const;
    virtual void removeFromCondSet(std::vector<Conditions *> & cond_set, const Conditions * to_be_remove) const;

  public:
    PolyPlacement(ComputeSystem * cs);
    virtual ~PolyPlacement();

    void setPolyDriver(PolyDriver * polydriver_);
};

#endif /* __POLYHEDRAL_PLACEMENT_HPP__ */

