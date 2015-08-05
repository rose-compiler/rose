
#ifndef __KLT_API_HPP__
#define __KLT_API_HPP__

#include "MDCG/Tools/api.hpp"

class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgType;
class SgInitializedName;
class SgExpression;
class SgStatement;
class SgScopeStatement;
class SgFunctionParameterList;
class SgBasicBlock;

namespace MFB {
  template <template <class Object> class Model>  class Driver;
  template <class Object> class Sage;
}
namespace KLT {
  namespace Descriptor {
    struct kernel_t;
    struct loop_t;
    struct tile_t;
    struct data_t;
  }
  namespace Utils {
    struct symbol_map_t;
  }

namespace API {

struct kernel_t : public ::MDCG::Tools::api_t {
  protected:
    SgClassSymbol * klt_loop_context_class;

    SgFunctionSymbol * get_loop_lower_fnct;
    SgFunctionSymbol * get_loop_upper_fnct;
    SgFunctionSymbol * get_loop_stride_fnct;

    SgFunctionSymbol * get_tile_length_fnct;
    SgFunctionSymbol * get_tile_stride_fnct;

    SgClassSymbol * klt_data_context_class;

  public:
    virtual void load(const MDCG::Model::model_t & model);

  public:
    // Loop Context and Getters

    SgType * getLoopContextPtrType() const;

    SgExpression * buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const;

    SgExpression * buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const;
    SgExpression * buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const;

    // Data Context and Getters (NIY)

    SgType * getDataContextPtrType() const;
};

struct host_t : public ::MDCG::Tools::api_t {
  protected:
    SgClassSymbol * kernel_class;
      SgVariableSymbol * kernel_param_field;
      SgVariableSymbol * kernel_data_field;
      SgVariableSymbol * kernel_loops_field;
//    SgVariableSymbol * kernel_tiles_field;

    SgClassSymbol * loop_class;
      SgVariableSymbol * loop_lower_field;
      SgVariableSymbol * loop_upper_field;
      SgVariableSymbol * loop_stride_field;

//  SgClassSymbol * tile_class;
//    SgVariableSymbol * tile_length_field;
//    SgVariableSymbol * tile_stride_field;

    SgClassSymbol * data_class;
      SgVariableSymbol * data_ptr_field;
      SgVariableSymbol * data_sections_field;

    SgClassSymbol * section_class;
      SgVariableSymbol * section_offset_field;
      SgVariableSymbol * section_length_field;

    SgFunctionSymbol * build_kernel_func;
    SgFunctionSymbol * execute_kernel_func;

  public:
    virtual void load(const MDCG::Model::model_t & model);

  public:
    SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const;
    void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const;

    SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;

    SgStatement * buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;
    SgStatement * buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const;

    SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
    SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const;
};

struct call_interface_t {
  protected:
    ::MFB::Driver< ::MFB::Sage> & driver;
    kernel_t * kernel_api;

  public:
    call_interface_t(::MFB::Driver< ::MFB::Sage> & driver_, kernel_t * kernel_api_);

    SgFunctionParameterList * buildKernelParamList(Descriptor::kernel_t & kernel) const;
    SgBasicBlock * generateKernelBody(Descriptor::kernel_t & kernel, SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map) const;

    // default: none
    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const;

    // default: void
    virtual SgType * buildKernelReturnType(Descriptor::kernel_t & kernel) const;

  protected:
    // default: klt_loop_context_t & loop_ctx (TODO klt_data_context_t & data_ctx)
    virtual void addKernelArgsForContext(SgFunctionParameterList * param_list) const;
    virtual void getContextSymbol(SgFunctionDefinition * func_defn, Utils::symbol_map_t & symbol_map) const;

    // default: "loop_it_'loop.id'"
    virtual void createLoopIterator(const std::vector<Descriptor::loop_t *> & loops, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;

    // default: "tile_it_'tile.id'"
    virtual void createTileIterator(const std::vector<Descriptor::tile_t *> & tiles, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;

    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const = 0;
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const = 0;

    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const = 0;
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const = 0;
};

struct array_args_interface_t : public call_interface_t {
  public:
    array_args_interface_t(::MFB::Driver< ::MFB::Sage> & driver, kernel_t * kernel_api);

    // adds one arguments: 'void ** parameters' (array of pointers to the parameters) 
    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const;
    // adds one arguments: 'void ** data' (array of pointers to the data)
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const;

    // for each parameter create a local declaration using the array 'parameters'
    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
    // for each data create a local declaration using the array 'data'
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
};

struct individual_args_interface_t : public call_interface_t {
  public:
    individual_args_interface_t(::MFB::Driver< ::MFB::Sage> & driver, kernel_t * kernel_api);

    // adds one arguments for each parameter
    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const;
    // adds one arguments for each data
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const;

    // get parameters argument's symbol
    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
    // get data argument's symbol
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const;
};

} // namespace KLT::API

} // namespace KLT

#endif /* __KLT_API_HPP__ */

