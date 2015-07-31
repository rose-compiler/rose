
#ifndef __KLT_RUNTIME_HPP__
#define __KLT_RUNTIME_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include "MDCG/Core/model-builder.hpp"

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

class Runtime {
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
    Runtime(MFB::Driver<MFB::KLT::KLT> & driver_, MDCG::ModelBuilder & model_builder_);

    void loadModel(const std::string & klt_inc_dir, const std::string & usr_inc_dir);

    virtual void loadExtraModel(const std::string & usr_inc_dir) = 0;

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
    template <class runtime_tpl, class host_api_tpl, class kernel_api_tpl, class call_interface_tpl>
    static runtime_tpl * build(MFB::Driver<MFB::KLT::KLT> & driver, MDCG::ModelBuilder & model_builder, const std::string & klt_inc_dir, const std::string & usr_inc_dir, const std::string & basename) {
      runtime_tpl * runtime = new runtime_tpl(driver, model_builder);
        runtime->host_api       = new host_api_tpl();
        runtime->kernel_api     = new kernel_api_tpl();
        runtime->call_interface = new call_interface_tpl(driver, runtime->kernel_api);
        runtime->loadModel(klt_inc_dir, usr_inc_dir);

      std::string kernel_filename(basename + "-kernel.c");
      runtime->kernel_file_id = driver.create(boost::filesystem::path(kernel_filename));
          driver.setUnparsedFile(runtime->kernel_file_id);
          driver.setCompiledFile(runtime->kernel_file_id);
      runtime->getKernelAPI().use(driver, runtime->kernel_file_id);

      std::string static_filename(basename + "-static.c");
      runtime->static_file_id = driver.create(boost::filesystem::path(static_filename));
          driver.setUnparsedFile(runtime->static_file_id);
          driver.setCompiledFile(runtime->static_file_id);
      runtime->getHostAPI().use(driver, runtime->static_file_id);

      return runtime;
    }

  public:
    template <class language_tpl>
    struct tiling_info_t {};

    template <class language_tpl>
    void applyLoopTiling(
      Kernel::kernel_t * kernel,
      const std::map<typename language_tpl::directive_t *, size_t> & directive_loop_id_map,
      std::map<tiling_info_t<language_tpl> *, std::vector<Kernel::kernel_t *> > & tiled_kernels
    ) {
      tiling_info_t<language_tpl> * tiling_info = new tiling_info_t<language_tpl>();
      tiled_kernels[tiling_info].push_back(kernel);
    }

    template <class language_tpl>
    void solveDataFlow(
      Kernel::kernel_t * kernel,
      tiling_info_t<language_tpl> * tiling_info,
      const std::vector<Kernel::kernel_t *> & subkernels,
      std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > & kernel_deps_map,
      const std::map<Kernel::kernel_t *, Descriptor::kernel_t *> & translation_map,
      const std::map<Descriptor::kernel_t *, Kernel::kernel_t *> & rtranslation_map
    ) {
      // Simplest implementation I can think of....

      Descriptor::kernel_t * previous = NULL;
      Descriptor::kernel_t * current = NULL;

      std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> >::iterator it_kernel_deps;
      std::map<Kernel::kernel_t *, Descriptor::kernel_t *>::const_iterator it_trans;

      std::vector<Kernel::kernel_t *>::const_iterator it;
      for (it = subkernels.begin(); it != subkernels.end(); it++) {
        it_trans = translation_map.find(*it);
        assert(it_trans != translation_map.end());
        current = it_trans->second;

        it_kernel_deps = kernel_deps_map.find(current);
        assert(it_kernel_deps != kernel_deps_map.end());
        std::vector<Descriptor::kernel_t *> & deps = it_kernel_deps->second;

        if (previous != NULL)
          deps.push_back(previous);

        previous = current;
      }
    }
};

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

