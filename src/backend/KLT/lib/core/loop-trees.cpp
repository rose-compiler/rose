
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

void parseParams(LoopTrees & loop_trees, std::list<SgVariableSymbol *> & parameter_order) {
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
      parameter_order.push_back(var_sym);

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

void parseDatas(LoopTrees & loop_trees, std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order) {
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
      inout_data_order.first.push_back(data);
    }
    else if (AstFromString::afs_match_substr("flow-out")) {
      loop_trees.addDataOut(data);
      inout_data_order.second.push_back(data);
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

void LoopTrees::read(char * filename, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order) {
  std::ifstream in_file;
  in_file.open(filename);

  assert(in_file.is_open());

  read(in_file, parameter_order, inout_data_order);

  in_file.close();
}

void LoopTrees::read(std::ifstream & in_file, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order) {
  initAstFromString(in_file);

  parseParams(*this, parameter_order);

  parseCoefs(*this);

  parseDatas(*this, inout_data_order);

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

SgExpression * translateConstExpression(
  SgExpression * expr,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
) {
  SgExpression * result = SageInterface::copyExpression(expr);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;

    it_sym_to_local = param_to_local.find(var_sym);
    if (it_sym_to_local != param_to_local.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = iter_to_local.find(var_sym);
    if (it_sym_to_local != iter_to_local.end()) {
      assert(local_sym == NULL); // implies VarRef to a variable symbol which is both parameter and iterator... It does not make sense!

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol (neither parameter or iterator)

    SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
  }

  return result;
}

SgStatement * generateStatement(
  LoopTrees::node_t * node, 
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & coef_to_local,
  const std::map<Data *, SgVariableSymbol *>             & data_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local,
  bool generate_in_depth,
  bool flatten_array_ref
) {
  SgStatement * result = NULL;

  LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(node);
  LoopTrees::stmt_t * stmt = dynamic_cast<LoopTrees::stmt_t *>(node);

  assert((loop != NULL) xor (stmt != NULL));

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
  std::map<Data *, SgVariableSymbol *>::const_iterator it_data_to_local;

  if (loop != NULL) {
    it_sym_to_local = iter_to_local.find(loop->iterator);
    assert(it_sym_to_local != iter_to_local.end());
    SgVariableSymbol * local_it_sym = it_sym_to_local->second;

    SgExpression * lower_bound = translateConstExpression(loop->lower_bound, param_to_local, iter_to_local);
    SgExpression * upper_bound = translateConstExpression(loop->upper_bound, param_to_local, iter_to_local);

    SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(local_it_sym), lower_bound));
    SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(local_it_sym), upper_bound));;
    SgExpression * inc_expr = SageBuilder::buildPlusPlusOp(SageBuilder::buildVarRefExp(local_it_sym));

    SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
    SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);
    assert(for_body->get_parent() != NULL);

    if (generate_in_depth) {
      std::list<LoopTrees::node_t * >::const_iterator it_child;
      for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++) {
        SgStatement * child_stmt = generateStatement(*it_child, param_to_local, coef_to_local, data_to_local, iter_to_local, generate_in_depth, flatten_array_ref);
        SageInterface::appendStatement(child_stmt, for_body);
      }
    }

    result = for_stmt;
  }

  if (stmt != NULL) {
    result = SageInterface::copyStatement(stmt->statement);

    std::map<SgVariableSymbol *, SgVariableSymbol *> data_sym_to_local;
    std::map<SgVariableSymbol *, Data *> data_sym_to_data;
    std::map<Data *, SgVariableSymbol *>::const_iterator it_data_to_local;
    for (it_data_to_local = data_to_local.begin(); it_data_to_local != data_to_local.end(); it_data_to_local++) {
      Data * data = it_data_to_local->first;
      SgVariableSymbol * data_sym = it_data_to_local->first->getVariableSymbol();
      SgVariableSymbol * local_sym = it_data_to_local->second;

      data_sym_to_local.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, local_sym));
      data_sym_to_data.insert(std::pair<SgVariableSymbol *, Data *>(data_sym, data));
    }

    std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
    std::vector<SgVarRefExp *>::const_iterator it_var_ref;

    if (flatten_array_ref) {
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVarRefExp * var_ref = *it_var_ref;
        SgVariableSymbol * var_sym = var_ref->get_symbol();

        std::map<SgVariableSymbol *, Data *>::const_iterator it_data_sym_to_data = data_sym_to_data.find(var_sym);
        if (it_data_sym_to_data == data_sym_to_data.end()) continue; // Not a variable reference to a Data

        Data * data = it_data_sym_to_data->second;

        if (data->getSections().size() <= 1) continue; // No need for flattening

        SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(var_ref->get_parent());
        SgPntrArrRefExp * top_arr_ref = NULL;
        std::list<SgExpression *> subscripts;
        while (arr_ref != NULL) {
          top_arr_ref = arr_ref;
          subscripts.push_back(arr_ref->get_rhs_operand_i());
          arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());
        }
        assert(top_arr_ref != NULL);
        assert(subscripts.size() == data->getSections().size());

        std::list<SgExpression *>::const_iterator it_subscript;
        SgExpression * subscript = SageInterface::copyExpression(subscripts.front());
        subscripts.pop_front();
        unsigned int cnt = 0;
        for (it_subscript = subscripts.begin(); it_subscript != subscripts.end(); it_subscript++) {
          SgExpression * dim_size = SageInterface::copyExpression(data->getSections()[cnt++].second);
          subscript = SageBuilder::buildMultiplyOp(subscript, dim_size);
          subscript = SageBuilder::buildAddOp(subscript, SageInterface::copyExpression(*it_subscript));
        }

        SageInterface::replaceExpression(top_arr_ref, SageBuilder::buildPntrArrRefExp(SageInterface::copyExpression(var_ref), subscript));
      }
    }

    var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
      SgVarRefExp * var_ref = *it_var_ref;
      SgVariableSymbol * var_sym = var_ref->get_symbol();

      SgVariableSymbol * local_sym = NULL;

      it_sym_to_local = param_to_local.find(var_sym);
      if (it_sym_to_local != param_to_local.end())
        local_sym = it_sym_to_local->second;

      it_sym_to_local = coef_to_local.find(var_sym);
      if (it_sym_to_local != coef_to_local.end()) {
        assert(local_sym == NULL);

        local_sym = it_sym_to_local->second;
      }

      it_sym_to_local = data_sym_to_local.find(var_sym);
      if (it_sym_to_local != data_sym_to_local.end()) {
        assert(local_sym == NULL);

        local_sym = it_sym_to_local->second;
      }

      it_sym_to_local = iter_to_local.find(var_sym);
      if (it_sym_to_local != iter_to_local.end()) {
        assert(local_sym == NULL);

        local_sym = it_sym_to_local->second;
      }

      assert(local_sym != NULL); // implies VarRef to an unknown variable symbol

      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
    }
  }

  assert(result != NULL);

  return result;
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

void collectExpressions(LoopTrees::node_t * tree, std::set<SgExpression *> & exprs) {
  LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(tree);
  if (loop == NULL) return;

  exprs.insert(loop->lower_bound);
  exprs.insert(loop->upper_bound);
  if (loop->reduction_lhs != NULL)
    exprs.insert(loop->reduction_lhs);

  std::list<LoopTrees::node_t * >::const_iterator it_child;
  for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
    collectExpressions(*it_child, exprs);
}

void collectIteratorSymbols(LoopTrees::node_t * tree, std::set<SgVariableSymbol *> & symbols) {
  LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(tree);
  if (loop == NULL) return;

  symbols.insert(loop->iterator);

  std::list<LoopTrees::node_t * >::const_iterator it_child;
  for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
    collectIteratorSymbols(*it_child, symbols);
}

void collectReferencedSymbols(LoopTrees::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(tree);
  if (loop != NULL) {

    var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->lower_bound);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
      symbols.insert((*it_var_ref)->get_symbol());

    var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->upper_bound);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) 
      symbols.insert((*it_var_ref)->get_symbol());

    if (loop->reduction_lhs != NULL) {
      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->reduction_lhs);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());
    }

    if (go_down_children) {
      std::list<LoopTrees::node_t * >::const_iterator it_child;
      for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
        collectReferencedSymbols(*it_child, symbols);
    }
    
  }
  else {
    LoopTrees::stmt_t * stmt = dynamic_cast<LoopTrees::stmt_t *>(tree);
    assert(stmt != NULL);

    var_refs = SageInterface::querySubTree<SgVarRefExp>(stmt->statement);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
      symbols.insert((*it_var_ref)->get_symbol());
  }
}

}

}

