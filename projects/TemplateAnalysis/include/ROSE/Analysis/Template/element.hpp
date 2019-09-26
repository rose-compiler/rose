/** 
 * \file ROSE/Analysis/Template/element.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __ROSE_ANALYSIS_TEMPLATE_ELEMENT_HPP__
#define __ROSE_ANALYSIS_TEMPLATE_ELEMENT_HPP__

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

class Element {
  protected:
    SgNode * node;

  public:
    static std::map<SgNode *, Element *> all;

    static Element * get(SgNode *);

  protected:
    Element(SgNode * node__);

  public:
    virtual ~Element();

    virtual std::string getKind() const = 0;

  protected:
    virtual void finalize() = 0;

    virtual std::string getGraphVizLabel() const = 0;
    virtual std::string getGraphVizTag() const = 0;
    virtual std::string getGraphVizShape() const = 0;
    virtual std::string getGraphVizStyle() const = 0;
    virtual std::string getGraphVizNodeColor() const = 0;
    virtual size_t getGraphVizNodePenWidth() const = 0;
    virtual std::string getGraphVizFillColor() const;

    void toGraphViz(std::ostream & out) const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;

};

class TemplateElement : public Element {
  protected:
    static TemplateElement * build(SgNode * node);

  protected:
    TemplateElement(SgNode * node__);

  public:
    virtual ~TemplateElement();

    virtual std::string getKind() const;

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizStyle() const;
    virtual std::string getGraphVizNodeColor() const;
    virtual size_t getGraphVizNodePenWidth() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__ */

