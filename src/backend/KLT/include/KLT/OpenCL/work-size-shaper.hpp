
#ifndef __KLT_OPENCL_WORK_SIZE_SHAPER_HPP__
#define __KLT_OPENCL_WORK_SIZE_SHAPER_HPP__

#include "KLT/Core/loop-trees.hpp"

#include <vector>
#include <list>
#include <set>

class SgScopeStatement;
class SgVariableSymbol;
class SgExpression;

namespace KLT {

namespace OpenCL {

/*!
 * \addtogroup grp_klt_ocl
 * @{
*/

class WorkSizeShape {
  protected:
    unsigned int p_number_dims;
    
    const std::map<SgVariableSymbol *, unsigned int> & p_iterator_to_seq_map;
    const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & p_iterator_to_loop_map;

    std::vector<SgExpression *> p_global_dims;
    std::vector<SgExpression *> p_local_dims;
    
    bool p_have_local_dims;

  public:
    WorkSizeShape(
      unsigned int number_dims,
      const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
      const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map
    );
    virtual ~WorkSizeShape();

    unsigned int getNumberDimensions() const;

    const std::vector<SgExpression *> & getGlobalDimensions() const;

    const std::vector<SgExpression *> & getLocalDimensions() const;

    bool haveLocalDimensions() const;

    /** 
     * \param global_coordinates Symbols for the coordinates of the thread instances in the (global) work sharing grid
     * \param local_coordinates  Symbols for the coordinates of the thread instances in the (local ) work sharing grid
     * \param iterator Symbol of the iterator we want to setup
     * \return res.first: lower bound of the interval describe by the thread, res.second: size of the interval (if NULL it is assumed to be 1)
     */
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const std::vector<SgVariableSymbol *> & global_coordinates,
      const std::vector<SgVariableSymbol *> & local_coordinates,
      SgVariableSymbol * iterator
    ) = 0;
};

class WorkSizeShaper {
  public:
    virtual void generateShapes(
      const std::list<Core::LoopTrees::loop_t *> & nested_loops,
      const std::list<SgVariableSymbol *> & params,
      std::set<WorkSizeShape *> & shapes
    ) const;

    virtual void generateShapes(
      const std::list<Core::LoopTrees::loop_t *> & nested_loops,
      const std::list<SgVariableSymbol *> & params,
      const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
      const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map,
      std::set<WorkSizeShape *> & shapes
    ) const = 0;
};

// Dummy implementation

class Dummy_WorkSizeShape : public WorkSizeShape {

  public:
    Dummy_WorkSizeShape(
      const std::list<Core::LoopTrees::loop_t *> & nested_loops,
      const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
      const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map
    );
    virtual ~Dummy_WorkSizeShape();

    /**
     *  \return (exp, NULL) where exp is VarRef("one of the dimension") + "associated loop".lower_bound
     */
    virtual std::pair<SgExpression *, SgExpression *> generateIteratorBounds(
      const std::vector<SgVariableSymbol *> & global_coordinates,
      const std::vector<SgVariableSymbol *> & local_coordinates,
      SgVariableSymbol * iterator
    );
};

class Dummy_WorkSizeShaper : public WorkSizeShaper {
  public:
    /// Generate only one Dummy_WorkSizeShape : it maps each loop to a different dimension
    virtual void generateShapes(
      const std::list<Core::LoopTrees::loop_t *> & nested_loops,
      const std::list<SgVariableSymbol *> & params,
      const std::map<SgVariableSymbol *, unsigned int> & iterator_to_seq_map,
      const std::map<SgVariableSymbol *, Core::LoopTrees::loop_t *> & iterator_to_loop_map,
      std::set<WorkSizeShape *> & shapes
    ) const;
};

/** @} */

}

}

#endif /* __KLT_OPENCL_WORK_SIZE_SHAPER_HPP__ */
