
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
  out << "  " << getGraphVizTag() << " [label=\"" << getGraphVizLabel() << "\", shape=\"" << getGraphVizShape() << "\", color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\"];" << std::endl;
}

//

TemplateRelation * TemplateRelation::build(Instantiation * from, Element * to, kind_e kind, size_t pos) {
  std::pair<Element *, Element *> coord(from,to);

  TemplateRelation * result = NULL;

  if (all.find(coord) == all.end()) {
    result = new TemplateRelation(from, to, kind, pos);
    all.insert(std::pair<std::pair<Element *, Element *>, Relation *>(coord, result));
    result->construct();
  } else {
    result = dynamic_cast<TemplateRelation *>(all[coord]);
    if (result == NULL) {
      std::cerr << "FATAL: TemplateRelation::build(...) with from = " << std::hex << from->symbol << " (" << from->symbol->class_name() << ") and to = " << std::hex << to->node << " (" << to->node->class_name() << "): Another kind of relation already exists for this symbol!" << std::endl;
    }
    assert(result != NULL);
  }

  return result;
}

TemplateRelation::TemplateRelation(Instantiation * from_, Element * to_, kind_e kind_, size_t pos_) :
  Relation(),
  from(from_),
  to(to_),
  kind(kind_),
  position(pos_)
{}

TemplateRelation::~TemplateRelation() {}

void TemplateRelation::construct() {
  // TODO
}

void TemplateRelation::finalize() {
  // TODO
}

std::string TemplateRelation::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "tr_" << std::hex << this;
  return oss.str();
}

void TemplateRelation::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=10.0, penwidth=1, constraint=true];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=10.0, penwidth=1, constraint=true];" << std::endl;
}

std::string TemplateRelation::getGraphVizLabel() const {
  std::ostringstream oss;

  switch (kind) {
    case e_nontype_parameter: {
      oss << "non-type parameter";
      break;
    }
    case e_type_parameter: {
      oss << "type parameter";
      break;
    }
    case e_template_parameter: {
      oss << "template template parameter";
      break;
    }
    case e_nontype_argument: {
      oss << "non-type argument";
      break;
    }
    case e_type_argument: {
      oss << "type argument";
      break;
    }
    case e_template_argument: {
      oss << "template template argument";
      break;
    }
    case e_pack_expansion_argument: {
      assert(false);
    }
    default: {
      assert(false);
    }
  }
  oss << "\\nposition = " << position;

  return oss.str();
}

std::string TemplateRelation::getGraphVizColor() const {
  return std::string("purple");
}

std::string TemplateRelation::getGraphVizShape() const {
  return std::string("octagon");
}

std::string TemplateRelation::getGraphVizStyle() const {
  return std::string("dashed");
}

////

CannonicalRelation * CannonicalRelation::build(TemplateInstantiation * from, TemplateInstantiation * to) {
  std::pair<Element *, Element *> coord(from,to);

  CannonicalRelation * result = NULL;

  if (all.find(coord) == all.end()) {
    result = new CannonicalRelation(from, to);
    all.insert(std::pair<std::pair<Element *, Element *>, Relation *>(coord, result));
    result->construct();
  } else {
    result = dynamic_cast<CannonicalRelation *>(all[coord]);
    if (result == NULL) {
      std::cerr << "FATAL: CannonicalRelation::build(...) with from = " << std::hex << from << " and to = " << std::hex << to << ": Another kind of relation already exists for this symbol!" << std::endl;
    }
    assert(result != NULL);
  }

  return result;
}

CannonicalRelation::CannonicalRelation(TemplateInstantiation * from_, TemplateInstantiation * to_) :
  Relation(),
  from(from_),
  to(to_)
{}

CannonicalRelation::~CannonicalRelation() {}

void CannonicalRelation::construct() {
  // TODO
}

void CannonicalRelation::finalize() {
  // TODO
}

std::string CannonicalRelation::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "cr_" << std::hex << this;
  return oss.str();
}

void CannonicalRelation::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
}

std::string CannonicalRelation::getGraphVizLabel() const {
  return std::string("cannonical");
}

std::string CannonicalRelation::getGraphVizColor() const {
  return std::string("orange");
}

std::string CannonicalRelation::getGraphVizShape() const {
  return std::string("octagon");
}

std::string CannonicalRelation::getGraphVizStyle() const {
  return std::string("dotted");
}

////

BaseTypeRelation * BaseTypeRelation::build(Element * from, Element * to) {
  std::pair<Element *, Element *> coord(from,to);

  BaseTypeRelation * result = NULL;

  if (all.find(coord) == all.end()) {
    result = new BaseTypeRelation(from, to);
    all.insert(std::pair<std::pair<Element *, Element *>, Relation *>(coord, result));
    result->construct();
  } else {
    result = dynamic_cast<BaseTypeRelation *>(all[coord]);
    if (result == NULL) {
      std::cerr << "FATAL: BaseTypeRelation::build(...) with from = " << std::hex << from << " and to = " << std::hex << to << ": Another kind of relation already exists for this symbol!" << std::endl;
    }
    assert(result != NULL);
  }

  return result;
}

BaseTypeRelation::BaseTypeRelation(Element * from_, Element * to_) :
  Relation(),
  from(from_),
  to(to_)
{}

BaseTypeRelation::~BaseTypeRelation() {}

void BaseTypeRelation::construct() {
  // TODO
}

void BaseTypeRelation::finalize() {
  // TODO
}

std::string BaseTypeRelation::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "btr_" << std::hex << this;
  return oss.str();
}

void BaseTypeRelation::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
}

std::string BaseTypeRelation::getGraphVizLabel() const {
  return std::string("base type");
}

std::string BaseTypeRelation::getGraphVizColor() const {
  return std::string("brown");
}

std::string BaseTypeRelation::getGraphVizShape() const {
  return std::string("octagon");
}

std::string BaseTypeRelation::getGraphVizStyle() const {
  return std::string("dotted");
}


////

TypeOfRelation * TypeOfRelation::build(Element * from, Element * to) {
  std::pair<Element *, Element *> coord(from,to);

  TypeOfRelation * result = NULL;

  if (all.find(coord) == all.end()) {
    result = new TypeOfRelation(from, to);
    all.insert(std::pair<std::pair<Element *, Element *>, Relation *>(coord, result));
    result->construct();
  } else {
    result = dynamic_cast<TypeOfRelation *>(all[coord]);
    if (result == NULL) {
      std::cerr << "FATAL: TypeOfRelation::build(...) with from = " << std::hex << from << " and to = " << std::hex << to << ": Another kind of relation already exists for this symbol!" << std::endl;
    }
    assert(result != NULL);
  }

  return result;
}

TypeOfRelation::TypeOfRelation(Element * from_, Element * to_) :
  Relation(),
  from(from_),
  to(to_)
{}

TypeOfRelation::~TypeOfRelation() {}

void TypeOfRelation::construct() {
  // TODO
}

void TypeOfRelation::finalize() {
  // TODO
}

std::string TypeOfRelation::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "tor_" << std::hex << this;
  return oss.str();
}

void TypeOfRelation::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=5.0, penwidth=1, constraint=true];" << std::endl;
}

std::string TypeOfRelation::getGraphVizLabel() const {
  return std::string("typeof");
}

std::string TypeOfRelation::getGraphVizColor() const {
  return std::string("brown");
}

std::string TypeOfRelation::getGraphVizShape() const {
  return std::string("octagon");
}

std::string TypeOfRelation::getGraphVizStyle() const {
  return std::string("dotted");
}

} } }

