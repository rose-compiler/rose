
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
  struct symbol_map_t;
  namespace Descriptor {
    struct kernel_t;
    struct loop_t;
    struct tile_t;
    struct data_t;
  }
  namespace API {
    struct kernel_t;
    struct host_t;
  }
  namespace Utils {
    struct symbol_map_t;
  }

class Runtime {
  public:
    typedef Descriptor::kernel_t kernel_desc_t;
    typedef Descriptor::loop_t   loop_desc_t;
    typedef Descriptor::tile_t   tile_desc_t;
    typedef Descriptor::data_t   data_desc_t;

    typedef API::kernel_t kernel_api_t;
    typedef API::host_t   host_api_t;

    typedef ::MFB::Driver< ::MFB::Sage> Driver;

  public:
    enum exec_mode_t { e_default };

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
    virtual void loadUserModel() = 0;
    virtual void useUserKernelSymbols(Driver & driver, size_t file_id) const = 0;
    virtual void useUserHostSymbols(Driver & driver, size_t file_id) const = 0;

  public:
    void useKernelSymbols(Driver & driver, size_t file_id) const;
    void useHostSymbols(Driver & driver, size_t file_id) const;

    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const = 0;
    virtual void addKernelArgsForData     (SgFunctionParameterList * param_list, const std::vector<data_desc_t *> & data) const = 0;
    virtual void addKernelArgsForContext  (SgFunctionParameterList * param_list) const = 0;

    virtual SgVariableSymbol * getSymbolForParameter(SgVariableSymbol * parameter, SgBasicBlock * bb) const = 0;
    virtual SgVariableSymbol * getSymbolForData     (data_desc_t      * data     , SgBasicBlock * bb) const = 0;

    virtual SgVariableSymbol * createLoopIterator(const loop_desc_t & loop, SgBasicBlock * bb) const = 0;
    virtual SgVariableSymbol * createTileIterator(const tile_desc_t & tile, SgBasicBlock * bb) const = 0;

    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const = 0;
    virtual void addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id) const = 0;

    virtual SgType * buildKernelReturnType(::KLT::Descriptor::kernel_t & kernel) const = 0;
};

} // namespace KLT

#endif /* __KLT_RUNTIME_HPP__ */

