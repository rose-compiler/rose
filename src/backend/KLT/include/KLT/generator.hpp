
#ifndef __KLT_RUNTIME_HPP__
#define __KLT_RUNTIME_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include "MDCG/Core/model-builder.hpp"

#include "KLT/looptree.hpp"
#include "KLT/kernel.hpp"
#include "KLT/descriptor.hpp"
#include "KLT/api.hpp"

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgBasicBlock;
class SgVariableSymbol;

namespace MFB {
  template <template <class Object> class Model>  class Driver;
  template <class Object> class Sage;
  namespace KLT {
    template <class Object> class KLT;
  }
}
namespace MDCG {
  class ModelBuilder;
}
namespace KLT {
  namespace Descriptor {
    struct kernel_t;
    struct loop_t;
    struct tile_t;
    struct data_t;
  }
  namespace API {
    struct kernel_t;
    struct host_t;
    struct call_interface_t;
  }
  namespace Kernel {
    struct kernel_t;
  }
  namespace Utils {
    struct symbol_map_t;
  }
}
namespace KLT {

class Generator {
  protected:
    MFB::Driver<MFB::KLT::KLT> & driver;
    MDCG::ModelBuilder & model_builder;

    size_t tilek_model;

    MFB::file_id_t kernel_file_id;
    MFB::file_id_t static_file_id;

    API::host_t * host_api;
    API::kernel_t * kernel_api;
    API::call_interface_t * call_interface;

  protected:
    Generator(MFB::Driver<MFB::KLT::KLT> & driver_, MDCG::ModelBuilder & model_builder_);

    void loadModel(const std::string & klt_inc_dir, const std::string & usr_inc_dir);

    virtual void loadExtraModel(const std::string & usr_inc_dir) = 0;

  public:
    template <class generator_tpl>
    static generator_tpl * build(MFB::Driver<MFB::KLT::KLT> & driver, MDCG::ModelBuilder & model_builder, const std::string & klt_inc_dir, const std::string & usr_inc_dir, const std::string & basename);

  public:
    MFB::Driver<MFB::KLT::KLT> & getDriver();
    const MFB::Driver<MFB::KLT::KLT> & getDriver() const;

    MDCG::ModelBuilder & getModelBuilder();
    const MDCG::ModelBuilder & getModelBuilder() const;

    API::host_t & getHostAPI();
    const API::host_t & getHostAPI() const;

    API::kernel_t & getKernelAPI();
    const API::kernel_t & getKernelAPI() const;

    API::call_interface_t & getCallInterface();
    const API::call_interface_t & getCallInterface() const;

    MFB::file_id_t getKernelFileID() const;
    MFB::file_id_t getStaticFileID() const;

  public:
    template <class language_tpl>
    static SgStatement * instanciateOnHost(const Kernel::kernel_t * original, const std::vector<Descriptor::loop_t *> & loops);

  public:
    template <class language_tpl>
    struct tiling_info_t {
      std::map<size_t, std::map<size_t, typename language_tpl::tile_parameter_t *> > tiling_map;

      void toGraphViz(std::ostream & out) const;
    };

    typedef std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > kernel_deps_map_t;

    template <class language_tpl>
    struct subkernel_result_t {
      Kernel::kernel_t * original;
      std::vector<Descriptor::loop_t *> loops;
      std::map<tiling_info_t<language_tpl> *, kernel_deps_map_t> tiled;

      void toGraphViz(std::ostream & out) const;
    };

    template <class language_tpl>
    static void addToStaticData(const subkernel_result_t<language_tpl> & subkernel_result);

  public:
    template <class language_tpl>
    static bool createTiles(LoopTree::loop_t * loop, const std::map<size_t, typename language_tpl::tile_parameter_t *> & tiling, LoopTree::tile_t * & first, LoopTree::tile_t * & last, size_t & tile_cnt);

  public:
    virtual void solveDataFlow(
      Kernel::kernel_t * kernel,
      const std::vector<Kernel::kernel_t *> & subkernels,
      kernel_deps_map_t & kernel_deps_map,
      const std::map<Kernel::kernel_t *, Descriptor::kernel_t *> & translation_map,
      const std::map<Descriptor::kernel_t *, Kernel::kernel_t *> & rtranslation_map
    ) const;
};

template <class generator_tpl>
generator_tpl * Generator::build(MFB::Driver<MFB::KLT::KLT> & driver, MDCG::ModelBuilder & model_builder, const std::string & klt_inc_dir, const std::string & usr_inc_dir, const std::string & basename) {
  generator_tpl * generator = new generator_tpl(driver, model_builder);
    generator->host_api       = new typename generator_tpl::host_t();
    generator->kernel_api     = new typename generator_tpl::kernel_t();
    generator->call_interface = new typename generator_tpl::call_interface_t(driver, generator->kernel_api);
  generator->loadModel(klt_inc_dir, usr_inc_dir);

  // TODO virtual boost::filesystem::path MFB::API::api_t::createFilePath(const std::string & name) const { return boost::filesystem::path(name + ".c"); }
  std::string static_filename(basename + "-static.c");
  generator->static_file_id = driver.create(boost::filesystem::path(static_filename));
    driver.setUnparsedFile(generator->static_file_id);
    driver.setCompiledFile(generator->static_file_id);
  generator->getHostAPI().use(driver, generator->static_file_id);

  std::string kernel_filename(basename + "-kernel.c");
  generator->kernel_file_id = driver.create(boost::filesystem::path(kernel_filename));
    driver.setUnparsedFile(generator->kernel_file_id);
    driver.setCompiledFile(generator->kernel_file_id);
  generator->getKernelAPI().use(driver, generator->kernel_file_id);

  return generator;
}


template <class language_tpl>
SgStatement * Generator::instanciateOnHost(const Kernel::kernel_t * original, const std::vector<Descriptor::loop_t *> & loops) {
  return NULL; // TODO
}

template <class language_tpl>
void Generator::addToStaticData(const subkernel_result_t<language_tpl> & subkernel_result) {
  // TODO
}

template <class language_tpl>
bool Generator::createTiles(
  LoopTree::loop_t * loop, const std::map<size_t, typename language_tpl::tile_parameter_t *> & tiling, LoopTree::tile_t * & first, LoopTree::tile_t * & last, size_t & tile_cnt
) {
  typename std::map<size_t, typename language_tpl::tile_parameter_t *>::const_iterator it_tile;
  for (it_tile = tiling.begin(); it_tile != tiling.end(); it_tile++) {
    LoopTree::tile_t * current = new LoopTree::tile_t(tile_cnt++, it_tile->second->kind, it_tile->second->order, it_tile->second->param, loop->id, it_tile->first);
      current->parent = last;

    if (last != NULL) {
      last->next_tile = current;
      last->next_node = NULL;
    }
    else first = current;

    last = current;
  }
  return true;
}

template <class language_tpl>
void Generator::tiling_info_t<language_tpl>::toGraphViz(std::ostream & out) const {
  // NOP ?
}

template <class language_tpl>
void Generator::subkernel_result_t<language_tpl>::toGraphViz(std::ostream & out) const {
  out << "digraph looptree {" << std::endl;
  out << "  subgraph cluster_original {" << std::endl;
  original->root->toGraphViz(out, "    ");
  out << "  }" << std::endl;
  size_t tiled_kernel_cnt = 0;

  typename std::map<tiling_info_t<language_tpl> *, kernel_deps_map_t>::const_iterator it_tiled_kernel;
  for (it_tiled_kernel = tiled.begin(); it_tiled_kernel != tiled.end(); it_tiled_kernel++) {
    std::map<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *> translation_map;
    std::map<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *> rtranslation_map;

    tiling_info_t<language_tpl> * tiling_info = it_tiled_kernel->first;
    const kernel_deps_map_t & subkernels = it_tiled_kernel->second;
/*
    out << "  subgraph cluster_k_" << tiled_kernel_cnt++ << " {" << std::endl;

    tiling_info->toGraphViz(out);

    size_t subkernel_cnt = 0;
    kernel_deps_map_t::const_iterator it_kernel;
    for (it_kernel = subkernels.begin(); it_kernel != subkernels.end(); it_kernel++) {
      out << "    subgraph cluster_sk_" << subkernel_cnt++ << " {" << std::endl;
      it_kernel->first->root->toGraphViz(out, "      ");
      out << "    }" << std::endl;
      out << "  }" << std::endl;
    }
    out << "}" << std::endl;
*/
  }
}

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

