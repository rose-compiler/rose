
#ifndef __KLT_DATA_FLOW_HPP__
#define __KLT_DATA_FLOW_HPP__

#include <list>

namespace KLT {

template <class Annotation> class Data;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class Kernel;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> class LoopTrees;

/*!
 * \addtogroup grp_klt_dataflow
 * @{
*/

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class DataFlow {
  public:
    virtual void generateFlowSets(
      const LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees,
      const std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & kernels
    ) const;
};

template <class DataAnnotation>
void append_access(
    SgExpression * exp,
    std::set<Data<DataAnnotation> *> & access_set,
    const std::set<Data<DataAnnotation> *> & datas
);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
void compute_read_write(
    LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation>::node_t * tree,
    DataFlow<Language, Runtime> & data_flow,
    const std::set<Data<DataAnnotation> *> & datas
);

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
void DataFlow<Language, Runtime>::generateFlowSets<DataAnnotation, RegionAnnotation, LoopAnnotation>(
  const LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation> & loop_trees,
  const std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & kernels
) const {
  assert(false);
#if 0
  assert(kernels.size() > 0);

  typename std::list<Kernel *>::const_iterator it_kernel;
  typename std::list<Kernel *>::const_reverse_iterator rit_kernel;

  std::set<Data *> datas;
  { // Collect all datas
   const std::set<Data *> & datas_in = loop_trees.getDatasIn();
     datas.insert(datas_in.begin(), datas_in.end());
   const std::set<Data *> & datas_out = loop_trees.getDatasOut();
     datas.insert(datas_out.begin(), datas_out.end());
   const std::set<Data *> & datas_local = loop_trees.getDatasLocal();
     datas.insert(datas_local.begin(), datas_local.end());
  }

  // 1 - Compute read/write sets for each kernel and set of all accessed data
  
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    Kernel * kernel = *it_kernel;
    typename Kernel::dataflow_t & data_flow = kernel->getDataflow();
    compute_read_write(kernel->getRoot(), data_flow, datas);
  }

  // 2 - Propagate:
  //       * data is flow-in  in a kernel if it is not flow-in  for a previous  kernel and it is used in this kernel (R/W)
  //       * data is flow-out in a kernel if it is not flow-out for a following kernel and it is used in this kernel (R/W)

  std::set<Data *> datas_in(loop_trees.getDatasIn());
  for (it_kernel = kernels.begin(); it_kernel != kernels.end(); it_kernel++) {
    Kernel * kernel = *it_kernel;
    typename Kernel::dataflow_t & data_flow = kernel->getDataflow();

    Data::set_intersection(data_flow.flow_in, datas_in, data_flow.datas);

    Data::set_remove(datas_in, data_flow.flow_in);
  }

  std::set<Data *> datas_out(loop_trees.getDatasOut());
  for (rit_kernel = kernels.rbegin(); rit_kernel != kernels.rend(); rit_kernel++) {
    Kernel * kernel = *rit_kernel;
    typename Kernel::dataflow_t & data_flow = kernel->getDataflow();

    Data::set_intersection(data_flow.flow_out, datas_out, data_flow.datas);

    Data::set_remove(datas_out, data_flow.flow_out);
  }

  // 3 - Compute the flow of data

  // TODO last set of equations...
#endif
}

/** @} */

}

#endif /* __KLT_DATA_FLOW_HPP__ */

