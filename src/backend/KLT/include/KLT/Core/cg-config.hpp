
#ifndef __KLT_CG_CONFIG_HPP__
#define __KLT_CG_CONFIG_HPP__

#include <cstddef>

namespace KLT {

template <class Annotation, class Language, class Runtime> class LoopMapper;
template <class Annotation, class Language, class Runtime> class LoopTiler;
template <class Annotation, class Language, class Runtime> class DataFlow;

/*!
 * \addtogroup grp_klt_codegen
 * @{
*/

template <class Annotation, class Language, class Runtime>
class CG_Config {
  protected:
    LoopMapper<Annotation, Language, Runtime> * p_loop_mapper;
    LoopTiler<Annotation, Language, Runtime> * p_loop_tiler;
    DataFlow<Annotation, Language, Runtime> * p_data_flow;

  public:
    CG_Config(
        LoopMapper<Annotation, Language, Runtime> * loop_mapper,
        LoopTiler<Annotation, Language, Runtime> * iteration_mapper,
        DataFlow<Annotation, Language, Runtime> * data_flow
    );
    virtual ~CG_Config();
    
    const LoopMapper<Annotation, Language, Runtime> & getLoopMapper() const;
    const LoopTiler<Annotation, Language, Runtime> & getLoopTiler() const;
    const DataFlow<Annotation, Language, Runtime> & getDataFlow() const;
};

template <class Annotation, class Language, class Runtime>
CG_Config<Annotation, Language, Runtime>::CG_Config(
  LoopMapper<Annotation, Language, Runtime> * loop_mapper,
  LoopTiler<Annotation, Language, Runtime> * loop_tiler,
  DataFlow<Annotation, Language, Runtime> * data_flow
) :
  p_loop_mapper(loop_mapper),
  p_loop_tiler(loop_tiler),
  p_data_flow(data_flow)
{}

template <class Annotation, class Language, class Runtime>
CG_Config<Annotation, Language, Runtime>::~CG_Config() {
  if (p_loop_mapper != NULL) delete p_loop_mapper;
  if (p_loop_tiler != NULL) delete p_loop_tiler;
  if (p_data_flow != NULL) delete p_data_flow;
}

template <class Annotation, class Language, class Runtime>
const LoopMapper<Annotation, Language, Runtime> & CG_Config<Annotation, Language, Runtime>::getLoopMapper() const { return *p_loop_mapper; }

template <class Annotation, class Language, class Runtime>
const LoopTiler<Annotation, Language, Runtime> & CG_Config<Annotation, Language, Runtime>::getLoopTiler() const { return *p_loop_tiler; }

template <class Annotation, class Language, class Runtime>
const DataFlow<Annotation, Language, Runtime> & CG_Config<Annotation, Language, Runtime>::getDataFlow() const { return *p_data_flow; }

/** @} */

}

#endif /* __KLT_CG_CONFIG_HPP__ */

