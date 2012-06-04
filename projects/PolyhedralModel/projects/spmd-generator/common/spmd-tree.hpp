
#ifndef __SPMD_TREE_HPP__
#define __SPMD_TREE_HPP__

#include "rose/Variable.hpp"

#include <vector>
#include <map>

class SgStatement;

class CommDescriptor;
class SyncDescriptor;

class Domain;
class Expression;

class ComputeSystem;

class Conditions;

class SPMD_Tree {
  protected:
    SPMD_Tree * parent;
    std::vector<SPMD_Tree *> children;

  protected:
    SPMD_Tree(SPMD_Tree * parent_);

  public:
    virtual ~SPMD_Tree();

    void appendChild(SPMD_Tree * child);
    void prependChild(SPMD_Tree * child);

    void deepDelete();

    std::vector<SPMD_Tree *> & getChildren();
    SPMD_Tree * getParent() const;

    virtual void print(std::ostream & out) const = 0;

  friend class SPMD_NativeStmt;
  friend class SPMD_Loop;
  friend class SPMD_DomainRestriction;
  friend class SPMD_KernelCall;
  friend class SPMD_Comm;
  friend class SPMD_Sync;
  friend class SPMD_Root;
};

class SPMD_Root : public SPMD_Tree {
  public:
    SPMD_Root(SPMD_Tree * parent_);
    ~SPMD_Root();

    virtual void print(std::ostream & out) const;
};

class SPMD_NativeStmt : public SPMD_Tree {
  protected:
    SgStatement * stmt;

  public:
    SPMD_NativeStmt(SPMD_Tree * parent_, SgStatement * stmt_);
    virtual ~SPMD_NativeStmt();

    SgStatement * getStatement();

    virtual void print(std::ostream & out) const;
};

class SPMD_Loop : public SPMD_Tree {
  protected:
    RoseVariable iterator;
    Domain * domain;

  public:
    SPMD_Loop(SPMD_Tree * parent_, RoseVariable iterator_, Domain * domain_);
    virtual ~SPMD_Loop();

    RoseVariable & getIterator();
    Domain * getDomain();

    virtual void print(std::ostream & out) const;

  friend class SPMD_KernelCall;
};

class SPMD_DomainRestriction : public SPMD_Tree {
  protected:
    std::vector<std::pair<Expression *, bool> > restrictions;

  public:
    SPMD_DomainRestriction(SPMD_Tree * parent_);
    virtual ~SPMD_DomainRestriction();

    void addRestriction(Expression * restriction, bool is_equality);

    std::vector<std::pair<Expression *, bool> > & getRestriction();

    virtual void print(std::ostream & out) const;

  friend class SPMD_KernelCall;
};

class SPMD_KernelCall : public SPMD_Tree {
  protected:
    std::map<RoseVariable, Domain *> iterators;
    std::vector<std::pair<Expression *, bool> > restrictions;
    std::vector<RoseVariable> ordered_iterators;
    unsigned id;

  static unsigned id_cnt;

  public:
    SPMD_KernelCall(SPMD_Tree * parent_, SPMD_Tree * first, SPMD_Tree * last);
    virtual ~SPMD_KernelCall();

    unsigned getID() const;

    std::vector<SgExpression *> * generateDimensionSizes() const;

    const std::map<RoseVariable, Domain *> & getIterators() const;
    const std::vector<std::pair<Expression *, bool> >  & getRestrictions() const;

    const std::vector<RoseVariable> & getOrderedIterators() const;

    virtual void print(std::ostream & out) const;
};

class SPMD_Comm : public SPMD_Tree {
  protected:
    CommDescriptor * comm_descriptor;
    std::vector<Conditions *> conditions;

  public:
    SPMD_Comm(SPMD_Tree * parent_, CommDescriptor * comm_descriptor_, std::vector<Conditions *> & conditons_);
    virtual ~SPMD_Comm();

    CommDescriptor * getCommDescriptor() const;
    const std::vector<Conditions *> & getConditons() const;

    virtual void print(std::ostream & out) const;
};

class SPMD_Sync :  public SPMD_Tree {
  protected:
    SyncDescriptor * sync_descriptor;

  public:
    SPMD_Sync(SPMD_Tree * parent_, SyncDescriptor * sync_descriptor_);
    virtual ~SPMD_Sync();

    virtual void print(std::ostream & out) const;
};

#endif /* __SPMD_TREE_HPP__ */

