
#include "sage3basic.h"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"
#include "ROSE/Analysis/Template/instantiation.hpp"
#include "ROSE/Analysis/Template/constraints.hpp"
#include "ROSE/Analysis/Template/element.hpp"
#include "ROSE/Analysis/Template/relation.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/utils.hpp"

#include <sstream>

namespace ROSE { namespace Analysis { namespace Template {

Constraints::Constraints() :
  Relation()
{}

Constraints::~Constraints() {}

void Constraints::construct() {
  // TODO
}

std::string Constraints::getGraphVizShape() const {
  return std::string("octagon");
}

std::string Constraints::getGraphVizStyle() const {
  return std::string("solid");
}

InstantiationConstraints * InstantiationConstraints::build(TemplateInstantiation * from, NonrealInstantiation * to) {
  std::pair<Instantiation *, Instantiation *> coord(from,to);

  InstantiationConstraints * result = NULL;
  if (all.find(coord) != all.end()) {
    result = dynamic_cast<InstantiationConstraints *>(all[coord]);
  } else {
    result = new InstantiationConstraints(from, to);
    all.insert(std::pair<std::pair<Instantiation *, Instantiation *>, Constraints *>(coord, result));
    result->construct();
  }
  return result;
}

InstantiationConstraints::InstantiationConstraints(TemplateInstantiation * from_, NonrealInstantiation * to_) :
  Constraints(),
  from(from_),
  to(to_)
{}

InstantiationConstraints::~InstantiationConstraints() {}

void InstantiationConstraints::construct() {
#if 0
  std::cerr << "InstantiationConstraints::construct(this = " << std::hex << this << "):" << std::endl;
  std::cerr << " -- from = " << std::hex << from << ": " << from->symbol->get_name().getString() << std::endl;
  std::cerr << "   -- arguments_map.size()  = " << from->arguments_map.size()  << std::endl;
  std::cerr << "   -- parameters_map.size() = " << from->parameters_map.size() << std::endl;
  std::cerr << " -- to   = " << std::hex << to   << ": " << to->symbol->get_name().getString()   << std::endl;
  std::cerr << "   -- arguments_map.size()  = " << to->arguments_map.size()  << std::endl;
  std::cerr << "   -- parameters_map.size() = " << to->parameters_map.size() << std::endl;
#endif

  if (to->cannonical != NULL) {
//  std::cerr << " -> Non-type parameters:" << std::endl;
    if (to->arguments_map.size() == to->cannonical->parameters_map.size()) {
      for (auto it = to->cannonical->nontype_parameters.begin(); it != to->cannonical->nontype_parameters.end(); it++) {
        size_t pos = it->second.first;
//      std::cerr << "   -- pos = " << pos << std::endl;
        SgType * t = it->second.second;
        SgExpression * e = isSgExpression(to->arguments_map[pos]->node);
        assert(e != NULL);

        std::vector<SgNonrealRefExp *> nrrefs = SageInterface::querySubTree<SgNonrealRefExp>(e);
        for (auto it = nrrefs.begin(); it != nrrefs.end(); it++) {
          SgNonrealSymbol * nrsym = (*it)->get_symbol();
          assert(nrsym != NULL);
//        std::cerr << "     -- nrsym = " << std::hex << nrsym << " : " << nrsym->get_name() << std::endl;

          SgNode * parent = (*it)->get_parent();
//        std::cerr << "       -- parent = " << std::hex << parent << " (" << parent->class_name() << ")" << std::endl;

//        SgCastExp * cexp = isSgCastExp((*it)->get_parent());
//        assert(cexp != NULL);

          SgExpression * exp = isSgExpression(parent);
          ROSE_ASSERT(exp != NULL);
          SgType * req_type = exp->get_type();
          assert(req_type != NULL);

          nontype_constraints.insert(std::pair<SgNonrealSymbol *, SgType *>(nrsym, req_type));
        }
      }
      for (auto it = to->cannonical->type_parameters.begin(); it != to->cannonical->type_parameters.end(); it++) {
        // TODO I feel like that is the "default" (if neither "nontype" nor "template" then it is "type") also the only constraint I can think of is for the difference (if any) between 'class' and 'typename'
      }
      for (auto it = to->cannonical->template_parameters.begin(); it != to->cannonical->template_parameters.end(); it++) {
        // TODO template constraint include the template header
      }
    } else {
      // FIXME ROSE-1465
    }
  }
}

void InstantiationConstraints::finalize() {
  // TODO
}

std::string InstantiationConstraints::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "ic_" << std::hex << this;
  return oss.str();
}

void InstantiationConstraints::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=1.0, penwidth=3];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=1.0, penwidth=3];" << std::endl;
}

std::string InstantiationConstraints::getGraphVizLabel() const {
  std::ostringstream oss;
  
  oss << "Instantiation\\n";

  for (auto it = nontype_constraints.begin(); it != nontype_constraints.end(); it++) {
    if (it != nontype_constraints.begin())
      oss << "\\n";

    SgNonrealSymbol * nrsym = it->first;
    assert(nrsym != NULL);

    std::string nrname;
    if (from->field_constraints.find(nrsym) != from->field_constraints.end()) {
      nrname = from->field_constraints[nrsym].second;
    } else {
//    assert(from->nontype_parameters.find(nrsym) != from->nontype_parameters.end()); // FIXME ROSE-1465
      nrname = nrsym->get_name();
    }

    SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
      inputUnparseInfoPointer->set_SkipComments();
      inputUnparseInfoPointer->set_SkipWhitespaces();
      inputUnparseInfoPointer->set_SkipEnumDefinition();
      inputUnparseInfoPointer->set_SkipClassDefinition();
      inputUnparseInfoPointer->set_SkipFunctionDefinition();
      inputUnparseInfoPointer->set_SkipBasicBlock();
      inputUnparseInfoPointer->set_isTypeFirstPart();

    oss << "typeof(" << nrname << ") == " << globalUnparseToString(it->second, inputUnparseInfoPointer);
  }

  return oss.str();
}

std::string InstantiationConstraints::getGraphVizColor() const {
  return std::string("red");
}

SpecializationConstraints * SpecializationConstraints::build(NonrealInstantiation * from, TemplateInstantiation * to, size_t order) {
  std::pair<Instantiation *, Instantiation *> coord(from,to);

  SpecializationConstraints * result = NULL;
  if (all.find(coord) != all.end()) {
    result = dynamic_cast<SpecializationConstraints *>(all[coord]);
  } else {
    result = new SpecializationConstraints(from, to, order);
    all.insert(std::pair<std::pair<Instantiation *, Instantiation *>, Constraints *>(coord, result));
    result->construct();
  }

  return result;
}

SpecializationConstraints::SpecializationConstraints(NonrealInstantiation * from_, TemplateInstantiation * to_, size_t order_) :
  Constraints(),
  from(from_),
  to(to_),
  order(order_),
  value_constraints(),
  type_constraints()
{}

SpecializationConstraints::~SpecializationConstraints() {}

void SpecializationConstraints::construct() {
#if 0
  std::cerr << "SpecializationConstraints::construct(this = " << std::hex << this << "):" << std::endl;
  std::cerr << " -- from = " << std::hex << from << ": " << from->symbol->get_name().getString() << " (" << from->symbol->class_name() << ")" << std::endl;
  std::cerr << "   -- cannonical = " << from->cannonical << std::endl;
  std::cerr << "   -- arguments_map.size()  = " << from->arguments_map.size()  << std::endl;
  std::cerr << "   -- parameters_map.size() = " << from->parameters_map.size() << std::endl;
  std::cerr << " -- to   = " << std::hex <<   to << ": " <<   to->symbol->get_name().getString() << " (" <<   to->symbol->class_name() << ")" << std::endl;
  std::cerr << "   -- cannonical = " << to->cannonical << std::endl;
  std::cerr << "   -- arguments_map.size()  = " << to->arguments_map.size()  << std::endl;
  std::cerr << "   -- parameters_map.size() = " << to->parameters_map.size() << std::endl;
#endif

  if (to != to->cannonical) {
    if (from->arguments_map.size() == to->arguments_map.size()) {
      for (size_t i = 0; i < from->arguments_map.size(); i++) {
//      std::cerr << " -- from.args[" << i << "] = " << std::hex << from->arguments_map[i]->node << " (" << from->arguments_map[i]->node->class_name() << ")" << std::endl;
//      std::cerr << " --   to.args[" << i << "] = " << std::hex <<   to->arguments_map[i]->node << " (" <<   to->arguments_map[i]->node->class_name() << ")" << std::endl;

        if (from->arguments_kind[i] == to->arguments_kind[i]) {
          switch (from->arguments_kind[i]) {
            case TemplateRelation::e_type_argument: {
              SgType * type_from = isSgType(from->arguments_map[i]->node);
              SgType * type_to   = isSgType(to->arguments_map[i]->node);
              type_constraints.insert(std::pair<SgType *, SgType *>(type_from, type_to));
              break;
            }
            case TemplateRelation::e_nontype_argument: {
              SgExpression * expr_from = isSgExpression(from->arguments_map[i]->node);
              SgExpression * expr_to   = isSgExpression(to->arguments_map[i]->node);
              value_constraints.insert(std::pair<SgExpression *, SgExpression *>(expr_from, expr_to));
              break;
            }
            case TemplateRelation::e_template_argument: {
//            assert(false); // TODO
              break;
            }
            case TemplateRelation::e_pack_expansion_argument: {
              assert(false);
            }
            default: {
              assert(false);
            }
          }
        } else {
          // FIXME ROSE-1465
        }
      }
    } else {
      // FIXME ROSE-1465
    }
  } else {
    // FIXME I don't think it creates any constraint as it is the default...
  }
}

void SpecializationConstraints::finalize() {
  // TODO
}

std::string SpecializationConstraints::getGraphVizTag() const {
  std::ostringstream oss;
  oss << "sc_" << std::hex << this;
  return oss.str();
}

void SpecializationConstraints::toGraphVizEdges(std::ostream & out) const {
  out << from->getGraphVizTag() << " -> " << getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=1.0, penwidth=3];" << std::endl;
  out << getGraphVizTag() << " -> " << to->getGraphVizTag() << " [color=\"" << getGraphVizColor() << "\", style=\"" << getGraphVizStyle() << "\", weight=1.0, penwidth=3];" << std::endl;
}

std::string SpecializationConstraints::getGraphVizLabel() const {
  std::ostringstream oss;

  SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
    inputUnparseInfoPointer->set_SkipComments();
    inputUnparseInfoPointer->set_SkipWhitespaces();
    inputUnparseInfoPointer->set_SkipEnumDefinition();
    inputUnparseInfoPointer->set_SkipClassDefinition();
    inputUnparseInfoPointer->set_SkipFunctionDefinition();
    inputUnparseInfoPointer->set_SkipBasicBlock();
    inputUnparseInfoPointer->set_isTypeFirstPart();
  
  oss << "Specialization: " << order << "\\n\\n";

  for (auto it = value_constraints.begin(); it != value_constraints.end(); it++) {
    if (it != value_constraints.begin())
      oss << "\\n";
    oss << globalUnparseToString(it->first, inputUnparseInfoPointer) << " == " << globalUnparseToString(it->second, inputUnparseInfoPointer);
  }
  
  if (!value_constraints.empty())
    oss << "\\n";

  for (auto it = type_constraints.begin(); it != type_constraints.end(); it++) {
    if (it != type_constraints.begin())
      oss << "\\n";
    oss << "type_match( " << globalUnparseToString(it->first, inputUnparseInfoPointer) << " , " << globalUnparseToString(it->second, inputUnparseInfoPointer) << " )";
  }

  return oss.str();
}

std::string SpecializationConstraints::getGraphVizColor() const {
  return std::string("blue");
}

} } }

