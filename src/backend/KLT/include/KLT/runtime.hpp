
#ifndef __KLT_RUNTIME_HPP__
#define __KLT_RUNTIME_HPP__

#include <cstddef>
#include <string>

class SgFunctionDeclaration;
namespace MFB {
  template <template <class Object> class Model>  class Driver;
  template <class Object> class Sage;
}
namespace MDCG {
  class ModelBuilder;
}
namespace KLT {
  namespace Descriptor {
    struct kernel_t;
    struct loop_t;
    struct tile_t;
  }
  namespace API {
    struct kernel_t;
    struct host_t;
  }

class Runtime {
  public:
    typedef Descriptor::kernel_t kernel_desc_t;
    typedef Descriptor::loop_t   loop_desc_t;
    typedef Descriptor::tile_t   tile_desc_t;

    typedef API::kernel_t kernel_api_t;
    typedef API::host_t   host_api_t;

    typedef ::MFB::Driver< ::MFB::Sage> Driver;

  protected:
    MDCG::ModelBuilder & model_builder;
    const std::string & klt_rtl_inc_dir;
    const std::string & klt_rtl_lib_dir;
    const std::string & usr_rtl_inc_dir;
    const std::string & usr_rtl_lib_dir;

    kernel_api_t * kernel_api;
    host_api_t   * host_api;

    size_t tilek_model;

  public:
    Runtime(
      MDCG::ModelBuilder & model_builder_,
      const std::string & klt_rtl_inc_dir_, const std::string & klt_rtl_lib_dir_,
      const std::string & usr_rtl_inc_dir_, const std::string & usr_rtl_lib_dir_,
      kernel_api_t * kernel_api_ = NULL, host_api_t * host_api_ = NULL
    );
    ~Runtime();

    void loadModel();

  protected:
    virtual void loadUserModel();

    virtual void useUserKernelSymbols(Driver & driver, size_t file_id) const;
    virtual void useUserHostSymbols(Driver & driver, size_t file_id) const;

  public:
    void useKernelSymbols(Driver & driver, size_t file_id) const;
    void useHostSymbols(Driver & driver, size_t file_id) const;

  public:
    enum exec_mode_t { e_default };

  public:
    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const;
    virtual void addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id) const;
};

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

