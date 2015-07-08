
#ifndef __KLT_TILEK_RUNTIME_HPP__
#define __KLT_TILEK_RUNTIME_HPP__

#include "KLT/Core/mfb-klt.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/loop-tiler.hpp"

#include "DLX/TileK/language.hpp"
#include "DLX/KLT/annotations.hpp"

#include "MFB/Sage/driver.hpp"
#include "MDCG/model-builder.hpp"

#include <vector>

class SgExpression;
class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgFunctionDeclaration;

namespace MDCG { namespace Model { class model_t; } }

namespace Runtime {

class TileK {
  public:
    enum tile_kind_e {
      e_static_tile = 0,
      e_dynamic_tile = 1
    };

    struct tile_desc_t {
      size_t id; // ID of the tile in the kernel

      enum tile_kind_e kind;

      size_t nbr_it;
      size_t order;

      SgVariableSymbol * iterator_sym;

      tile_desc_t(const tile_desc_t & tile_desc) :
        id(tile_desc.id), kind(tile_desc.kind), nbr_it(tile_desc.nbr_it), order(tile_desc.order), iterator_sym(tile_desc.iterator_sym)
      {}

      tile_desc_t() :
        id(0), kind((tile_kind_e)0), nbr_it(0), order(0), iterator_sym(NULL)
      {}
    };

    struct loop_desc_t {
      size_t id; // id of the loop in the kernel

      SgExpression * lb;
      SgExpression * ub;
      SgExpression * stride;

      std::vector<tile_desc_t *> tiles;

      loop_desc_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_) :
        id(id_), lb(lb_), ub(ub_), stride(stride_)
      {}
    };

    struct exec_config_t {};

    enum exec_mode_t { e_default };

    static struct kernel_api_t {
      SgClassSymbol * context_class;
        SgClassSymbol * context_loop_class;
        SgVariableSymbol * context_loop_field;
          SgVariableSymbol * context_loop_lower_field;
          SgVariableSymbol * context_loop_upper_field;
          SgVariableSymbol * context_loop_stride_field;
        SgClassSymbol * context_tile_class;
        SgVariableSymbol * context_tile_field;
          SgVariableSymbol * context_tile_length_field;
          SgVariableSymbol * context_tile_stride_field;

      SgExpression * buildLoopLower (size_t loop_id, SgVariableSymbol * ctx) const;
      SgExpression * buildLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const;
      SgExpression * buildLoopStride(size_t loop_id, SgVariableSymbol * ctx) const;

      SgExpression * buildTileLength(size_t tile_id, SgVariableSymbol * ctx) const;
      SgExpression * buildTileStride(size_t tile_id, SgVariableSymbol * ctx) const;
    } kernel_api;

    static struct host_api_t {
      SgClassSymbol * kernel_class;
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

      SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const;
      void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const;

      SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t param_id, SgExpression * rhs) const;
      SgStatement * buildScalarAssign(SgVariableSymbol * kernel_sym, size_t param_id, SgExpression * rhs) const;
      SgStatement * buildDataAssign(SgVariableSymbol * kernel_sym, size_t data_id, SgExpression * rhs) const;

      SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;
      SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;
      SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t loop_id, SgExpression * rhs) const;
    } host_api;

    static unsigned loadAPI(MDCG::ModelBuilder & model_builder, const std::string & headers_path);
    static void loadAPI(const MDCG::Model::model_t & model);
    static void useSymbolsKernel(MFB::Driver<MFB::Sage> & driver, unsigned long file_id);
    static void useSymbolsHost(MFB::Driver<MFB::Sage> & driver, unsigned long file_id);
};

} // namespace KLT::Runtime

} // namespace KLT

#endif /* __KLT_TILEK_RUNTIME_HPP__ */

