
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/utils.hpp"

#include <fstream>

#include "sage3basic.h"

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
    out << indent << "loop(" << std::endl;
    out << indent << "  "
        << loop->iterator->get_name().getString() << ", "
        << loop->lower_bound->unparseToString()   << ", "
        << loop->upper_bound->unparseToString()   << ", ";

    switch (loop->parallel_pattern) {
      case loop_t::none:
        out << "none, ";
        break;
      case loop_t::parfor:
        out << "parfor, ";
        break;
      case loop_t::reduction:
        assert(loop->reduction_lhs != NULL);
        out << "reduction(" << loop->reduction_lhs->unparseToString() << "), ";
        break;
      default:
        assert(false);
    }
    out << std::endl;
    
    std::list<node_t *>::const_iterator it_child = loop->children.begin();
    toText(*it_child, out, indent + "  ");
    it_child++;
    for (; it_child != loop->children.end(); it_child++) {
      out << "," << std::endl;
      toText(*it_child, out, indent + "  ");
    }

    out << std::endl << indent << ")";
  }
  
  if (stmt != NULL) {
    out << indent << "stmt(" << stmt->statement->unparseToString() << ")";
  }
}

const std::list<LoopTrees::node_t *> & LoopTrees::getTrees() const { return p_trees; }

const std::set<Data *> LoopTrees::getDatasIn() const { return p_datas_in; }

const std::set<Data *> LoopTrees::getDatasOut() const { return p_datas_out; }

const std::set<Data *> LoopTrees::getDatasLocal() const { return p_datas_local; }

const std::set<SgVariableSymbol *> LoopTrees::getCoefficients() const { return p_coefficients; }

const std::set<SgVariableSymbol *> LoopTrees::getParameters() const { return p_parameters; }

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

  if (!p_parameters.empty()) {
    it_sym = p_parameters.begin();
    out << "params(" << (*it_sym)->get_name().getString();
    it_sym++;
    for (; it_sym != p_parameters.end(); it_sym++)
      out << ", " << (*it_sym)->get_name().getString();
    out << ")" << std::endl;
  }
  else assert(false);

  if (!p_coefficients.empty()) {
    it_sym = p_coefficients.begin();
    out << "coefs(" << (*it_sym)->get_name().getString();
    it_sym++;
    for (; it_sym != p_coefficients.end(); it_sym++)
      out << ", " << (*it_sym)->get_name().getString();
    out << ")" << std::endl;
  }

  for (it_data = p_datas_in.begin(); it_data != p_datas_in.end(); it_data++) {
    out << "data(";
    (*it_data)->toText(out);
    out << ", flow-in)" << std::endl;
  }
  for (it_data = p_datas_out.begin(); it_data != p_datas_out.end(); it_data++) {
    out << "data(";
    (*it_data)->toText(out);
    out << ", flow-out)" << std::endl;
  }
  for (it_data = p_datas_local.begin(); it_data != p_datas_local.end(); it_data++) {
    out << "data(";
    (*it_data)->toText(out);
    out << ", local)" << std::endl;
  } 

  it_tree = p_trees.begin();
  out << "loop-trees(" << std::endl;
  toText(*it_tree, out, "  ");
  it_tree++;
  for (; it_tree != p_trees.end(); it_tree++) {
    out << "," << std::endl;
    toText(*it_tree, out, "  ");
  }
  out << std::endl << ")" << std::endl;
}

void parseParams(LoopTrees & loop_trees) {
  if (AstFromString::afs_match_substr("params")) {
    SgType * param_type = SageBuilder::buildUnsignedLongType();

    ensure('(');

    do {
      AstFromString::afs_skip_whitespace();

      assert(AstFromString::afs_match_identifier());
      SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
      assert(label != NULL);

      SgVariableDeclaration * param_decl = SageBuilder::buildVariableDeclaration_nfi(*label, param_type, NULL, NULL);
      SgVariableSymbol * var_sym = isSgVariableSymbol(param_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
      assert(var_sym != NULL);

      loop_trees.addParameter(var_sym);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    ensure(')');
  }
  AstFromString::afs_skip_whitespace();
}

void parseCoefs(LoopTrees & loop_trees) {
  if (AstFromString::afs_match_substr("coefs")) {
    SgType * coef_type = SageBuilder::buildFloatType();

    ensure('(');

    do {
      AstFromString::afs_skip_whitespace();

      assert(AstFromString::afs_match_identifier());
      SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
      assert(label != NULL);

      SgVariableDeclaration * coef_decl = SageBuilder::buildVariableDeclaration_nfi(*label, coef_type, NULL, NULL);
      SgVariableSymbol * var_sym = isSgVariableSymbol(coef_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
      assert(var_sym != NULL);

      loop_trees.addCoefficient(var_sym);
 
      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    ensure(')');
  }
  AstFromString::afs_skip_whitespace();
}

void parseDatas(LoopTrees & loop_trees) {
  while (AstFromString::afs_match_substr("data")) {

    ensure('(');

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    std::list<std::pair<SgExpression *, SgExpression *> > sections;
    SgType * data_type = SageBuilder::buildFloatType();
    while (AstFromString::afs_match_char('[')) {
      sections.push_back(std::pair<SgExpression *, SgExpression *>(NULL, NULL));
      std::pair<SgExpression *, SgExpression *> & section = sections.back();

      AstFromString::afs_skip_whitespace();

      assert(AstFromString::afs_match_additive_expression());
      section.first = isSgExpression(AstFromString::c_parsed_node);
      assert(section.first != NULL);

      AstFromString::afs_skip_whitespace();
      if (AstFromString::afs_match_char(':')) {
        AstFromString::afs_skip_whitespace();

        assert(AstFromString::afs_match_additive_expression());
        section.second = isSgExpression(AstFromString::c_parsed_node);
        assert(section.second != NULL);

        AstFromString::afs_skip_whitespace();
      }
      else {
        section.second = section.first;
      }

      ensure(']');

      data_type = SageBuilder::buildPointerType(data_type);
    }

    SgVariableDeclaration * data_decl = SageBuilder::buildVariableDeclaration_nfi(*label, data_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(data_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    Data * data = new Data(var_sym);

    std::list<std::pair<SgExpression *, SgExpression *> >::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++)
      data->addSection(*it_section);

    ensure(',');

    if (AstFromString::afs_match_substr("flow-in")) {
      loop_trees.addDataIn(data);
    }
    else if (AstFromString::afs_match_substr("flow-out")) {
      loop_trees.addDataOut(data);
    }
    else if (AstFromString::afs_match_substr("local")) {
      loop_trees.addDataLocal(data);
    }
    else assert(false);

    ensure(')');
  }
  AstFromString::afs_skip_whitespace();
}

LoopTrees::node_t * parseLoopTreesNode() {
  LoopTrees::node_t * lt_node = NULL;
  AstFromString::afs_skip_whitespace();

  if (AstFromString::afs_match_substr("loop")) {
    ensure('(');

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    SgVariableDeclaration * it_decl = SageBuilder::buildVariableDeclaration_nfi(*label, SageBuilder::buildUnsignedLongType(), NULL, NULL);
    SgVariableSymbol * it_sym = isSgVariableSymbol(it_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(it_sym != NULL);

    ensure(',');

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * lb_exp = isSgExpression(AstFromString::c_parsed_node);
    assert(lb_exp != NULL);

    ensure(',');

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * ub_exp = isSgExpression(AstFromString::c_parsed_node);
    assert(ub_exp != NULL);

    ensure(',');

    LoopTrees::loop_t::parallel_pattern_e pattern;
    SgExpression * parameter = NULL;
    if (AstFromString::afs_match_substr("none", false))
      pattern = LoopTrees::loop_t::none;
    else if (AstFromString::afs_match_substr("parfor", false))
      pattern = LoopTrees::loop_t::parfor;
    else if (AstFromString::afs_match_substr("reduction")) {
      pattern = LoopTrees::loop_t::reduction;

      ensure('(');

      assert(AstFromString::afs_match_additive_expression());
      parameter = isSgExpression(AstFromString::c_parsed_node);
      assert(parameter != NULL);

      ensure(')');
    }
    else assert(false);

    ensure(',');

    LoopTrees::loop_t * lt_loop = new LoopTrees::loop_t(it_sym, lb_exp, ub_exp, pattern, parameter);

    do {
      AstFromString::afs_skip_whitespace();

      LoopTrees::node_t * child_node = parseLoopTreesNode();
      assert(child_node != NULL);
      lt_loop->children.push_back(child_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    lt_node = lt_loop;

    ensure(')');
  }
  else if (AstFromString::afs_match_substr("stmt")) {

    ensure('(');

    assert(AstFromString::afs_match_statement());
    SgStatement * stmt = isSgStatement(AstFromString::c_parsed_node);
    assert(stmt != NULL);
    stmt->set_parent(AstFromString::c_sgnode);
    lt_node = new LoopTrees::stmt_t(stmt);

    ensure(')');
  }
  else assert(false);

  AstFromString::afs_skip_whitespace();
  return lt_node;
}

void LoopTrees::read(char * filename) {
  std::ifstream in_file;
  in_file.open(filename);

  assert(in_file.is_open());

  read(in_file);
}

void LoopTrees::read(std::ifstream & in_file) {
  initAstFromString(in_file);

  parseParams(*this);

  parseCoefs(*this);

  parseDatas(*this);

  if (AstFromString::afs_match_substr("loop-trees")) {

    ensure('(');

    do {
      LoopTrees::node_t * lt_node = parseLoopTreesNode();
      assert(lt_node != NULL);
      addTree(lt_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    ensure(')');
  }
  else assert(false);

  SageBuilder::popScopeStack();
}

void collectLeaves(LoopTrees::node_t * tree, std::set<SgStatement *> & leaves) {
  LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(tree);
  if (loop != NULL) {
    std::list<LoopTrees::node_t * >::const_iterator it_child;
    for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
      collectLeaves(*it_child, leaves);
    return;
  }

  LoopTrees::stmt_t * stmt = dynamic_cast<LoopTrees::stmt_t *>(tree);
  assert(stmt != NULL);

  leaves.insert(stmt->statement);
}

}

}
