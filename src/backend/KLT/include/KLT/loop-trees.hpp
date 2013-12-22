
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
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

      std::vector<LoopAnnotation> annotations;

      std::list<node_t *> children;

      loop_t(
        SgVariableSymbol * it = NULL,
        SgExpression * lb = NULL,
        SgExpression * ub = NULL
      );
      virtual ~loop_t();
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
    std::set<Data<DataAnnotation> *> p_datas;

    /// Coefficiants (constant values) used in the sequence loop trees
    std::set<SgVariableSymbol *> p_scalar;

    /// Parameters (constant integers not used in computation, array shape and loop sizes) of the sequence loop trees
    std::set<SgVariableSymbol *> p_parameters;

  public:
    std::vector<RegionAnnotation> annotations;

  public:
    const std::list<node_t *> & getTrees() const;

    const std::set<Data<DataAnnotation> *> getDatas() const;

    const std::set<SgVariableSymbol *> getScalar() const;
    const std::set<SgVariableSymbol *> getParameters() const;

  public:
    LoopTrees();
    virtual ~LoopTrees();

    /// Add a tree at the end of the list
    void addTree(node_t * tree);

    /// Add a data used by loop trees
    void addData(Data<DataAnnotation> * data);

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
};

//////////////////////////////////////////////////////////////////////////////////////

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseParams(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseScalars(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseDatas(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees);

template <class DataAnnotation>
void parseDataAnnotations(Data<DataAnnotation> * data);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseRegionAnnotations(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseLoopAnnotations(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * parseLoopTreesNode();

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void printLoopAnnotations(
  typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop,
  std::ostream & out,
  std::string indent
);

//////////////////////////////////////////////////////////////////////////////////////

SgExpression * translateConstExpression(
  SgExpression * expr, 
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
SgStatement * generateStatement(
  typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * node,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & coef_to_local,
  const std::map<Data<DataAnnotation> *, SgVariableSymbol *> & data_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local,
  bool generate_in_depth = false,
  bool flatten_array_ref = true
);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void collectLeaves(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree, std::set<SgStatement *> & leaves);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void collectExpressions(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree, std::set<SgExpression *> & exprs);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void collectIteratorSymbols(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree, std::set<SgVariableSymbol *> & symbols);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void collectReferencedSymbols(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children = true);

//////////////////////////////////////////////////////////////////////////////////////

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t::node_t() :
  parent(NULL)
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t::~node_t() {}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t::loop_t(
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t::~loop_t() {}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::stmt_t::stmt_t(SgStatement * stmt) :
  node_t(),
  statement(stmt)
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::stmt_t::~stmt_t() {}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::toText(node_t * node, std::ostream & out, std::string indent) {
  loop_t * loop = dynamic_cast<loop_t *>(node);
  stmt_t * stmt = dynamic_cast<stmt_t *>(node);
  
  assert(loop != NULL || stmt != NULL);
  
  if (loop != NULL) {
    out << indent << "loop(" << std::endl;
    out << indent << "  "
        << loop->iterator->get_name().getString() << ", "
        << loop->lower_bound->unparseToString()   << ", "
        << loop->upper_bound->unparseToString()   << ", ";

    printLoopAnnotations<DataAnnotation, RegionAnnotation, LoopAnnotation>(loop, out, indent);
    
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
const std::list<typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t *> & 
                    LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::getTrees() const { return p_trees; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
const std::set<Data<DataAnnotation> *> LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::getDatas() const { return p_datas; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
const std::set<SgVariableSymbol *> LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::getScalar() const { return p_scalar; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
const std::set<SgVariableSymbol *> LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::getParameters() const { return p_parameters; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::LoopTrees() :
  p_trees(),
  p_datas(),
  p_scalar(),
  p_parameters(),
  annotations()
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::~LoopTrees() {}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::addTree(node_t * tree) { p_trees.push_back(tree); }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::addData(Data<DataAnnotation> * data) { p_datas.insert(data); }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::addScalar(SgVariableSymbol * var_sym) { p_scalar.insert(var_sym); }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::addParameter(SgVariableSymbol * var_sym) { p_parameters.insert(var_sym); }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::toText(char * filename) const {
  std::ofstream file;
  file.open(filename);
  assert(file.is_open());
  toText(file);
  file.close();
}
   
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> 
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::toText(std::ostream & out) const {
  std::set<SgVariableSymbol *>::const_iterator it_sym;
  typename std::set<Data<DataAnnotation> *>::const_iterator it_data;
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

  if (!p_scalar.empty()) {
    it_sym = p_scalar.begin();
    out << "scalars(" << (*it_sym)->get_name().getString();
    it_sym++;
    for (; it_sym != p_scalar.end(); it_sym++)
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> 
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::read(char * filename) {
  std::ifstream in_file;
  in_file.open(filename);

  assert(in_file.is_open());

  read(in_file);

  in_file.close();
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> 
void LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::read(std::ifstream & in_file) {
  initAstFromString(in_file);

  parseParams<DataAnnotation, RegionAnnotation, LoopAnnotation>(*this);

  parseScalars<DataAnnotation, RegionAnnotation, LoopAnnotation>(*this);

  parseDatas<DataAnnotation, RegionAnnotation, LoopAnnotation>(*this);

  if (AstFromString::afs_match_substr("region")) {

    ensure('(');

    parseRegionAnnotations<DataAnnotation, RegionAnnotation, LoopAnnotation>(*this);

    ensure(',');

    do {
      LoopTrees::node_t * lt_node = parseLoopTreesNode<DataAnnotation, RegionAnnotation, LoopAnnotation>();
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseParams(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees) {
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseScalars(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees) {
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

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseDatas(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees) {
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

    Data<DataAnnotation> * data = new Data<DataAnnotation>(var_sym);

    std::list<std::pair<SgExpression *, SgExpression *> >::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++)
      data->addSection(*it_section);

    ensure(',');

    parseDataAnnotations<DataAnnotation>(data);

    ensure(')');

    loop_trees.addData(data);
  }
  AstFromString::afs_skip_whitespace();
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * parseLoopTreesNode() {
  typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * lt_node = NULL;
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

    typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * lt_loop =
               new typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t(it_sym, lb_exp, ub_exp);

    ensure(',');

    parseLoopAnnotations<DataAnnotation, RegionAnnotation, LoopAnnotation>(lt_loop);

    ensure(',');

    do {
      AstFromString::afs_skip_whitespace();

      typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * child_node = 
          parseLoopTreesNode<DataAnnotation, RegionAnnotation, LoopAnnotation>();
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
    lt_node = new typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::stmt_t(stmt);

    ensure(')');
  }
  else assert(false);

  AstFromString::afs_skip_whitespace();
  return lt_node;
}

template <class DataAnnotation>
void parseDataAnnotations(Data<DataAnnotation> * data) {
  assert(DataAnnotation::matchLabel());

  ensure('(');

  do {
    AstFromString::afs_skip_whitespace();

    DataAnnotation::parse(data->annotations);

    AstFromString::afs_skip_whitespace();
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseRegionAnnotations(LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees) {
  assert(RegionAnnotation::matchLabel());

  ensure('(');

  do {
    RegionAnnotation::parse(loop_trees.annotations);
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void parseLoopAnnotations(typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop) {
  assert(LoopAnnotation::matchLabel());

  ensure('(');

  do {
    LoopAnnotation::parse(loop->annotations);
  } while (AstFromString::afs_match_char(','));

  ensure(')');
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation>
void printLoopAnnotations(
  typename LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::loop_t * loop,
  std::ostream & out,
  std::string indent
) {
  assert(false);
}

/** @} */

}

#endif /* __LOOP_TREES_HPP__ */

