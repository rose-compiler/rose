
#ifndef __KLT_TILEK_HPP__
#define __KLT_TILEK_HPP__

#include "KLT/Core/mfb-klt.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/loop-tiler.hpp"

#include "DLX/TileK/language.hpp"
#include "DLX/klt-annotations.hpp"

#include "MFB/Sage/driver.hpp"

#include <vector>

class SgExpression;
class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;
class SgFunctionDeclaration;

namespace MDCG { namespace Model { class model_t; } }

namespace DLX {

template <>
void KLT_Annotation<DLX::TileK::language_t>::parseRegion(std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> > & container);

template <>
void KLT_Annotation<DLX::TileK::language_t>::parseData(std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> > & container);

template <>
void KLT_Annotation<DLX::TileK::language_t>::parseLoop(std::vector<DLX::KLT_Annotation<DLX::TileK::language_t> > & container);

}

namespace KLT {

namespace Language {

struct None {
  static void applyKernelModifiers(SgFunctionDeclaration * kernel_decl);
};

}

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

      loop_desc_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_) :
        id(id_), lb(lb_), ub(ub_), stride(stride_)
      {}
    };

    struct exec_config_t {};

    enum exec_mode_t { e_default };

    static struct tilek_kernel_api_t {
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
    } tilek_kernel_api;

    static struct tilek_host_api_t {
      SgClassSymbol * kernel_class;
        SgVariableSymbol * kernel_data_field;
        SgVariableSymbol * kernel_param_field;
        SgVariableSymbol * kernel_loop_field;
          SgVariableSymbol * loop_lower_field;
          SgVariableSymbol * loop_upper_field;
          SgVariableSymbol * loop_stride_field;
      SgFunctionSymbol * build_kernel_func;
      SgFunctionSymbol * execute_kernel_func;
    } tilek_host_api;

    static void loadAPI(const MDCG::Model::model_t & model);
    static void useSymbolsKernel(MFB::Driver<MFB::Sage> & driver, unsigned long file_id);
    static void useSymbolsHost(MFB::Driver<MFB::Sage> & driver, unsigned long file_id);
};

}

template <>
LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::loop_tiling_t::loop_tiling_t(LoopTrees<DLX::KLT_Annotation<DLX::TileK::language_t> >::loop_t * loop_);

}

namespace MFB {

typedef ::KLT::Kernel< ::DLX::KLT_Annotation< ::DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK> tilek_kernel_t;

template <>
class KLT<tilek_kernel_t> {
  public:
    typedef ::DLX::KLT_Annotation< ::DLX::TileK::language_t> Annotation;
    typedef ::KLT::Language::None Language;
    typedef ::KLT::Runtime::TileK Runtime;

    struct object_desc_t {
      unsigned id;
      tilek_kernel_t * kernel;
      unsigned long file_id;
      std::map<
        ::KLT::LoopTrees< ::DLX::KLT_Annotation< ::DLX::TileK::language_t> >::loop_t *,
        ::KLT::LoopTiler<DLX::KLT_Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::loop_tiling_t *
      > tiling;

      object_desc_t(
        unsigned id_,
        tilek_kernel_t * kernel_,
        unsigned long file_id_
      );
    };

    typedef tilek_kernel_t::kernel_desc_t * build_result_t;
};

}

#endif /* __KLT_TILEK_HPP__ */

