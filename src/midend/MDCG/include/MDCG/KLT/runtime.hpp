
#ifndef __MDCG_KLT_RUNTIME_HPP__
#define __MDCG_KLT_RUNTIME_HPP__

#include "KLT/Core/kernel.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-builder.hpp"

class SgExpression;

namespace MDCG {

namespace KLT {

namespace API {

template <class Klang>
struct kernel_t {
  private:
    SgClassSymbol * context_class;
    SgClassSymbol * loop_class;
    SgClassSymbol * tile_class;

    SgVariableSymbol * context_loop_field;
    SgVariableSymbol * loop_lower_field;
    SgVariableSymbol * loop_upper_field;
    SgVariableSymbol * loop_stride_field;
    SgVariableSymbol * context_tile_field;
    SgVariableSymbol * tile_length_field;
    SgVariableSymbol * tile_stride_field;

  public:
    SgInitializedName * createContext() const;

    SgExpression * buildLoopLower (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const;
    SgExpression * buildLoopStride(size_t loop_id, SgVariableSymbol * ctx) const;

    SgExpression * buildTileLength(size_t tile_id, SgVariableSymbol * ctx) const;
    SgExpression * buildTileStride(size_t tile_id, SgVariableSymbol * ctx) const;

    void load(const MDCG::Model::model_t & model);

    SgClassSymbol * getContextClass() const { return context_class; }
    SgClassSymbol * getLoopClass() const { return loop_class; }
    SgClassSymbol * getTileClass() const { return tile_class; }
};

template <class Hlang>
struct host_t {
  private:
    SgClassSymbol * kernel_class;
    SgClassSymbol * loop_class;
    SgClassSymbol * tile_class;

    SgVariableSymbol * kernel_data_field;
    SgVariableSymbol * kernel_param_field;
    SgVariableSymbol * kernel_scalar_field;
    SgVariableSymbol * kernel_loop_field;
    SgVariableSymbol * loop_lower_field;
    SgVariableSymbol * loop_upper_field;
    SgVariableSymbol * loop_stride_field;

    SgFunctionSymbol * build_kernel_func;
    SgFunctionSymbol * execute_kernel_func;

    SgType * kernel_func_ptr_type;

  public:
    SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const;
    void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const;

    SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t param_id, SgExpression * rhs) const;
    SgStatement * buildScalarAssign(SgVariableSymbol * kernel_sym, size_t param_id, SgExpression * rhs) const;
    SgStatement * buildDataAssign(SgVariableSymbol * kernel_sym, size_t data_id, SgExpression * rhs) const;

    SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;
    SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;
    SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;

    void load(const MDCG::Model::model_t & model);

    SgType * getKernelFnctPtrType() const { return kernel_func_ptr_type; }

    SgClassSymbol * getKernelClass() const { return kernel_class; }
    SgClassSymbol * getLoopClass() const { return loop_class; }
    SgClassSymbol * getTileClass() const { return tile_class; }
};

} // namespace MDCG::KLT::API

namespace Descriptor {

enum tile_kind_e { e_static_tile = 0, e_dynamic_tile = 1 };

struct tile_t {
  size_t id;
  enum tile_kind_e kind;
  size_t nbr_it;
  size_t order;
  SgVariableSymbol * iterator_sym;

  tile_t();

  template <class tile_clause_tpl>
  void init(tile_clause_tpl * tile_clause) {
    kind = (tile_kind_e)tile_clause->parameters.kind;
    nbr_it = tile_clause->parameters.nbr_it;
    order = tile_clause->parameters.order;
  }
};

struct loop_t {
  size_t id;
  SgExpression * lb;
  SgExpression * ub;
  SgExpression * stride;
  std::vector<tile_t *> tiles;

  loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_);
};

} // namespace MDCG::KLT::Descriptor

template <class Hlang, class Klang>
struct Runtime {

  // Tile and Loop Descriptions

    typedef Descriptor::tile_t tile_desc_t;
    typedef Descriptor::loop_t loop_desc_t;

  // APIs

    static size_t loadAPI(MDCG::ModelBuilder & model_builder, const std::string & KLT_RTL, const std::string & USER_RTL);

    typedef API::kernel_t<Klang> kernel_api_t;
    static kernel_api_t kernel_api;
    static void useSymbolsKernel(MFB::Driver<MFB::Sage> & driver, size_t file_id);

    typedef API::host_t<Hlang> host_api_t;
    static host_api_t host_api;
    static void useSymbolsHost(MFB::Driver<MFB::Sage> & driver, size_t file_id);

  // Execution Modes

    enum exec_mode_t { e_default };

  // Execution Configuration

    typedef struct {} exec_config_t;

    template <class Annotation>
    static void set_exec_config(exec_config_t & exec_config, ::KLT::Kernel<Annotation, Runtime> * kernel) {}

  // Extra

    static void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) {};
};

} // namespace MDCG::KLT

} // namespace MDCG

#endif /* __MDCG_KLT_RUNTIME_HPP__ */

