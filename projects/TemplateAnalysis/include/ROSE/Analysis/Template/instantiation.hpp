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

    typedef enum {
      e_nontype_parameter,
      e_type_parameter,
      e_template_parameter
    } parameter_kind_e;

    typedef enum {
      e_nontype_argument,
      e_type_argument,
      e_template_argument,
      e_pack_expansion_argument
    } argument_kind_e;

  protected:
    SgSymbol * symbol;
    TemplateInstantiation * cannonical;

    SgDeclarationScope * nonreal_scope;

    std::map<SgSymbol *, nontype_param_info_t  > nontype_parameters;
    std::map<SgSymbol *, type_param_info_t     > type_parameters;
    std::map<SgSymbol *, template_param_info_t > template_parameters;
    std::vector<SgNode *> parameters_map;
    std::vector<parameter_kind_e> parameters_kind;

    std::map<SgExpression *, nontype_arg_info_t  > nontype_arguments;
    std::map<SgType *,       type_arg_info_t     > type_arguments;
    std::map<SgSymbol *,     template_arg_info_t > template_arguments;
    std::vector<SgNode *> arguments_map;
    std::vector<argument_kind_e> arguments_kind;

    std::set<Instantiation *> sources;

  public:
    Instantiation(SgSymbol * sym);
    virtual ~Instantiation();
    
    void construct();
    virtual void finalize() = 0;

    virtual std::string getGraphVizLabel() const;
    virtual std::string getGraphVizTag() const = 0;
    virtual std::string getGraphVizShape() const = 0;
    virtual std::string getGraphVizLabelConstraints() const = 0;
    virtual std::string getGraphVizNodeColor() const = 0;

    void toGraphViz(std::ostream & out) const;

  friend class InstantiationFlow;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;
};

class NonrealInstantiation : public Instantiation {
  protected:
    static NonrealInstantiation * build(SgSymbol * symbol, Instantiation * source = NULL);

  protected:
    NonrealInstantiation(SgSymbol * symbol__);
    
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizLabelConstraints() const;
    virtual std::string getGraphVizNodeColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class TemplateInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;
};

class TemplateInstantiation : public Instantiation {
  protected:
    std::vector<std::set<TemplateInstantiation *> > specializations;
    std::set<NonrealInstantiation *> instantiations;
    
    std::map < SgNonrealSymbol *, std::pair< size_t , std::string > > field_constraints; // fields of non-real declaration: assoc. template parameter position and qualified name for the field (first qualifier is template parameter name)

    bool is_defined;

  protected:
    static TemplateInstantiation * build(SgSymbol * symbol, Instantiation * source = NULL);

  protected:
    TemplateInstantiation(SgSymbol * symbol__);
    
    void construct();
    virtual void finalize();

    virtual std::string getGraphVizTag() const;
    virtual std::string getGraphVizShape() const;
    virtual std::string getGraphVizLabelConstraints() const;
    virtual std::string getGraphVizNodeColor() const;

  friend class InstantiationFlow;

  friend class Instantiation;
  friend class NonrealInstantiation;

  friend class Constraints;
  friend class InstantiationConstraints;
  friend class SpecializationConstraints;

  friend class Element;
  friend class TemplateParameterElement;
  friend class TemplateArgumentElement;

  friend class Relation;
  friend class TemplateParameterRelation;
  friend class TemplateArgumentRelation;
};

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_HPP__ */

