
#ifndef __LOOP_TREES_HPP__
#define __LOOP_TREES_HPP__

#include <set>
#include <map>
#include <list>
#include <utility>

#include <iostream>

class SgProject;
class SgExpression;
class SgStatement;
class SgVariableSymbol;

namespace KLT {

namespace Core {

class Data;

class LoopTrees {
  public:
    struct node_t {
      node_t * parent;

      virtual ~node_t();

      protected:
        node_t(); // Prevent construction of the base class
    };

    struct loop_t : public node_t {
      enum parallel_pattern_e {
        none,
        parfor,
        reduction
      };

      SgVariableSymbol * iterator;

      SgExpression * lower_bound;
      SgExpression * upper_bound;

      parallel_pattern_e parallel_pattern;
      SgExpression * reduction_lhs;

      std::list<node_t *> children;

      loop_t(
        SgVariableSymbol * it = NULL,
        SgExpression * lb = NULL,
        SgExpression * ub = NULL,
        parallel_pattern_e par_pattern = none,
        SgExpression * red_lhs = NULL
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

    /// Datas flowing in the sequence loop trees
    std::set<Data *> p_datas_in;

    /// Datas flowing out the sequence loop trees
    std::set<Data *> p_datas_out;

    /// Datas local to the sequence loop trees
    std::set<Data *> p_datas_local;

    /// Coefficiants (constant values) used in the sequence loop trees
    std::set<SgVariableSymbol *> p_coefficients;

    /// Parameters (constant integers not used in computation, array shape and loop sizes) of the sequence loop trees
    std::set<SgVariableSymbol *> p_parameters;

  public:
    const std::list<node_t *> & getTrees() const;

    const std::set<Data *> getDatasIn() const;
    const std::set<Data *> getDatasOut() const;
    const std::set<Data *> getDatasLocal() const;

    const std::set<SgVariableSymbol *> getCoefficients() const;
    const std::set<SgVariableSymbol *> getParameters() const;

  public:
    LoopTrees();
    virtual ~LoopTrees();

    /// Add a tree at the end of the list
    void addTree(node_t * tree);

    /// Add a data flowing in the sequence of loop trees
    void addDataIn(Data * data);

    /// Add a data flowing out the sequence of loop trees
    void addDataOut(Data * data);

    /// Add a data local to the sequence of loop trees
    void addDataLocal(Data * data);

    /// Add a coefficient of the sequence of loop trees
    void addCoefficient(SgVariableSymbol * var_sym);

    /// Add a parameter of the sequence of loop trees
    void addParameter(SgVariableSymbol * var_sym);

    /// Read from a lisp like text file
    void read(char * filename, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order);

    /// Read from a lisp like text file
    void read(std::ifstream & in_file, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order);

    /// Write a lisp like text
    void toText(char * filename) const;
    
    /// Write a lisp like text
    void toText(std::ostream & out) const;
};

SgExpression * translateConstExpression(
  SgExpression * expr, 
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
);

SgStatement * generateStatement(
  LoopTrees::node_t * node,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & coef_to_local,
  const std::map<Data *, SgVariableSymbol *>             & data_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local,
  bool generate_in_depth = false,
  bool flatten_array_ref = true
);

void collectLeaves(LoopTrees::node_t * tree, std::set<SgStatement *> & leaves);

void collectExpressions(LoopTrees::node_t * tree, std::set<SgExpression *> & exprs);

void collectIteratorSymbols(LoopTrees::node_t * tree, std::set<SgVariableSymbol *> & symbols);

void collectReferencedSymbols(LoopTrees::node_t * tree, std::set<SgVariableSymbol *> & symbols, bool go_down_children = true);

}

}

#endif /* __LOOP_TREES_HPP__ */

