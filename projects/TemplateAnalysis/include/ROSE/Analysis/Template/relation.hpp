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

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;
};

class TemplateParameterRelation {
  protected:
    Instantiation * from;
    TemplateParameterElement * to;

  protected:
    static TemplateParameterRelation * build(Instantiation * from, TemplateParameterElement * to);

  protected:
    TemplateParameterRelation(Instantiation * from_, TemplateParameterElement * to_);

  public:
    virtual ~TemplateParameterRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateArgumentRelation;
};

class TemplateArgumentRelation {
  protected:
    Instantiation * from;
    TemplateArgumentElement * to;

  protected:
    static TemplateArgumentRelation * build(Instantiation * from, TemplateArgumentElement * to);

  protected:
    TemplateArgumentRelation(Instantiation * from_, TemplateArgumentElement * to_);

  public:
    virtual ~TemplateArgumentRelation();

  protected:
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual void toGraphVizEdges(std::ostream & out) const;
    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizColor() const;
    virtual std::string getGraphVizShape() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateParameterRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_RELATION_HPP__ */

