
#ifndef __KLT_GENERATOR_HPP__
#define __KLT_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>


class SgProject;
class SgSourceFile;
class SgVariableSymbol;

namespace MFB {
  template <class Object> class KLT;
  template <class Object> class Sage;
  template <template <class Object> class Model> class Driver;

  class KLT_Driver;
}

namespace KLT {

template <class Annotation> class Data;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class Kernel;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation> class LoopTrees;

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class LoopMapper;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class IterationMapper;
template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class DataFlow;

template <class Language, class Runtime> class IterationMap;

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime> class CG_Config;

/*!
 * \addtogroup grp_klt_codegen
 * @{
*/

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver = ::MFB::KLT_Driver>
class Generator {
  protected:
    Driver & p_driver;

    ::MFB::Driver< ::MFB::KLT  > & p_klt_driver;
    ::MFB::Driver< ::MFB::Sage > & p_sage_driver;

    std::string p_file_name;
    unsigned long  p_file_id;

  protected:
    unsigned long createFile();

    void buildArgumentLists(const LoopTrees & loop_trees, Kernel * kernel);

    typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::a_kernel * callToKernelBuilder(
      Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * kernel,
      typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_map,
      IterationMap<Language, Runtime> * iteration_map
    ) const;

  public:
    Generator(Driver & driver, const std::string & file_name);
    virtual ~Generator();

    void generate(
      const LoopTrees<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & loop_trees,
      std::set<std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> > & kernel_lists,
      const CG_Config<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> & cg_config
    );
};

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver>
typename Kernel::a_kernel * Generator<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::callToKernelBuilder(
  Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * kernel,
  typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping,
  IterationMap<Language, Runtime> * iteration_map
) const {
  typename ::MFB::KLT<Kernel>::object_desc_t kernel_desc(kernel, loop_mapping, iteration_map, p_file_id);
  typename ::MFB::KLT<Kernel>::build_result_t result =  p_klt_driver.build<Kernel>(kernel_desc);
  return result;
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver>
void Generator<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::buildArgumentLists(const LoopTrees & loop_trees, Kernel * kernel) {
  std::set<SgVariableSymbol *>::const_iterator it_symbol;
  std::set<Data<DataAnnotation> *>::const_iterator it_data;

  const std::set<SgVariableSymbol *> & parameters = loop_trees.getParameters();
  const std::set<SgVariableSymbol *> & coefficients = loop_trees.getCoefficients();

  std::set<SgVariableSymbol *> sym_var_refs;
    collectReferencedSymbols(kernel, sym_var_refs);

  for (it_symbol = parameters.begin(); it_symbol != parameters.end(); it_symbol++) {
    SgVariableSymbol * parameter = *it_symbol;

    bool used = sym_var_refs.find(parameter) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().parameters.push_back(parameter);
  }

  for (it_symbol = coefficients.begin(); it_symbol != coefficients.end(); it_symbol++) {
    SgVariableSymbol * coefficient = *it_symbol;

    bool used = sym_var_refs.find(coefficient) != sym_var_refs.end(); // If it is referenced (SgVarRefExp) then it is needed

    if (used) kernel->getArguments().coefficients.push_back(coefficient);
  }

  for (it_data = kernel->getDataflow().datas.begin(); it_data != kernel->getDataflow().datas.end(); it_data++) {
    Data<DataAnnotation> * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol(); // FIXME Whole data approx (same symbol same data)

    bool used = sym_var_refs.find(data_sym) != sym_var_refs.end();

    if (used) kernel->getArguments().datas.push_back(data); // If it is referenced (SgVarRefExp) then it is needed
  }

  // FIXME with partial data we might end up referencing multiple time the same variable..
}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver>
Generator<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::Generator(
  Driver & driver,
  const std::string & file_name
) :
  p_driver(driver),
  p_klt_driver(p_driver),
  p_sage_driver(p_driver),
  p_file_name(file_name),
  p_file_id(createFile())
{}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver>
Generator<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::~Generator() {}

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime, class Driver>
void Generator<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::generate(const LoopTrees & loop_trees, std::set<std::list<Kernel *> > & kernel_lists, const CG_Config<Kernel> & cg_config) {
  typename std::set<std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> >::const_iterator it_kernel_list;
  typename std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *>::const_iterator it_kernel;
  typename std::set<typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t  *>::const_iterator it_loop_mapping;
  typename std::set<IterationMap<Language, Runtime> *>::const_iterator it_iteration_map;

  // 1 - Loop Selection

  cg_config.getLoopMapper().createKernels(loop_trees, kernel_lists);

  // 2 - Data Flow

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel *> & kernel_list = *it_kernel_list;
    cg_config.getDataFlow().generateFlowSets(loop_trees, kernel_list);
  }

  // 3 - Arguments
  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel * kernel = *it_kernel;
      buildArgumentLists(loop_trees, kernel);
    }
  }

  // 4 - Iterations Mapping

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * kernel = *it_kernel;
      const std::set<typename Kernel::loop_mapping_t *> & loop_mappings = kernel->getLoopMappings();
      for (it_loop_mapping = loop_mappings.begin(); it_loop_mapping != loop_mappings.end(); it_loop_mapping++) {
        typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t  * loop_mapping = *it_loop_mapping;
        cg_config.getIterationMapper().generateShapes(loop_mapping, kernel->getIterationMaps(loop_mapping));
      }
    }
  }

  // 5 - Code Generation

  for (it_kernel_list = kernel_lists.begin(); it_kernel_list != kernel_lists.end(); it_kernel_list++) {
    const std::list<Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> *> & kernel_list = *it_kernel_list;
    for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
      Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime> * kernel = *it_kernel;
      const std::set<typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t  *> & loop_mappings = kernel->getLoopMappings();
      for (it_loop_mapping = loop_mappings.begin(); it_loop_mapping != loop_mappings.end(); it_loop_mapping++) {
        typename Kernel<DataAnnotation, RegionAnnotation, LoopAnnotation, Language, Runtime>::loop_mapping_t * loop_mapping = *it_loop_mapping;
        const std::set<IterationMap<Language, Runtime> *> & iteration_maps =  kernel->getIterationMaps(loop_mapping);
        for (it_iteration_map = iteration_maps.begin(); it_iteration_map != iteration_maps.end(); it_iteration_map++) {
          IterationMap<Language, Runtime> * iteration_map = *it_iteration_map;
          kernel->setKernel(loop_mapping, iteration_map, callToKernelBuilder(kernel, loop_mapping, iteration_map));
        }
      }
    }
  }
}



/** @} */

}

#endif /* __KLT_GENERATOR_HPP__ */

