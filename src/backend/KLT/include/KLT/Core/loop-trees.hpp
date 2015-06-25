
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
    size_t id;
  private:
    static size_t id_cnt;
  public:
    struct node_t {
      node_t * parent;

      node_t() : parent(NULL) {}
      virtual ~node_t() {}
    };

    struct block_t : public node_t {
      std::vector<node_t *> children;

      block_t() : node_t(), children() {}
      virtual ~block_t() {}
    };

    struct cond_t : public node_t {
      SgExpression * condition;

      block_t * block_true;
      block_t * block_false;

      cond_t(SgExpression * cond = NULL) : node_t(), condition(cond), block_true(NULL), block_false(NULL) {};
      virtual ~cond_t() {}
    };

    struct loop_t : public node_t {
      size_t id;

      SgVariableSymbol * iterator;

      SgExpression * lower_bound;
      SgExpression * upper_bound;
      SgExpression * stride;

      std::vector<Annotation> annotations;

      block_t * block;

      loop_t(
        size_t id_,
        SgVariableSymbol * it = NULL,
        SgExpression * lb = NULL,
        SgExpression * ub = NULL,
        SgExpression * stride_ = NULL
      ) :
        node_t(),
        id(id_),
        iterator(it),
        lower_bound(lb),
        upper_bound(ub),
        stride(stride_),
        annotations(),
        block(NULL)
      {}
      virtual ~loop_t() {}

      bool isDistributed() const;
      bool isSplitted() const;
    };

    struct tile_t : public node_t {
      size_t id;
      size_t order;

      loop_t * loop;
      tile_t * tile;
      block_t * block;

      SgVariableSymbol * iterator_sym;

      tile_t(
        size_t id_, size_t order_, SgVariableSymbol * iterator_sym_, loop_t * loop_
      ) :
        id(id_), order(order_), node_t(), loop(loop_),
        tile(NULL), block(NULL), iterator_sym(iterator_sym_)
      {}

      virtual ~tile_t() {}
    };

    struct stmt_t : public node_t {
      SgStatement * statement;

      stmt_t(SgStatement * stmt = NULL) : node_t(), statement(stmt) {}
      virtual ~stmt_t() {}
    };

  protected:
    /// List of loop tree in textual order
    std::vector<node_t *> p_trees;

    /// All loops in text order
    std::vector<loop_t *> p_loops;

    /// All node in text order
    std::vector<node_t *> p_nodes;

    /// Datas used by loop trees
    std::vector<Data<Annotation> *> p_datas;

    /// Private datas used by loop trees
    std::vector<Data<Annotation> *> p_privates;

    /// Coefficiants (constant values) used in the sequence loop trees
    std::vector<SgVariableSymbol *> p_scalars;

    /// Parameters (constant integers not used in computation, array shape and loop sizes) of the sequence loop trees
    std::vector<SgVariableSymbol *> p_parameters;

    void registerLoopsAndNodes(typename LoopTrees<Annotation>::node_t * node) {
      assert(node != NULL);

      p_nodes.push_back(node);

      typename ::KLT::LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::cond_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::block_t * block = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::block_t *>(node);
      typename ::KLT::LoopTrees<Annotation>::stmt_t  * stmt  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::stmt_t  *>(node);

      if (loop != NULL) {
        p_loops.push_back(loop);
        registerLoopsAndNodes(loop->block);
        loop->block->parent = loop;
      }
      else if (cond != NULL) {
        if (cond->block_true != NULL) {
          registerLoopsAndNodes(cond->block_true);
          cond->block_true->parent = cond;
        }
        if (cond->block_false != NULL) {
          registerLoopsAndNodes(cond->block_false);
          cond->block_false->parent = cond;
        }
      }
      else if (block != NULL) {
        typename std::vector<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
        for (it_child = block->children.begin(); it_child != block->children.end(); it_child++) {
          registerLoopsAndNodes(*it_child);
          (*it_child)->parent = block;
        }
      }
      else assert(stmt != NULL);
    }

  public:
    std::vector<Annotation> annotations;

  public:
    const std::vector<node_t *> & getTrees() const { return p_trees; }

    const std::vector<Data<Annotation> *> & getDatas() const { return p_datas; }
    size_t getNumDatas() const { return p_datas.size(); }

    const std::vector<Data<Annotation> *> & getPrivates() const { return p_privates; }
    size_t getNumPrivates() const { return p_privates.size(); }

    const std::vector<SgVariableSymbol *> & getScalars() const { return p_scalars; }
    size_t getNumScalars() const { return p_scalars.size(); }

    const std::vector<SgVariableSymbol *> & getParameters() const { return p_parameters; }
    size_t getNumParameters() const { return p_parameters.size(); }
    
    size_t getNumberLoops() const { return p_loops.size(); }
    const std::vector<loop_t *> & getLoops() const { return p_loops; }
    size_t getLoopID(loop_t * loop) const {
      typename std::vector<loop_t *>::const_iterator it_loop = std::find(p_loops.begin(), p_loops.end(), loop);
      assert(it_loop != p_loops.end());
      return it_loop - p_loops.begin();
    }

    const std::vector<node_t *> & getNodes() const { return p_nodes; }

  public:
    LoopTrees();
    virtual ~LoopTrees();

    /// Add a tree at the end of the list
    void addTree(node_t * tree) {
      p_trees.push_back(tree);
      registerLoopsAndNodes(tree);
    }

    /// Add a data used by loop trees
    void addData(Data<Annotation> * data) { if (std::find(p_datas.begin(), p_datas.end(), data) == p_datas.end()) p_datas.push_back(data); }

    /// Add a data used by loop trees
    void addPrivate(Data<Annotation> * data) { if (std::find(p_privates.begin(), p_privates.end(), data) == p_privates.end()) p_privates.push_back(data); }

    /// Add a coefficient of the sequence of loop trees
    void addScalar(SgVariableSymbol * var_sym) { if (std::find(p_scalars.begin(), p_scalars.end(), var_sym) == p_scalars.end()) p_scalars.push_back(var_sym); }

    /// Add a parameter of the sequence of loop trees
    void addParameter(SgVariableSymbol * var_sym) { if (std::find(p_parameters.begin(), p_parameters.end(), var_sym) == p_parameters.end()) p_parameters.push_back(var_sym); }

    /// Read from a lisp like text file
    void read(char * filename);
    void read(const std::string & filename);
    void read(std::ifstream & in_file);

    static void toText(node_t * node, std::ostream & out, std::string indent);

    /// Write a lisp like text
    void toText(char * filename) const;
    
    /// Write a lisp like text
    void toText(std::ostream & out) const;
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
void printAnnotations(
  const std::vector<Annotation> & annotations,
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
void LoopTrees<Annotation>::toText(node_t * node, std::ostream & out, std::string indent) {
  if (node == NULL) return;

  loop_t  * loop  = dynamic_cast<loop_t  *>(node);
  tile_t  * tile  = dynamic_cast<tile_t  *>(node);
  cond_t  * cond  = dynamic_cast<cond_t  *>(node);
  block_t * block = dynamic_cast<block_t *>(node);
  stmt_t  * stmt  = dynamic_cast<stmt_t  *>(node);
  
  assert(loop != NULL || tile != NULL || cond != NULL || block != NULL || stmt != NULL);
  
  if (loop != NULL) {
    out << indent << "loop(" << std::endl;
    out << indent << "  "
        << loop->id << ", "
        << loop->iterator->get_name().getString() << ", "
        << loop->lower_bound->unparseToString()   << ", "
        << loop->upper_bound->unparseToString()   << ", "
        << loop->stride->unparseToString()   << ", ";

    printAnnotations<Annotation>(loop->annotations, out, indent);
    
    toText(loop->block, out, indent + "  ");

    out << std::endl << indent << ")";
  }
  
  if (tile != NULL) {
    out << indent << "tile(" << std::endl;
    out << indent << "  " << tile->id << ", " << tile->iterator_sym->get_name().getString() << ", " << std::endl
        << indent << "  " << tile->loop->id << ", " << tile->loop->iterator->get_name().getString() << std::endl
        << indent << "  " << tile->order << ", " << std::endl;
    
    if (tile->tile != NULL)
      toText(tile->tile, out, indent + "  ");
    else
      toText(tile->block, out, indent + "  ");

    out << std::endl << indent << ")";
  }
  
  if (cond != NULL) {
    out << indent << "cond(" << std::endl;
    out << indent << "  " << cond->condition->unparseToString() << "," << std::endl;
    
    toText(cond->block_true , out, indent + "  ");
    out << "," << std::endl;
      
    toText(cond->block_false, out, indent + "  ");
    out << std::endl << indent << ")";
  }
  
  if (block != NULL) {
    assert(!block->children.empty());
    out << indent << "block(" << std::endl;

    typename std::vector<node_t *>::const_iterator it_block = block->children.begin();
    toText(*it_block, out, indent + "  ");
    it_block++;
    for (; it_block != block->children.end(); it_block++) {
      out << "," << std::endl;
      toText(*it_block, out, indent + "  ");
    }
    out << std::endl << indent << ")";
  }
  
  if (stmt != NULL) {
    out << indent << "stmt(" << stmt->statement->unparseToString() << ")";
  }
}

template <class Annotation>
LoopTrees<Annotation>::LoopTrees() :
  id(id_cnt++),
  p_trees(),
  p_loops(),
  p_nodes(),
  p_datas(),
  p_scalars(),
  p_parameters(),
  annotations()
{}

template <class Annotation>
LoopTrees<Annotation>::~LoopTrees() {}

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
  std::vector<SgVariableSymbol *>::const_iterator it_sym;
  typename std::vector<Data<Annotation> *>::const_iterator it_data;
  typename std::vector<node_t *>::const_iterator it_tree;

  for (it_sym = p_parameters.begin(); it_sym != p_parameters.end(); it_sym++)
    out << "param(" << (*it_sym)->get_name().getString() << "," << SageInterface::get_name((*it_sym)->get_type()) << ")" << std::endl;

  for (it_sym = p_scalars.begin(); it_sym != p_scalars.end(); it_sym++)
    out << "scalar(" << (*it_sym)->get_name().getString() << "," << SageInterface::get_name((*it_sym)->get_type()) << ")" << std::endl;

  for (it_data = p_datas.begin(); it_data != p_datas.end(); it_data++)
    (*it_data)->toText(out);

  assert(!p_trees.empty());

  it_tree = p_trees.begin();
  out << "region(" << std::endl;
  printAnnotations<Annotation>(annotations, out, "  ");
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
void LoopTrees<Annotation>::read(const std::string & filename) {
  std::ifstream in_file;
  in_file.open(filename.c_str());

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
      node_t * lt_node = parseLoopTreesNode<Annotation>();
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

    std::list<typename KLT::Data<Annotation>::section_t> sections;

    assert(AstFromString::afs_match_substr("section"));

    ensure('(');

    int nbr_dims;
    assert(AstFromString::afs_match_integer_const(&nbr_dims));
    assert(nbr_dims > 0);

    for (int i = 0; i < nbr_dims; i++) {
      ensure(',');

      sections.push_back(typename KLT::Data<Annotation>::section_t());
      typename KLT::Data<Annotation>::section_t & section = sections.back();

      AstFromString::afs_skip_whitespace();

      if (AstFromString::afs_match_additive_expression())
        section.lower_bound = isSgExpression(AstFromString::c_parsed_node);
      else
        section.lower_bound = NULL;

      ensure(',');

      if (AstFromString::afs_match_additive_expression())
        section.size = isSgExpression(AstFromString::c_parsed_node);
      else
        section.size = NULL;

      ensure(',');

      if (AstFromString::afs_match_additive_expression())
        section.stride = isSgExpression(AstFromString::c_parsed_node);
      else
        section.stride = NULL;

      data_type = SageBuilder::buildPointerType(data_type);
    }

    ensure(')');

    SgVariableDeclaration * data_decl = SageBuilder::buildVariableDeclaration_nfi(*label, data_type, NULL, NULL);
    SgVariableSymbol * var_sym = isSgVariableSymbol(data_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    assert(var_sym != NULL);

    Data<Annotation> * data = new Data<Annotation>(var_sym);

    typename std::list<typename KLT::Data<Annotation>::section_t>::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++) {
      typename Data<Annotation>::section_t section;
        section.lower_bound = it_section->lower_bound;
        section.size = it_section->size;
        section.stride = it_section->stride;
      data->addSection(section);
    }

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

    SgVariableSymbol * it_sym = NULL;
    assert(AstFromString::afs_match_identifier());
    SgVarRefExp * expr = isSgVarRefExp(AstFromString::c_parsed_node);
    if (expr != NULL)
      it_sym = isSgVariableSymbol(expr->get_symbol());
    else {
      SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
      assert(label != NULL);

      SgVariableDeclaration * it_decl = SageBuilder::buildVariableDeclaration_nfi(*label, SageBuilder::buildLongType(), NULL, NULL);
      it_sym = isSgVariableSymbol(it_decl->get_variables()[0]->search_for_symbol_from_symbol_table());
    }
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

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * stride_exp = isSgExpression(AstFromString::c_parsed_node);
    assert(stride_exp != NULL);

    typename LoopTrees<Annotation>::loop_t * lt_loop =
               new typename LoopTrees<Annotation>::loop_t(it_sym, lb_exp, ub_exp, stride_exp);

    ensure(',');

    parseLoopAnnotations<Annotation>(lt_loop);

    ensure(',');

    typename LoopTrees<Annotation>::node_t * child_block = parseLoopTreesNode<Annotation>();
    lt_loop->block = dynamic_cast<typename LoopTrees<Annotation>::block_t *>(child_block);
    assert(lt_loop->block != NULL);

    lt_node = lt_loop;

    ensure(')');
  }
  else if (AstFromString::afs_match_substr("cond")) {

    ensure('(');

    assert(AstFromString::afs_match_additive_expression());
    SgExpression * condition = isSgExpression(AstFromString::c_parsed_node);
    assert(condition != NULL);

    typename LoopTrees<Annotation>::cond_t * lt_cond = new typename LoopTrees<Annotation>::cond_t(condition);

    ensure(',');

    typename LoopTrees<Annotation>::node_t * block_true_node = parseLoopTreesNode<Annotation>();
    lt_cond->block_true = dynamic_cast<typename LoopTrees<Annotation>::block_t *>(block_true_node);
    assert(lt_cond->block_true != NULL);

    ensure(',');

    typename LoopTrees<Annotation>::node_t * block_false_node = parseLoopTreesNode<Annotation>();
    lt_cond->block_false = dynamic_cast<typename LoopTrees<Annotation>::block_t *>(block_false_node);
    assert(lt_cond->block_false != NULL);

    lt_node = lt_cond;

    ensure(')');
  }
  else if (AstFromString::afs_match_substr("block")) {

    ensure('(');

    typename LoopTrees<Annotation>::block_t * lt_block = new typename LoopTrees<Annotation>::block_t();

    do {
      AstFromString::afs_skip_whitespace();

      typename LoopTrees<Annotation>::node_t * child_node = parseLoopTreesNode<Annotation>();
      assert(child_node != NULL);
      lt_block->children.push_back(child_node);

      AstFromString::afs_skip_whitespace();
    } while (AstFromString::afs_match_char(','));

    lt_node = lt_block;

    ensure(')');
  }
  else if (AstFromString::afs_match_substr("stmt")) {

    ensure('(');

    SgStatement * stmt = NULL;
    if (AstFromString::afs_match_statement()) {
      stmt = isSgStatement(AstFromString::c_parsed_node);
      assert(stmt != NULL);
    }
    else if (AstFromString::afs_match_declaration()) {
      stmt = isSgVariableDeclaration(AstFromString::c_parsed_node);
      assert(stmt != NULL);
    }
    else {
      std::cerr << AstFromString::c_char << std::endl;
      assert(false);
    }

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
void collectLeaves(typename LoopTrees<Annotation>::node_t * tree, std::set<SgStatement *> & leaves) {
  if (tree == NULL) return;

  typename LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename LoopTrees<Annotation>::loop_t  *>(tree);
  typename LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename LoopTrees<Annotation>::cond_t  *>(tree);
  typename LoopTrees<Annotation>::block_t * block = dynamic_cast<typename LoopTrees<Annotation>::block_t *>(tree);
  typename LoopTrees<Annotation>::stmt_t  * stmt  = dynamic_cast<typename LoopTrees<Annotation>::stmt_t  *>(tree);

  if (loop != NULL) {
    collectLeaves<Annotation>(loop->block, leaves);
  }
  else if (cond != NULL) {
    collectLeaves<Annotation>(cond->block_true, leaves);
    collectLeaves<Annotation>(cond->block_false, leaves);
  }
  else if (block != NULL) {
    typename std::vector<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      collectLeaves<Annotation>(*it_child, leaves);
  }
  else if (stmt != NULL) {
    leaves.insert(stmt->statement);
  }
  else assert(false);
}

template <class Annotation>
void collectReferencedSymbols(typename LoopTrees<Annotation>::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children) {
  if (tree == NULL) return;

  typename LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename LoopTrees<Annotation>::loop_t  *>(tree);
  typename LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename LoopTrees<Annotation>::cond_t  *>(tree);
  typename LoopTrees<Annotation>::block_t * block = dynamic_cast<typename LoopTrees<Annotation>::block_t *>(tree);
  typename LoopTrees<Annotation>::stmt_t  * stmt  = dynamic_cast<typename LoopTrees<Annotation>::stmt_t  *>(tree);

  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  if (loop != NULL) {
    // If a loop is distributed, bounds are computed by the host.
    if (!loop->isDistributed()) {
      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->lower_bound);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());

      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->upper_bound);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());

      var_refs = SageInterface::querySubTree<SgVarRefExp>(loop->stride);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());
    }

    if (go_down_children)
      collectReferencedSymbols<Annotation>(loop->block, symbols);
  }
  else if (cond != NULL) {
    var_refs = SageInterface::querySubTree<SgVarRefExp>(cond->condition);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
      symbols.insert((*it_var_ref)->get_symbol());

    if (go_down_children) {
      collectReferencedSymbols<Annotation>(cond->block_true, symbols);
      collectReferencedSymbols<Annotation>(cond->block_false, symbols);
    }
  }
  else if (block != NULL) {
    typename std::vector<typename LoopTrees<Annotation>::node_t *>::const_iterator it;
    for (it = block->children.begin(); it != block->children.end(); it++)
      collectReferencedSymbols<Annotation>(*it, symbols);
  }
  else if (stmt != NULL) {
    var_refs = SageInterface::querySubTree<SgVarRefExp>(stmt->statement);
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
      symbols.insert((*it_var_ref)->get_symbol());
  }
  else assert(false);
}

/** @} */

}

#endif /* __LOOP_TREES_HPP__ */

