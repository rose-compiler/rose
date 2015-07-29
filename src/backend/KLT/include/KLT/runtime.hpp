
#ifndef __KLT_RUNTIME_HPP__
#define __KLT_RUNTIME_HPP__

#include <cstddef>
#include <string>
#include <vector>

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgBasicBlock;
class SgVariableSymbol;

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
    struct data_t;
  }
  namespace API {
    struct kernel_t;
    struct host_t;
    struct call_interface_t;
  }
  namespace Utils {
    struct symbol_map_t;
  }

class Runtime {
  public:

    typedef ::MFB::Driver< ::MFB::Sage> Driver;

  public:
    enum exec_mode_t { e_default };

  protected:
    MDCG::ModelBuilder & model_builder;
    const std::string & klt_rtl_inc_dir;
    const std::string & klt_rtl_lib_dir;
    const std::string & usr_rtl_inc_dir;
    const std::string & usr_rtl_lib_dir;

    API::host_t * host_api;
    API::kernel_t * kernel_api;
    API::call_interface_t * call_interface;

    size_t tilek_model;

  public:
    Runtime(
      MDCG::ModelBuilder & model_builder_,
      const std::string & klt_rtl_inc_dir_, const std::string & klt_rtl_lib_dir_,
      const std::string & usr_rtl_inc_dir_, const std::string & usr_rtl_lib_dir_,
      API::host_t * host_api_ = NULL, API::kernel_t * kernel_api_ = NULL,
      API::call_interface_t * call_interface_ = NULL
    );

  public:
    void loadModel();

  protected:
    virtual void loadExtraModel() = 0;

  public:
    API::kernel_t & getKernelAPI();
    const API::kernel_t & getKernelAPI() const;
    API::host_t & getHostAPI();
    const API::host_t & getHostAPI() const;
    API::call_interface_t & getCallInterface();
    const API::call_interface_t & getCallInterface() const;

    virtual void addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id) const = 0;
};

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

