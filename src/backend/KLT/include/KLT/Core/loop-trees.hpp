
#ifndef __LOOP_TREES_HPP__
#define __LOOP_TREES_HPP__

#include <set>
#include <list>
#include <utility>

#include <iostream>

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
    void read(char * filename);

    /// Read from a lisp like text file
    void read(std::ifstream & in_file);

    /// Write a lisp like text
    void toText(char * filename) const;
    
    /// Write a lisp like text
    void toText(std::ostream & out) const;

  friend class Generator;
};

void collectLeaves(LoopTrees::node_t * tree, std::set<SgStatement *> & leaves);

}

}

#endif /* __LOOP_TREES_HPP__ */

