
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include <fstream>

#include "rose.h"

#include <cassert>

namespace KLT {

namespace Core {

LoopTrees::node_t::node_t() :
  parent(NULL)
{}

LoopTrees::node_t::~node_t() {}

LoopTrees::loop_t::loop_t(
  SgVariableSymbol * it,
  SgExpression * lb,
  SgExpression * ub,
  parallel_pattern_e par_pattern,
  SgExpression * red_lhs
) :
  node_t(),
  iterator(it),
  lower_bound(lb),
  upper_bound(ub),
  parallel_pattern(par_pattern),
  reduction_lhs(red_lhs),
  children()
{}

LoopTrees::loop_t::~loop_t() {}

LoopTrees::stmt_t::stmt_t(SgStatement * stmt) :
  node_t(),
  statement(stmt)
{}

LoopTrees::stmt_t::~stmt_t() {}

void LoopTrees::toText(node_t * node, std::ostream & out, std::string indent) {
  loop_t * loop = dynamic_cast<loop_t *>(node);
  stmt_t * stmt = dynamic_cast<stmt_t *>(node);
  
  assert(loop != NULL || stmt != NULL);
  
  if (loop != NULL) {
    // TODO
  }
  
  if (stmt != NULL) {
    // TODO
  }
}

LoopTrees::LoopTrees() :
  p_trees(),
  p_datas_in(),
  p_datas_out(),
  p_datas_local(),
  p_coefficients(),
  p_parameters()
{}

LoopTrees::~LoopTrees() {}

void LoopTrees::addTree(node_t * tree) { p_trees.push_back(tree); }

void LoopTrees::addDataIn(Data * data) { p_datas_in.insert(data); }

void LoopTrees::addDataOut(Data * data) { p_datas_out.insert(data); }

void LoopTrees::addDataLocal(Data * data) { p_datas_local.insert(data); }

void LoopTrees::addCoefficient(SgVariableSymbol * var_sym) { p_coefficients.insert(var_sym); }

void LoopTrees::addParameter(SgVariableSymbol * var_sym) { p_parameters.insert(var_sym); }

void LoopTrees::toText(char * filename) const {
  std::ofstream file;
  file.open(filename);
  assert(file.is_open());
  toText(file);
  file.close();
}
    
void LoopTrees::toText(std::ostream & out) const {
  std::set<SgVariableSymbol *>::const_iterator it_sym;
  std::set<Data *>::const_iterator it_data;
  std::list<node_t *>::const_iterator it_tree;

  out << "Parameters:" << std::endl;
  for (it_sym = p_parameters.begin(); it_sym != p_parameters.end(); it_sym++)
    out << " > " << (*it_sym)->get_name().getString() << std::endl;

  out << "Coefficient:" << std::endl;
  for (it_sym = p_coefficients.begin(); it_sym != p_coefficients.end(); it_sym++)
    out << " > " << (*it_sym)->get_name().getString() << std::endl;

  out << "Data:" << std::endl;
  for (it_data = p_datas_in.begin(); it_data != p_datas_in.end(); it_data++) {
    out << " >    in : ";
    (*it_data)->toText(out);
    out << std::endl;
  }
  for (it_data = p_datas_out.begin(); it_data != p_datas_out.end(); it_data++) {
    out << " >   out : ";
    (*it_data)->toText(out);
    out << std::endl;
  }
  for (it_data = p_datas_local.begin(); it_data != p_datas_local.end(); it_data++) {
    out << " > local : ";
    (*it_data)->toText(out);
    out << std::endl;
  } 

  out << "Loop trees:" << std::endl;
  for (it_tree = p_trees.begin(); it_tree != p_trees.end(); it_tree++)
    toText(*it_tree, out, " > ");
}

}

}
