
#ifndef __MDCG_KLT_MODEL_HPP__
#define __MDCG_KLT_MODEL_HPP__

#include "MDCG/Core/static-initializer.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"

namespace MDCG {

namespace KLT {

template <class Hlang_>
struct TileDesc {
  typedef MDCG::KLT::Descriptor::tile_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int idx;
        return SageBuilder::buildIntVal(input->id);
      }
      case 1:
      { // enum tile_kind_e { e_tile_static, e_tile_dynamic } kind;
        return SageBuilder::buildIntVal(input->kind);
      }
      case 2:
      { // int param;
        return SageBuilder::buildIntVal(input->nbr_it);
      }
      default:
        assert(false);
    }
  }
};

template <class Hlang_>
struct LoopDesc {
  typedef MDCG::KLT::Descriptor::loop_t * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
        /// size_t id;
        return SageBuilder::buildIntVal(input->id);
      case 1:
        /// size_t num_tiles;
        return SageBuilder::buildIntVal(input->tiles.size());
      case 2:
      { /// struct tile_desc_t * tile_desc;
        std::ostringstream decl_name; decl_name << "tile_" << &input;
        MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "tile_desc", "klt_tile_desc_t");
        return static_initializer.createArrayPointer<TileDesc<Hlang_> >(field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str());
      }
      default:
        assert(false);
    }
  }
};

template <class Annotation_, class Runtime_>
struct LoopContainer {
  typedef ::KLT::Kernel<Annotation_, Runtime_> Kernel;
  typedef Kernel * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    assert(input->getKernels().size() == 1);
    typename Kernel::kernel_desc_t * kernel = input->getKernels()[0];
    assert(kernel != NULL);

    switch (field_id) {
      case 0:
      { // int num_loops;
        return SageBuilder::buildIntVal(kernel->loops.size());
      }
      case 1:
      { // int num_tiles;
        return SageBuilder::buildIntVal(kernel->tiles.size());
      }
      case 2:
      { // struct loop_desc_t * loop_desc;
        std::ostringstream decl_name; decl_name << "loop_" << kernel;
        MDCG::Model::class_t field_class = StaticInitializer::getBaseClassForPointerOnClass(element, "loop_desc", "klt_loop_desc_t");
        return static_initializer.createArrayPointer< ::MDCG::KLT::LoopDesc<typename Runtime_::Hlang> >(field_class, kernel->loops.size(), kernel->loops.begin(), kernel->loops.end(), file_id, decl_name.str());
      }
      default:
        assert(false);
    }
  }
};

template <class Annotation_, class Runtime_>
struct DataContainer {
  typedef ::KLT::Kernel<Annotation_, Runtime_> * input_t;
  typedef ::KLT::Data<Annotation_> Data;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int num_param;
        return SageBuilder::buildIntVal(input->getArguments().parameters.size());
      }
      case 1:
      { // int * sizeof_param;
        SgGlobal * global_scope = static_initializer.getDriver().getGlobalScope(file_id);
        std::ostringstream decl_name; decl_name << "sizeof_param_" << input;
        SgExprListExp * expr_list = SageBuilder::buildExprListExp();
        SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
        std::list<SgVariableSymbol *>::const_iterator it;
        for (it = input->getArguments().parameters.begin(); it != input->getArguments().parameters.end(); it++) {
          static_initializer.getDriver().useType((*it)->get_type(), global_scope);
          expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));
        }
        SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(input->getArguments().parameters.size()));
        MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name.str(), type, init, NULL, file_id, false, true);
        MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);
        return SageBuilder::buildVarRefExp(var_decl_res.symbol);
      }
      case 2:
      { // int num_scalar;
        return SageBuilder::buildIntVal(input->getArguments().scalars.size());
      }
      case 3:
      { // int * sizeof_scalar;
        SgGlobal * global_scope = static_initializer.getDriver().getGlobalScope(file_id);
        std::ostringstream decl_name; decl_name << "sizeof_scalar_" << input;
        SgExprListExp * expr_list = SageBuilder::buildExprListExp();
        SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
        std::list<SgVariableSymbol *>::const_iterator it;
        for (it = input->getArguments().scalars.begin(); it != input->getArguments().scalars.end(); it++) {
          static_initializer.getDriver().useType((*it)->get_type(), global_scope);
          expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));
        }
        SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(input->getArguments().scalars.size()));
        MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name.str(), type, init, NULL, file_id, false, true);
        MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);
        return SageBuilder::buildVarRefExp(var_decl_res.symbol);
      }
      case 4:
      { // int num_data;
        return SageBuilder::buildIntVal(input->getArguments().datas.size());
      }
      case 5:
      { // int * sizeof_data;
        SgGlobal * global_scope = static_initializer.getDriver().getGlobalScope(file_id);
        std::ostringstream decl_name; decl_name << "sizeof_data_" << input;
        SgExprListExp * expr_list = SageBuilder::buildExprListExp();
        SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
        typename std::list<Data *>::const_iterator it;
        for (it = input->getArguments().datas.begin(); it != input->getArguments().datas.end(); it++) {
          static_initializer.getDriver().useType((*it)->getBaseType(), global_scope);
          expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->getBaseType()));
        }
        SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(input->getArguments().datas.size()));
        MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name.str(), type, init, NULL, file_id, false, true);
        MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);
        return SageBuilder::buildVarRefExp(var_decl_res.symbol);
      }
      case 6:
      { // int num_priv;
        return SageBuilder::buildIntVal(input->getArguments().privates.size());
      }
      case 7:
      { // int * sizeof_priv;
        SgGlobal * global_scope = static_initializer.getDriver().getGlobalScope(file_id);
        std::ostringstream decl_name; decl_name << "sizeof_priv_" << input;
        SgExprListExp * expr_list = SageBuilder::buildExprListExp();
        SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);
        typename std::list<Data *>::const_iterator it;
        for (it = input->getArguments().privates.begin(); it != input->getArguments().privates.end(); it++) {
          static_initializer.getDriver().useType((*it)->getBaseType(), global_scope);
          expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->getBaseType()));
        }
        SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(input->getArguments().privates.size()));
        MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name.str(), type, init, NULL, file_id, false, true);
        MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);
        return SageBuilder::buildVarRefExp(var_decl_res.symbol);
      }
      default:
        assert(false);
    }
  }
};

} // namespace MDCG::KLT

} // namespace MDCG

#endif /* __MDCG_KLT_MODEL_HPP__ */
