
#ifndef __KLT_CORE_GENERATOR_HPP__
#define __KLT_CORE_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>


class SgProject;
class SgSourceFile;
class SgVariableSymbol;

namespace MultiFileBuilder {
  template <class Object> class KLT;
  template <class Object> class Sage;
  template <template <class Object> class Model> class Driver;

  class KLT_Driver;
}

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

class Data;
class LoopTrees;

template <class Kernel> class CG_Config;
template <class Kernel> class IterationMap;

template <class Kernel, class Driver = ::MultiFileBuilder::KLT_Driver>
class Generator {
  protected:
    Driver & p_driver;

    ::MultiFileBuilder::Driver< ::MultiFileBuilder::KLT>  & p_klt_driver;
    ::MultiFileBuilder::Driver< ::MultiFileBuilder::Sage> & p_sage_driver;

    std::string p_file_name;
    unsigned long  p_file_id;

  protected:
    unsigned long createFile();

    void buildArgumentLists(const LoopTrees & loop_trees, Kernel * kernel);

    typename Kernel::a_kernel * callToKernelBuilder(
      Kernel * kernel,
      typename Kernel::loop_mapping_t * loop_mapping,
      IterationMap<Kernel> * iteration_map
    ) const;

  public:
    Generator(Driver & driver, const std::string & file_name);
    virtual ~Generator();

    void generate(const LoopTrees & loop_trees, std::set<std::list<Kernel *> > & kernel_lists, const CG_Config<Kernel> & cg_config);
};

/** @} */

}

}

#ifndef __NO_TEMPLATE_INSTANTIATION__
#include "KLT/Core/generator.tpp"
#endif

#endif /* __KLT_CORE_GENERATOR_HPP__ */

