
#ifndef __LOOP_TREES_HPP__
#define __LOOP_TREES_HPP__

#include <set>
#include <map>
#include <list>
#include <vector>
#include <utility>

#include <iostream>

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

    /// Datas flowing in the sequence loop trees
    std::set<Data<DataAnnotation> *> p_datas_in;

    /// Datas flowing out the sequence loop trees
    std::set<Data<DataAnnotation> *> p_datas_out;

    /// Datas local to the sequence loop trees
    std::set<Data<DataAnnotation> *> p_datas_local;

    /// Coefficiants (constant values) used in the sequence loop trees
    std::set<SgVariableSymbol *> p_coefficients;

    /// Parameters (constant integers not used in computation, array shape and loop sizes) of the sequence loop trees
    std::set<SgVariableSymbol *> p_parameters;

    std::vector<RegionAnnotation> annotations;

  public:
    const std::list<node_t *> & getTrees() const;

    const std::set<Data<DataAnnotation> *> getDatasIn() const;
    const std::set<Data<DataAnnotation> *> getDatasOut() const;
    const std::set<Data<DataAnnotation> *> getDatasLocal() const;

    const std::set<SgVariableSymbol *> getCoefficients() const;
    const std::set<SgVariableSymbol *> getParameters() const;

  public:
    LoopTrees();
    virtual ~LoopTrees();

    /// Add a tree at the end of the list
    void addTree(node_t * tree);

    /// Add a data flowing in the sequence of loop trees
    void addDataIn(Data<DataAnnotation> * data);

    /// Add a data flowing out the sequence of loop trees
    void addDataOut(Data<DataAnnotation> * data);

    /// Add a data local to the sequence of loop trees
    void addDataLocal(Data<DataAnnotation> * data);

    /// Add a coefficient of the sequence of loop trees
    void addCoefficient(SgVariableSymbol * var_sym);

    /// Add a parameter of the sequence of loop trees
    void addParameter(SgVariableSymbol * var_sym);

    /// Read from a lisp like text file
    void read(char * filename, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Data<DataAnnotation> *>, std::list<KLT::Data<DataAnnotation> *> > & inout_data_order);

    /// Read from a lisp like text file
    void read(std::ifstream & in_file, std::list<SgVariableSymbol *> & parameter_order, std::pair<std::list<KLT::Data<DataAnnotation> *>, std::list<KLT::Data<DataAnnotation> *> > & inout_data_order);

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

/** @} */

}

#endif /* __LOOP_TREES_HPP__ */

