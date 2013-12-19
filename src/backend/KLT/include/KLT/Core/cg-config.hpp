
#ifndef __KLT_CORE_CG_CONFIG_HPP__
#define __KLT_CORE_CG_CONFIG_HPP__

#include <cstddef>

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel> class LoopMapper;
template <class Kernel> class IterationMapper;
template <class Kernel> class DataFlow;

template <class Kernel>
class CG_Config {
  protected:
    LoopMapper<Kernel>      * p_loop_mapper;
    IterationMapper<Kernel> * p_iteration_mapper;
    DataFlow<Kernel>        * p_data_flow;

  public:
    CG_Config(LoopMapper<Kernel> * loop_mapper, IterationMapper<Kernel> * iteration_mapper, DataFlow<Kernel> * data_flow);
    virtual ~CG_Config();
    
    const LoopMapper<Kernel>      & getLoopMapper()      const;
    const DataFlow<Kernel>        & getDataFlow()        const;
    const IterationMapper<Kernel> & getIterationMapper() const;
};

/** @} */

}

}

#ifndef __NO_TEMPLATE_INSTANTIATION__
#include "KLT/Core/cg-config.tpp"
#endif

#endif /* __KLT_CORE_CG_CONFIG_HPP__ */

