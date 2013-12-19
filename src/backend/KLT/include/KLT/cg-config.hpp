
#ifndef __KLT_CORE_CG_CONFIG_HPP__
#define __KLT_CORE_CG_CONFIG_HPP__

#include <cstddef>

namespace KLT {

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class LoopMapper;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class IterationMapper;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class DataFlow;

/*!
 * \addtogroup grp_klt_codegen
 * @{
*/

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class CG_Config {
  protected:
    LoopMapper      <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * p_loop_mapper;
    IterationMapper <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * p_iteration_mapper;
    DataFlow        <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * p_data_flow;

  public:
    CG_Config(
        LoopMapper      <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * loop_mapper,
        IterationMapper <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * iteration_mapper,
        DataFlow        <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * data_flow
    );
    virtual ~CG_Config();
    
    const LoopMapper      <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & getLoopMapper()      const;
    const IterationMapper <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & getIterationMapper() const;
    const DataFlow        <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & getDataFlow()        const;
};

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
CG_Config<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::CG_Config(
  LoopMapper      <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * loop_mapper,
  IterationMapper <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * iteration_mapper,
  DataFlow        <DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * data_flow
) :
  p_loop_mapper(loop_mapper),
  p_iteration_mapper(iteration_mapper),
  p_data_flow(data_flow)
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
CG_Config<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::~CG_Config() {
  if (p_loop_mapper != NULL) delete p_loop_mapper;
  if (p_iteration_mapper != NULL) delete p_iteration_mapper;
  if (p_data_flow != NULL) delete p_data_flow;
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
const LoopMapper<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & CG_Config<Language, Runtime>::getLoopMapper() const { return *p_loop_mapper; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
const IterationMapper<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & CG_Config<Language, Runtime>::getIterationMapper() const { return *p_iteration_mapper; }

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
const DataFlow<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & CG_Config<Language, Runtime>::getDataFlow() const { return *p_data_flow; }

/** @} */

}

#endif /* __KLT_CORE_CG_CONFIG_HPP__ */

