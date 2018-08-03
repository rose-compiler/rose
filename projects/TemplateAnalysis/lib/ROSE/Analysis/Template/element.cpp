
#include "sage3basic.h"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE { namespace Analysis { namespace Template {

std::map<SgNode *, Element *> Element::all;

Element::Element() {}

Element::~Element() {}

void Element::toGraphViz(std::ostream & out) const {
  out << getGraphVizTag() << "[label=\"" << getGraphVizLabel() << "\", shape=\"" << getGraphVizShape() << "\", color=\"" << getGraphVizNodeColor() << "\"];" << std::endl;
}

//

TemplateParameterElement * TemplateParameterElement::build(SgNode * node) {
  assert(false);
}

TemplateParameterElement::TemplateParameterElement(SgNode * node__) :
  node(node__)
{
  assert(false);
}

TemplateParameterElement::~TemplateParameterElement() {
  assert(false);
}

void construct() {
  assert(false);
}

void TemplateParameterElement::finalize() {
  assert(false);
}

std::string TemplateParameterElement::getGraphVizTag() const {
  assert(false);
}

std::string TemplateParameterElement::getGraphVizLabel() const {
  assert(false);
}

std::string TemplateParameterElement::getGraphVizShape() const {
  assert(false);
}

std::string TemplateParameterElement::getGraphVizNodeColor() const {
  assert(false);
}

//

TemplateArgumentElement * TemplateArgumentElement::build(SgNode * node) {
  assert(false);
}

TemplateArgumentElement::TemplateArgumentElement(SgNode * node__) :
  node(node__)
{
  assert(false);
}

TemplateArgumentElement::~TemplateArgumentElement() {
  assert(false);
}

void TemplateArgumentElement::construct() {
  assert(false);
}

void TemplateArgumentElement::finalize() {
  assert(false);
}

std::string TemplateArgumentElement::getGraphVizTag() const {
  assert(false);
}

std::string TemplateArgumentElement::getGraphVizLabel() const {
  assert(false);
}

std::string TemplateArgumentElement::getGraphVizShape() const {
  assert(false);
}

std::string TemplateArgumentElement::getGraphVizNodeColor() const {
  assert(false);
}

} } }

