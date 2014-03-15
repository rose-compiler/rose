
#ifndef __LOOP_TREES_HPP__
#define __LOOP_TREES_HPP__

#include "KLT/utils.hpp"

#include <set>
#include <map>
#include <list>
#include <vector>
#include <utility>

#include <iostream>
#include <fstream>

#include <cassert>

#include "sage3basic.h"
#include "AstFromString.h"

class SgProject;
class SgExpression;
class SgStatement;
class SgVariableSymbol;

namespace KLT {

/*!
 * \addtogroup grp_klt_looptree
 * @{
*/

template <class Annotation> class Data;

template <class Annotation>
class LoopTrees {
  public:
    struct node_t {
      node_t * parent;

      virtual ~node_t();

      protected:
        node_t(); // Prevent construction of the base class
    };

    struct loop_t : public node_t {
      SgVariableSymbol * iterator;

      SgExpression * lower_bound;
      SgExpression * upper_bound;

      std::vector<Annotation> annotations;

      std::list<node_t *> children;

      loop_t(
        SgVariableSymbol * it = NULL,
        SgExpression * lb = NULL,
        SgExpression * ub = NULL
      );
      ~loop_t();

      bool isDistributed() const;
    };

    struct stmt_t : public node_t {
      SgStatement * statement;

      stmt_t(SgStatement * stmt = NULL);
      virtual ~stmt_t();
    };

  private:
    static void toText(node_t * node, std::ostream & out, std::string indent);

  protected:
    /// List of loop tree in textual order
    std::list<node_t *> p_trees;

    /// Datas used by loop trees
    std::set<Data<Annotation> *> p_datas;

    /// Coefficiants (constant values) used in the sequence loop trees
    std::set<SgVariableSymbol *> p_scalars;

    /// Parameters (constant integers not used in computation, array shape and loop sizes) of the sequence loop trees
    std::set<SgVariableSymbol *> p_parameters;

  public:
    std::vector<Annotation> annotations;

  public:
    const std::list<node_t *> & getTrees() const;

    const std::set<Data<Annotation> *> getDatas() const;

    const std::set<SgVariableSymbol *> getScalars() const;
    const std::set<SgVariableSymbol *> getParameters() const;

  public:
    LoopTrees();
    virtual ~LoopTrees();

    /// Add a tree at the end of the list
    void addTree(node_t * tree);

    /// Add a data used by loop trees
    void addData(Data<Annotation> * data);

    /// Add a coefficient of the sequence of loop trees
    void addScalar(SgVariableSymbol * var_sym);

    /// Add a parameter of the sequence of loop trees
    void addParameter(SgVariableSymbol * var_sym);

    /// Read from a lisp like text file
    void read(char * filename);

    /// Read from a lisp like text file
    void read(std::ifstream & in_file);

    /// Write a lisp like text
    void toText(char * filename) const;
    
    /// Write a lisp like text
    void toText(std::ostream & out) const;
    
    unsigned numberLoops() const;
};

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
void parseParams(LoopTrees<Annotation> & loop_trees);

template <class Annotation>
void parseScalars(LoopTrees<Annotation> & loop_trees);

template <class Annotation>
void parseDatas(LoopTrees<Annotation> & loop_trees);

template <class Annotation>
void parseDataAnnotations(Data<Annotation> * data);

template <class Annotation>
void parseRegionAnnotations(LoopTrees<Annotation> & loop_trees);

template <class Annotation>
void parseLoopAnnotations(typename LoopTrees<Annotation>::loop_t * loop);

template <class Annotation>
typename LoopTrees<Annotation>::node_t * parseLoopTreesNode();

template <class Annotation>
void printLoopAnnotations(
  typename LoopTrees<Annotation>::loop_t * loop,
  std::ostream & out,
  std::string indent
);

//////////////////////////////////////////////////////////////////////////////////////

SgExpression * translateConstExpression(
  SgExpression * expr, 
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
);

template <class Annotation>
void collectLeaves(typename LoopTrees<Annotation>::node_t * tree, std::set<SgStatement *> & leaves);

template <class Annotation>
void collectExpressions(typename LoopTrees<Annotation>::node_t * tree, std::set<SgExpression *> & exprs);

template <class Annotation>
void collectReferencedSymbols(typename LoopTrees<Annotation>::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children = true);

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
LoopTrees<Annotation>::node_t::node_t() :
  parent(NULL)
{}

template <class Annotation>
LoopTrees<Annotation>::node_t::~node_t() {}

template <class Annotation>
LoopTrees<Annotation>::loop_t::loop_t(
  SgVariableSymbol * it,
  SgExpression * lb,
  SgExpression * ub
) :
  node_t(),
  iterator(it),
  lower_bound(lb),
  upper_bound(ub),
  annotations(),
  children()
{}

template <class Annotation>
LoopTrees<Annotation>::loop_t::~loop_t() {}

template <class Annotation>
LoopTrees<Annotation>::stmt_t::stmt_t(SgStatement * stmt) :
  node_t(),
  statement(stmt)
{}

template <class Annotation>
LoopTrees<Annotation>::stmt_t::~stmt_t() {}

template <class Annotation>
void LoopTrees<Annotation>::toText(node_t * node, std::ostream & out, std::string indent) {
  loop_t * loop = dynamic_cast<loop_t *>(node);
  stmt_t * stmt = dynamic_cast<stmt_t *>(node);
  
  assert(loop != NULL || stmt != NULL);
  
  if (loop != NULL) {
    out << indent << "loop(" << std::endl;
    out << indent << "  "
        << loop->iterator->get_name().getString() << ", "
        << loop->lower_bound->unparseToString()   << ", "
        << loop->upper_bound->unparseToString()   << ", ";

    printLoopAnnotations<Annotation>(loop, out, indent);
    
    typename std::list<node_t *>::const_iterator it_child = loop->children.begin();
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

template <class Annotation>
const std::list<typename LoopTrees<Annotation>::node_t *> & 
                    LoopTrees<Annotation>::getTrees() const { return p_trees; }

template <class Annotation>
const std::set<Data<Annotation> *> LoopTrees<Annotation>::getDatas() const { return p_datas; }

template <class Annotation>
const std::set<SgVariableSymbol *> LoopTrees<Annotation>::getScalars() const { return p_scalars; }

template <class Annotation>
const std::set<SgVariableSymbol *> LoopTrees<Annotation>::getParameters() const { return p_parameters; }

template <class Annotation>
LoopTrees<Annotation>::LoopTrees() :
  p_trees(),
  p_datas(),
  p_scalars(),
  p_parameters(),
  annotations()
{}

template <class Annotation>
LoopTrees<Annotation>::~LoopTrees() {}

template <class Annotation>
void LoopTrees<Annotation>::addTree(node_t * tree) { p_trees.push_back(tree); }

template <class Annotation>
void LoopTrees<Annotation>::addData(Data<Annotation> * data) { p_datas.insert(data); }

template <class Annotation>
void LoopTrees<Annotation>::addScalar(SgVariableSymbol * var_sym) { p_scalars.insert(var_sym); }

template <class Annotation>
void LoopTrees<Annotation>::addParameter(SgVariableSymbol * var_sym) { p_parameters.insert(var_sym); }

template <class Annotation>
void LoopTrees<Annotation>::toText(char * filename) const {
  std::ofstream file;
  file.open(filename);
  assert(file.is_open());
  toText(file);
  file.close();
}
   
template <class Annotation> 
void LoopTrees<Annotation>::toText(std::ostream & out) const {
  std::set<SgVariableSymbol *>::const_iterator it_sym;
  typename std::set<Data<Annotation> *>::const_iterator it_data;
  typename std::list<node_t *>::const_iterator it_tree;

  if (!p_parameters.empty()) {
    it_sym = p_parameters.begin();
    out << "params(" << (*it_sym)->get_name().getString();
    it_sym++;
    for (; it_sym != p_parameters.end(); it_sym++)
      out << ", " << (*it_sym)->get_name().getString();
    out << ")" << std::endl;
  }
  else assert(false);

  if (!p_scalars.empty()) {
    it_sym = p_scalars.begin();
    out << "scalars(" << (*it_sym)->get_name().getString();
    it_sym++;
    for (; it_sym != p_scalars.end(); it_sym++)
      out << ", " << (*it_sym)->get_name().getString();
    out << ")" << std::endl;
  }

  for (it_data = p_datas.begin(); it_data != p_datas.end(); it_data++)
    (*it_data)->toText(out);

  it_tree = p_trees.begin();
  out << "region(" << std::endl;
  toText(*it_tree, out, "  ");
  it_tree++;
  for (; it_tree != p_trees.end(); it_tree++) {
    out << "," << std::endl;
    toText(*it_tree, out, "  ");
  }
  out << std::endl << ")" << std::endl;
}

template <class Annotation> 
void LoopTrees<Annotation>::read(char * filename) {
  std::ifstream in_file;
  in_file.open(filename);

  assert(in_file.is_open());

  read(in_file);

  in_file.close();
}

template <class Annotation> 
void LoopTrees<Annotation>::read(std::ifstream & in_file) {
  initAstFromString(in_file);

  parseParams<Annotation>(*this);

  parseScalars<Annotation>(*this);

  parseDatas<Annotation>(*this);

  if (AstFromString::afs_match_substr("region")) {

    ensure('(');

    parseRegionAnnotations<Annotation>(*this);

    ensure(',');

    do {
      LoopTrees::node_t * lt_node = parseLoopTreesNode<Annotation>();
      assert(lt_node != NULL);
      addTree(lt_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    ensure(')');
  }
  else assert(false);

  SageBuilder::popScopeStack();
}

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
void parseParams(LoopTrees<Annotation> & loop_trees) {
  while (AstFromString::afs_match_substr("param")) {
    ensure('(');

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    ensure(',');

    assert(AstFromString::afs_match_type_specifier());
    SgType * param_type = dynamic_cast<SgType *>(AstFromString::c_parsed_node);
    assert(param_type != NULL);

    ensure(')');

    SgVariableDeclaration * param_decl = SageBuilder::buildVariableDeclaration_nfi(*label, param_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(param_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    loop_trees.addParameter(var_sym);
  }
  AstFromString::afs_skip_whitespace();
}

template <class Annotation>
void parseScalars(LoopTrees<Annotation> & loop_trees) {
  while (AstFromString::afs_match_substr("scalar")) {

    ensure('(');

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    ensure(',');

    assert(AstFromString::afs_match_type_specifier());
    SgType * coef_type = dynamic_cast<SgType *>(AstFromString::c_parsed_node);
    assert(coef_type != NULL);

    ensure(')');

    SgVariableDeclaration * coef_decl = SageBuilder::buildVariableDeclaration_nfi(*label, coef_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(coef_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    loop_trees.addScalar(var_sym);
  }
  AstFromString::afs_skip_whitespace();
}

template <class Annotation>
void parseDatas(LoopTrees<Annotation> & loop_trees) {
  while (AstFromString::afs_match_substr("data")) {

    ensure('(');

    assert(AstFromString::afs_match_identifier());
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);

    ensure(',');

    assert(AstFromString::afs_match_type_specifier(false));
    SgType * data_type = dynamic_cast<SgType *>(AstFromString::c_parsed_node);
    assert(data_type != NULL);

    ensure(',');

    std::list<std::pair<SgExpression *, SgExpression *> > sections;

    assert(AstFromString::afs_match_substr("section"));

    ensure('(');

    int nbr_dims;
    assert(AstFromString::afs_match_integer_const(&nbr_dims));
    assert(nbr_dims > 0);

    for (int i = 0; i < nbr_dims; i++) {
      ensure(',');

      sections.push_back(std::pair<SgExpression *, SgExpression *>(NULL, NULL));
      std::pair<SgExpression *, SgExpression *> & section = sections.back();

      AstFromString::afs_skip_whitespace();

      assert(AstFromString::afs_match_additive_expression());
      section.first = isSgExpression(AstFromString::c_parsed_node);
      assert(section.first != NULL);

      ensure(',');

      assert(AstFromString::afs_match_additive_expression());
      section.second = isSgExpression(AstFromString::c_parsed_node);
      assert(section.second != NULL);

      data_type = SageBuilder::buildPointerType(data_type);
    }

    ensure(')');

    SgVariableDeclaration * data_decl = SageBuilder::buildVariableDeclaration_nfi(*label, data_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(data_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    Data<Annotation> * data = new Data<Annotation>(var_sym);

    std::list<std::pair<SgExpression *, SgExpression *> >::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++)
      data->addSection(*it_section);

    ensure(',');

    parseDataAnnotations<Annotation>(data);

    ensure(')');

    loop_trees.addData(data);
  }
  AstFromString::afs_skip_whitespace();
}

template <class Annotation>
typename LoopTrees<Annotation>::node_t * parseLoopTreesNode() {
  typename LoopTrees<Annotation>::node_t * lt_node = NULL;
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

    typename LoopTrees<Annotation>::loop_t * lt_loop =
               new typename LoopTrees<Annotation>::loop_t(it_sym, lb_exp, ub_exp);

    ensure(',');

    parseLoopAnnotations<Annotation>(lt_loop);

    ensure(',');

    do {
      AstFromString::afs_skip_whitespace();

      typename LoopTrees<Annotation>::node_t * child_node = 
          parseLoopTreesNode<Annotation>();
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
    lt_node = new typename LoopTrees<Annotation>::stmt_t(stmt);

    ensure(')');
  }
  else assert(false);

  AstFromString::afs_skip_whitespace();
  return lt_node;
}

template <class Annotation>
void parseDataAnnotations(Data<Annotation> * data) {
  assert(Annotation::matchLabel());

  ensure('(');

  do {
    AstFromString::afs_skip_whitespace();

    Annotation::parseData(data->annotations);

    AstFromString::afs_skip_whitespace();
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class Annotation>
void parseRegionAnnotations(LoopTrees<Annotation> & loop_trees) {
  assert(Annotation::matchLabel());

  ensure('(');

  if (AstFromString::afs_match_char(')')) return;

  do {
    Annotation::parseRegion(loop_trees.annotations);
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class Annotation>
void parseLoopAnnotations(typename LoopTrees<Annotation>::loop_t * loop) {
  assert(Annotation::matchLabel());

  ensure('(');

  if (AstFromString::afs_match_char(')')) return;

  do {
    Annotation::parseLoop(loop->annotations);
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class Annotation>
void printLoopAnnotations(
  typename LoopTrees<Annotation>::loop_t * loop,
  std::ostream & out,
  std::string indent
) {
  assert(false);
}

template <class Annotation>
void collectLeaves(typename LoopTrees<Annotation>::node_t * tree, std::set<SgStatement *> & leaves) {
  typename LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(tree);
  if (loop != NULL) {
    typename std::list<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
    for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
      collectLeaves<Annotation>(*it_child, leaves);
    return;
  }

  typename LoopTrees<Annotation>::stmt_t * stmt = dynamic_cast<typename LoopTrees<Annotation>::stmt_t *>(tree);
  assert(stmt != NULL);

  leaves.insert(stmt->statement);
}

template <class Annotation>
void collectReferencedSymbols(typename LoopTrees<Annotation>::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  typename LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename LoopTrees<Annotation>::loop_t *>(tree);
  if (loop != NULL) {
    // If a loop is distributed (have gang, worker, or vector annotation) bounds will evaluated before launching the kernel.
    if (!loop->isDistributed()) {
      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->lower_bound);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());

      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->upper_bound);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) 
        symbols.insert((*it_var_ref)->get_symbol());
    }

    if (go_down_children) {
      typename std::list<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
      for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
        collectReferencedSymbols<Annotation>(*it_child, symbols);
    }
  }
  else {
    typename LoopTrees<Annotation>::stmt_t * stmt = dynamic_cast<typename LoopTrees<Annotation>::stmt_t *>(tree);
    assert(stmt != NULL);

    var_refs = SageInterface::querySubTree<SgVarRefExp>(stmt->statement);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
      symbols.insert((*it_var_ref)->get_symbol());
  }
}

template <class Annotation> 
unsigned LoopTrees<Annotation>::numberLoops() const {
  std::list<loop_t *> loops;
  typename std::list<node_t *>::const_iterator it_node;
  for (it_node = p_trees.begin(); it_node != p_trees.end(); it_node++) {
    loop_t * loop = dynamic_cast<loop_t *>(*it_node);
    if (loop != NULL)
      loops.push_back(loop);
  }

  typename std::list<loop_t *>::iterator it_loop = loops.begin();
  while (it_loop != loops.end()) {
    for (it_node = (*it_loop)->children.begin(); it_node != (*it_loop)->children.end(); it_node++) {
      loop_t * loop = dynamic_cast<loop_t *>(*it_node);
      if (loop != NULL)
        loops.push_back(loop);
    }
    it_loop++;
  }

  return loops.size();
}

/** @} */

}

#endif /* __LOOP_TREES_HPP__ */

