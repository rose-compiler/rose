/** 
 * \file ROSE/Analysis/Template/relation.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__
#define __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__

#include <string>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <iostream>

#include "ROSE/Analysis/Template/instantiation-flow.hpp"

namespace ROSE {

namespace Analysis {

namespace Template {

class Relation {
  public:
    static std::map<std::pair<Element *, Element *>, Relation *> all;

  protected:
    Relation();

  public:
    virtual ~Relation();

  protected:
    void construct();
    virtual void finalize() = 0;

    void toGraphViz(std::ostream & out) const;

    virtual std::string getGraphVizTag() const = 0;
    virtual void toGraphVizEdges(std::ostream & out) const = 0;
    virtual std::string getGraphVizLabel() const = 0;
    virtual std::string getGraphVizColor() const = 0;
    virtual std::string getGraphVizShape() const = 0;
    virtual std::string getGraphVizStyle() const = 0;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;
};

class TemplateRelation : public Relation {
  public:
    typedef enum {
      e_nontype_parameter,
      e_type_parameter,
      e_template_parameter,
      e_nontype_argument,
      e_type_argument,
      e_template_argument,
      e_pack_expansion_argument
    } kind_e;

  protected:
    Instantiation * from;
    Element * to;
    kind_e kind;
    size_t position;

  protected:
    static TemplateRelation * build(Instantiation * from, Element * to, kind_e kind, size_t pos);

  protected:
    TemplateRelation(Instantiation * from_, Element * to_, kind_e kind_, size_t pos_);

  public:
    virtual ~TemplateRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class CannonicalRelation : public Relation {
  protected:
    TemplateInstantiation * from;
    TemplateInstantiation * to;

  protected:
    static CannonicalRelation * build(TemplateInstantiation * from, TemplateInstantiation * to);

  protected:
    CannonicalRelation(TemplateInstantiation * from_, TemplateInstantiation * to_);

  public:
    virtual ~CannonicalRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class BaseTypeRelation : public Relation {
  protected:
    Element * from;
    Element * to;

  protected:
    static BaseTypeRelation * build(Element * from, Element * to);

  protected:
    BaseTypeRelation(Element * from_, Element * to_);

  public:
    virtual ~BaseTypeRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
};

class TypeOfRelation : public Relation {
  protected:
    Element * from;
    Element * to;

  protected:
    static TypeOfRelation * build(Element * from, Element * to);

  protected:
    TypeOfRelation(Element * from_, Element * to_);

  public:
    virtual ~TypeOfRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class BaseTypeRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__ */

