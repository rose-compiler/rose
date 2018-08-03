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
  public:
    static std::map<SgNode *, Element *> all;

  protected:
    Element();

  public:
    virtual ~Element();

  protected:
    virtual void finalize() = 0;

    virtual std::string getGraphVizLabel() const = 0;
    virtual std::string getGraphVizTag() const = 0;
    virtual std::string getGraphVizShape() const = 0;
    virtual std::string getGraphVizNodeColor() const = 0;

    void toGraphViz(std::ostream & out) const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;

};

class TemplateParameterElement {
  protected:
    SgNode * node;

  protected:
    static TemplateParameterElement * build(SgNode * node);

  protected:
    TemplateParameterElement(SgNode * node__);

  public:
    virtual ~TemplateParameterElement();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizNodeColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;
};

class TemplateArgumentElement {
  protected:
    SgNode * node;

  protected:
    static TemplateArgumentElement * build(SgNode * node);

  protected:
    TemplateArgumentElement(SgNode * node__);

  public:
    virtual ~TemplateArgumentElement();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizNodeColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__ */

