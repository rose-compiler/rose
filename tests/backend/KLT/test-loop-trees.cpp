
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include <cassert>

#include "AstFromString.h"

void initAstFromString(std::ifstream & in_file) {
  SgScopeStatement * scope = SageBuilder::buildBasicBlock();
  AstFromString::c_sgnode = scope;
  SageBuilder::pushScopeStack(scope);

  in_file.seekg (0, in_file.end);
  int length = in_file.tellg();
  in_file.seekg (0, in_file.beg);

  char * tmp_char_str = new char [length + 1];
  in_file.read(tmp_char_str, length);
  tmp_char_str[length] = 0;

  AstFromString::c_char = tmp_char_str;
}

KLT::Core::LoopTrees::node_t * parseLoopTreesNode() {
  KLT::Core::LoopTrees::node_t * lt_node = NULL;
  AstFromString::afs_skip_whitespace();

  if (AstFromString::afs_match_substr("loop")) {
    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char('('));
    AstFromString::afs_skip_whitespace();

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    SgVariableDeclaration * it_decl = SageBuilder::buildVariableDeclaration_nfi(*label, SageBuilder::buildUnsignedLongType(), NULL, NULL);
    SgVariableSymbol * it_sym = isSgVariableSymbol(it_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(it_sym != NULL);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(','));
    AstFromString::afs_skip_whitespace();

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * lb_exp = isSgExpression(AstFromString::c_parsed_node);
    assert(lb_exp != NULL);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(','));
    AstFromString::afs_skip_whitespace();

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * ub_exp = isSgExpression(AstFromString::c_parsed_node);
    assert(ub_exp != NULL);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(','));
    AstFromString::afs_skip_whitespace();

    KLT::Core::LoopTrees::loop_t::parallel_pattern_e pattern;
    SgExpression * parameter = NULL;
    if (AstFromString::afs_match_substr("none", false))
      pattern = KLT::Core::LoopTrees::loop_t::none;
    else if (AstFromString::afs_match_substr("parfor", false))
      pattern = KLT::Core::LoopTrees::loop_t::parfor;
    else if (AstFromString::afs_match_substr("reduction")) {
      pattern = KLT::Core::LoopTrees::loop_t::reduction;

      AstFromString::afs_skip_whitespace();
      assert(AstFromString::afs_match_char('('));
      AstFromString::afs_skip_whitespace();

      assert(AstFromString::afs_match_additive_expression());
      parameter = isSgExpression(AstFromString::c_parsed_node);
      assert(parameter != NULL);

      AstFromString::afs_skip_whitespace();
      assert(AstFromString::afs_match_char(')'));
      AstFromString::afs_skip_whitespace();
    }
    else assert(false);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(','));
    AstFromString::afs_skip_whitespace();

    KLT::Core::LoopTrees::loop_t * lt_loop = new KLT::Core::LoopTrees::loop_t(it_sym, lb_exp, ub_exp, pattern, parameter);

    do {
      AstFromString::afs_skip_whitespace();

      KLT::Core::LoopTrees::node_t * child_node = parseLoopTreesNode();
      assert(child_node != NULL);
      lt_loop->children.push_back(child_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    lt_node = lt_loop;

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(')'));
    AstFromString::afs_skip_whitespace();
  }
  else if (AstFromString::afs_match_substr("stmt")) {
    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char('('));
    AstFromString::afs_skip_whitespace();

    assert(AstFromString::afs_match_statement());
    SgStatement * stmt = isSgStatement(AstFromString::c_parsed_node);
    assert(stmt != NULL);
    stmt->set_parent(AstFromString::c_sgnode);
    lt_node = new KLT::Core::LoopTrees::stmt_t(stmt);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(')'));
    AstFromString::afs_skip_whitespace();
  }
  else assert(false);

  AstFromString::afs_skip_whitespace();
  return lt_node;
}

void read_loop_trees(KLT::Core::LoopTrees & loop_trees, std::ifstream & in_file) {
  initAstFromString(in_file);

  AstFromString::afs_skip_whitespace();

  if (AstFromString::afs_match_substr("params")) {
    SgType * param_type = SageBuilder::buildUnsignedLongType();

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char('('));

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
    assert(AstFromString::afs_match_char(')'));
  }
  AstFromString::afs_skip_whitespace();

  if (AstFromString::afs_match_substr("coefs")) {
    SgType * coef_type = SageBuilder::buildFloatType();
    assert(AstFromString::afs_match_char('('));
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
    assert(AstFromString::afs_match_char(')'));
  }
  AstFromString::afs_skip_whitespace();

  while (AstFromString::afs_match_substr("data")) {
    assert(AstFromString::afs_match_char('('));
    AstFromString::afs_skip_whitespace();

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
      assert(AstFromString::afs_match_char(']'));

      data_type = SageBuilder::buildPointerType(data_type);
    }

    SgVariableDeclaration * data_decl = SageBuilder::buildVariableDeclaration_nfi(*label, data_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(data_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    KLT::Core::Data * data = new KLT::Core::Data(var_sym);

    std::list<std::pair<SgExpression *, SgExpression *> >::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++)
      data->addSection(*it_section);

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(','));
    AstFromString::afs_skip_whitespace();

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

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(')'));
  }
  AstFromString::afs_skip_whitespace();

  if (AstFromString::afs_match_substr("loop-trees")) {
    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char('('));
    AstFromString::afs_skip_whitespace();

    do {
      KLT::Core::LoopTrees::node_t * lt_node = parseLoopTreesNode();
      assert(lt_node != NULL);
      loop_trees.addTree(lt_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    AstFromString::afs_skip_whitespace();
    assert(AstFromString::afs_match_char(')'));
    AstFromString::afs_skip_whitespace();
  }
  else assert(false);

  AstFromString::afs_skip_whitespace();
}

int main(int argc, char ** argv) {
  assert(argc == 3);

  KLT::Core::LoopTrees loop_trees;

  std::ifstream in_file;
  in_file.open(argv[1]);

  assert(in_file.is_open());

  read_loop_trees(loop_trees, in_file);

  loop_trees.toText(argv[2]);

  return 0;
}

