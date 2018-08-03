
#include "sage3basic.h"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

namespace ROSE { namespace Analysis { namespace Template {

std::map<std::pair<Element *, Element *>, Relation *> Relation::all;

Relation::Relation() {}

Relation::~Relation() {}

void Relation::construct() {}

void Relation::toGraphViz(std::ostream & out) const {
  toGraphVizEdges(out);
  out << "  " << getGraphVizTag() << " [label=\"" << getGraphVizLabel() << "\", shape=\"" << getGraphVizShape() << "\", color=\"" << getGraphVizColor() << "\"];" << std::endl;
}

//

TemplateParameterRelation * TemplateParameterRelation::build(Instantiation * from, TemplateParameterElement * to) {
  assert(false);
}

TemplateParameterRelation::TemplateParameterRelation(Instantiation * from_, TemplateParameterElement * to_) : 
  from(from_),
  to(to_)
{
  assert(false);
}

TemplateParameterRelation::~TemplateParameterRelation() {
  assert(false);
}

void TemplateParameterRelation::construct() {
  assert(false);
}

void TemplateParameterRelation::finalize() {
  assert(false);
}

std::string TemplateParameterRelation::getGraphVizTag() const {
  assert(false);
}

void TemplateParameterRelation::toGraphVizEdges(std::ostream & out) const {
  assert(false);
}

std::string TemplateParameterRelation::getGraphVizLabel() const {
  assert(false);
}

std::string TemplateParameterRelation::getGraphVizColor() const {
  assert(false);
}

std::string TemplateParameterRelation::getGraphVizShape() const {
  assert(false);
}

//

TemplateArgumentRelation * TemplateArgumentRelation::build(Instantiation * from, TemplateArgumentElement * to) {
  assert(false);
}

TemplateArgumentRelation::TemplateArgumentRelation(Instantiation * from_, TemplateArgumentElement * to_) :
  from(from_),
  to(to_)
{
  assert(false);
}

TemplateArgumentRelation::~TemplateArgumentRelation() {
  assert(false);
}

void TemplateArgumentRelation::construct() {
  assert(false);
}

void TemplateArgumentRelation::finalize() {
  assert(false);
}

std::string TemplateArgumentRelation::getGraphVizTag() const {
  assert(false);
}

void TemplateArgumentRelation::toGraphVizEdges(std::ostream & out) const {
  assert(false);
}

std::string TemplateArgumentRelation::getGraphVizLabel() const {
  assert(false);
}

std::string TemplateArgumentRelation::getGraphVizColor() const {
  assert(false);
}

std::string TemplateArgumentRelation::getGraphVizShape() const {
  assert(false);
}

} } }

