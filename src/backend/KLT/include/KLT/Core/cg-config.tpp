
#ifdef __NO_TEMPLATE_INSTANTIATION__
#error "Entering a template definition file when the macro __NO_TEMPLATE_INSTANTIATION__ is defined"
#endif

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

template <class Kernel>
CG_Config<Kernel>::CG_Config(LoopMapper<Kernel> * loop_mapper, IterationMapper<Kernel> * iteration_mapper, DataFlow<Kernel> * data_flow) :
  p_loop_mapper(loop_mapper),
  p_iteration_mapper(iteration_mapper),
  p_data_flow(data_flow)
{}

template <class Kernel>
CG_Config<Kernel>::~CG_Config() {
  if (p_data_flow != NULL) delete p_data_flow;
  if (p_loop_mapper != NULL) delete p_loop_mapper;
  if (p_iteration_mapper != NULL) delete p_iteration_mapper;
}

template <class Kernel>
const LoopMapper<Kernel> & CG_Config<Kernel>::getLoopMapper() const {return *p_loop_mapper; }

template <class Kernel>
const IterationMapper<Kernel> & CG_Config<Kernel>::getIterationMapper() const { return *p_iteration_mapper; }

template <class Kernel>
const DataFlow<Kernel> & CG_Config<Kernel>::getDataFlow() const { return *p_data_flow; }

/** @} */

}

}

