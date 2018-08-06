/** 
 * \file ROSE/Analysis/Template/constraints.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __ROSE_ANALYSIS_TEMPLATE_CONSTRAINTS_HPP__
#define __ROSE_ANALYSIS_TEMPLATE_CONSTRAINTS_HPP__

#include <string>
#include <set>
#include <vector>
#include <map>
#include <utility>

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE {

namespace Analysis {

namespace Template {

class Constraints : public Relation {
  protected:
    Constraints();

  public:
    virtual ~Constraints();

  protected:
    void construct();
    virtual void finalize() = 0;

    virtual std::string getGraphVizTag() const = 0;
    virtual void toGraphVizEdges(std::ostream & out) const = 0;
    virtual std::string getGraphVizLabel() const = 0;
    virtual std::string getGraphVizColor() const = 0;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class InstantiationConstraints : public Constraints {
  protected:
    TemplateInstantiation * from;
    NonrealInstantiation * to;

    std::map<SgNonrealSymbol *, SgType *> nontype_constraints; // constraints from non-type of parameter expected by the cannonical template 

  protected:
    static InstantiationConstraints * build(TemplateInstantiation * from, NonrealInstantiation * to);

  protected:
    InstantiationConstraints(TemplateInstantiation * from_, NonrealInstantiation * to_);

  public:
    virtual ~InstantiationConstraints();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class SpecializationConstraints : public Constraints {
  protected:
    NonrealInstantiation * from;
    TemplateInstantiation * to;

    size_t order;

    std::map<SgExpression *, SgExpression *> value_constraints;
    std::map<SgType *, SgType *> type_constraints;
//  std::map<SgType *, SgType *> template_constraints;

  protected:
    static SpecializationConstraints * build(NonrealInstantiation * from, TemplateInstantiation * to, size_t order);

  protected:
    SpecializationConstraints(NonrealInstantiation * from_, TemplateInstantiation * to_, size_t order_);

  public:
    virtual ~SpecializationConstraints();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_CONSTRAINTS_HPP__ */

