
#include "sage3basic.h"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE { namespace Analysis { namespace Template {

std::map<SgNode *, Element *> Element::all;

Element * Element::get(SgNode * n) {
  auto it = all.find(n);
  if (it != all.end())
    return it->second;
  return NULL;
}

Element::Element(SgNode * node__) :
  node(node__)
{}

Element::~Element() {}

void Element::toGraphViz(std::ostream & out) const {
  out << getGraphVizTag() << "[label=\"" << getGraphVizLabel() << "\", shape=\"" << getGraphVizShape() << "\", color=\"" << getGraphVizNodeColor() << "\", penwidth=" << getGraphVizNodePenWidth() << ", style=\"" << getGraphVizStyle() << "\", fillcolor=\"" << getGraphVizFillColor() << "\"];" << std::endl;
}

std::string Element::getGraphVizFillColor() const {
  return std::string("white");
}

//

TemplateElement * TemplateElement::build(SgNode * node) {
  TemplateElement * result = NULL;

  if (all.find(node) == all.end()) {
    result = new TemplateElement(node);
    all.insert(std::pair<SgNode *, Element *>(node, result));
    result->construct();
  } else {
    result = dynamic_cast<TemplateElement *>(all[node]);
    if (result == NULL) {
      std::cerr << "FATAL: TemplateElement::build(...) with node = " << std::hex << node << " (" << node->class_name() << "): Another kind of element already exists for this symbol!" << std::endl;
    }
    assert(result != NULL);
  }

  return result;
}

TemplateElement::TemplateElement(SgNode * node__) :
  Element(node__)
{}

TemplateElement::~TemplateElement() {}

std::string TemplateElement::getKind() const { return "TemplateElement"; }

void TemplateElement::construct() {
  if (isSgType(node)) {
    SgNamedType *     ntype = isSgNamedType(node);
    SgPointerType *   ptype = isSgPointerType(node);
    SgReferenceType * rtype = isSgReferenceType(node);
    SgModifierType *  mtype = isSgModifierType(node);
    SgFunctionType *  ftype = isSgFunctionType(node);

    if (ntype) {
      SgDeclarationStatement * declstmt = ntype->get_declaration();
      assert(declstmt != NULL);

      SgSymbol * sym = declstmt->search_for_symbol_from_symbol_table();
      assert(sym != NULL);

      SgClassSymbol *   csym  = isSgClassSymbol(sym);
      SgNonrealSymbol * nrsym = isSgNonrealSymbol(sym);

      Element * e = NULL;
      if (csym != NULL) {
        e = TemplateInstantiation::build(csym);
      } else if (nrsym != NULL) {
        SgNonrealDecl * nrdecl = nrsym->get_declaration();
        assert(nrdecl != NULL);

        if (nrdecl->get_is_class_member() || nrdecl->get_is_template_param() || nrdecl->get_is_template_template_param() || nrdecl->get_is_nonreal_template()) {
       // std::cout << "DEBUG: TemplateElement::construct(...) nrsym = " << std::hex << nrsym << " (" << nrsym->class_name() << "): " << nrsym->get_name().getString() << ": building TemplateElement!" << std::endl;
          e = TemplateElement::build(nrsym);
        } else {
       // std::cout << "DEBUG: TemplateElement::construct(...) nrsym = " << std::hex << nrsym << " (" << nrsym->class_name() << "): " << nrsym->get_name().getString() << ": building NonrealInstantiation!" << std::endl;
          e = NonrealInstantiation::build(nrsym);
        }
      } else {
        e = TemplateElement::build(sym);
      }
      assert(e != NULL);

      TypeOfRelation::build(this, e);

    } else if (ptype) {
      SgType * btype = ptype->get_base_type();
      assert(btype != NULL);

      TemplateElement * te = TemplateElement::build(btype);
      assert(te != NULL);

      BaseTypeRelation::build(this, te);

    } else if (rtype) {
      SgType * btype = rtype->get_base_type();
      assert(btype != NULL);

      TemplateElement * te = TemplateElement::build(btype);
      assert(te != NULL);

      BaseTypeRelation::build(this, te);

    } else if (mtype) {
      SgType * btype = mtype->get_base_type();
      assert(btype != NULL);

      TemplateElement * te = TemplateElement::build(btype);
      assert(te != NULL);

      BaseTypeRelation::build(this, te);

    } else if (ftype) {
      // TODO
      //   SgType * 	get_return_type ()
      //   SgFunctionParameterTypeList * 	get_argument_list ()
    }
  }
}

void TemplateElement::finalize() {
  // TODO
}

std::string TemplateElement::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "te_" << std::hex << node;
  return oss.str();
}

std::string TemplateElement::getGraphVizLabel() const {
  std::ostringstream oss;
  oss << node->class_name();
  oss << "\\n";

  SgSymbol * sym = isSgSymbol(node);
  if (sym != NULL) {
    oss << sym->get_name().getString();
  } else {
    SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
      inputUnparseInfoPointer->set_SkipComments();
      inputUnparseInfoPointer->set_SkipWhitespaces();
      inputUnparseInfoPointer->set_SkipEnumDefinition();
      inputUnparseInfoPointer->set_SkipClassDefinition();
      inputUnparseInfoPointer->set_SkipFunctionDefinition();
      inputUnparseInfoPointer->set_SkipBasicBlock();
      inputUnparseInfoPointer->set_isTypeFirstPart();
    oss << globalUnparseToString(node, inputUnparseInfoPointer);
  }

  oss << "\\n\\n";
  return oss.str();
}

std::string TemplateElement::getGraphVizShape() const {
  return std::string("ellipse");
}

std::string TemplateElement::getGraphVizNodeColor() const {
  return std::string("purple");
}

std::string TemplateElement::getGraphVizStyle() const {
  return std::string("dashed");
}

size_t TemplateElement::getGraphVizNodePenWidth() const {
  return 1;
}

} } }

