/** 
 * \file ROSE/Analysis/Template/instantiation-flow.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_FLOW_HPP__
#define __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_FLOW_HPP__

#include <set>
#include <iostream>

class SgNode;
class SgSymbol;
class SgClassSymbol;
class SgNonrealSymbol;
class SgType;
class SgExpression;
class SgDeclarationScope;

namespace ROSE {

namespace Analysis {

namespace Template {

class InstantiationFlow {

  public:
    static void insert(SgClassSymbol * symbol);
    static void insert(SgNonrealSymbol * symbol);

    static void finalize();

    static void toGraphViz(std::ostream & out, const std::set<std::string> & nsp_filter);

    static void clear();
};

class Element;
  class TemplateElement;
  class Instantiation;
    class TemplateInstantiation;
    class NonrealInstantiation;

class Relation;
  class TemplateRelation;
  class Constraints;
    class InstantiationConstraints;
    class SpecializationConstraints;

}

}

}

#endif /* __ROSE_ANALYSIS_TEMPLATE_INSTANTIATION_FLOW_HPP__ */

