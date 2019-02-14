/** 
 * \file ROSE/Analysis/Template/instantiation.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_HPP__
#define __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_HPP__

#include <string>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <iostream>

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE {

namespace Analysis {

namespace Template {

class Instantiation : public Element {
  public:
    typedef std::map < SgNonrealSymbol *, std::string> field_map_t;

    typedef std::pair< size_t, SgType *>     nontype_param_info_t;
    typedef std::pair< size_t, field_map_t > type_param_info_t;
    typedef std::pair< size_t, void * >      template_param_info_t; // TODO

    typedef std::pair< size_t, void * > nontype_arg_info_t;  // TODO
    typedef std::pair< size_t, void * > type_arg_info_t;     // TODO
    typedef std::pair< size_t, void * > template_arg_info_t; // TODO

  protected:
    SgSymbol * symbol;
    TemplateInstantiation * cannonical;

    SgDeclarationScope * nonreal_scope;

    std::map<SgSymbol *, nontype_param_info_t  > nontype_parameters;
    std::map<SgSymbol *, type_param_info_t     > type_parameters;
    std::map<SgSymbol *, template_param_info_t > template_parameters;
    std::vector<TemplateElement *> parameters_map;
    std::vector<TemplateRelation::kind_e> parameters_kind;

    std::map<SgExpression *, nontype_arg_info_t  > nontype_arguments;
    std::map<SgType *,       type_arg_info_t     > type_arguments;
    std::map<SgSymbol *,     template_arg_info_t > template_arguments;
    std::vector<Element *> arguments_map;
    std::vector<TemplateRelation::kind_e> arguments_kind;

  public:
    Instantiation(SgSymbol * sym);
    virtual ~Instantiation();

    virtual std::string getKind() const = 0;
    
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizStyle() const;
    virtual std::string getGraphVizTag() const = 0;
    virtual std::string getGraphVizShape() const = 0;
    virtual std::string getGraphVizLabelDetails() const = 0;
    virtual std::string getGraphVizNodeColor() const = 0;
    virtual std::string getGraphVizFillColor() const = 0;
    virtual size_t getGraphVizNodePenWidth() const;

  friend class InstantiationFlow;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class NonrealInstantiation : public Instantiation {
  protected:
    static NonrealInstantiation * build(SgSymbol * symbol);

  protected:
    NonrealInstantiation(SgSymbol * symbol__);

    virtual std::string getKind() const;
    
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizLabelDetails() const;
    virtual std::string getGraphVizNodeColor() const;
    virtual std::string getGraphVizFillColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateElement;

  friend class Relation;
  friend class TemplateRelation;
  friend class CannonicalRelation;
  friend class TypeOfRelation;
  friend class BaseTypeRelation;
};

class TemplateInstantiation : public Instantiation {
  protected:
    std::vector<std::set<TemplateInstantiation *> > specializations;
    std::set<NonrealInstantiation *> instantiations;
    
    std::map < SgNonrealSymbol *, std::pair< size_t , std::string > > field_constraints; // fields of non-real declaration: assoc. template parameter position and qualified name for the field (first qualifier is template parameter name)

    bool is_defined;

  protected:
    static TemplateInstantiation * build(SgSymbol * symbol);

  protected:
    TemplateInstantiation(SgSymbol * symbol__);

    virtual std::string getKind() const;
    
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizLabelDetails() const;
    virtual std::string getGraphVizNodeColor() const;
    virtual std::string getGraphVizFillColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

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

#endif /* __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_HPP__ */

