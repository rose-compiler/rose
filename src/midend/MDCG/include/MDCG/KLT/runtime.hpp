
#ifndef __MDCG_KLT_RUNTIME_HPP__
#define __MDCG_KLT_RUNTIME_HPP__

#include "KLT/Core/kernel.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-builder.hpp"

#include "MFB/utils.hpp"

class SgExpression;

namespace MDCG {

namespace KLT {

namespace API {

template <class Klang>
struct kernel_t {
  public:
    struct user_t;
    user_t * user;
    void load_user(const MDCG::Model::model_t & model);

  private:
    SgClassSymbol * klt_loop_context_class;

    SgFunctionSymbol * get_loop_lower_fnct;
    SgFunctionSymbol * get_loop_upper_fnct;
    SgFunctionSymbol * get_loop_stride_fnct;

    SgFunctionSymbol * get_tile_length_fnct;
    SgFunctionSymbol * get_tile_stride_fnct;

  public:
    kernel_t() :
      user(NULL), klt_loop_context_class(NULL),
      get_loop_lower_fnct(NULL), get_loop_upper_fnct(NULL), get_loop_stride_fnct(NULL),
      get_tile_length_fnct(NULL), get_tile_stride_fnct(NULL)
    {}

    ~kernel_t() { delete user; }

    SgType * addContextTypeModifier(SgType * type) const { return type; }

    SgInitializedName * createContext() const {
      return SageBuilder::buildInitializedName("context", addContextTypeModifier(SageBuilder::buildPointerType(klt_loop_context_class->get_declaration()->get_type())), NULL);
    }

    SgExpression * buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const {
      return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_lower_fnct);
    }
    SgExpression * buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const {
      return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_upper_fnct);
    }
    SgExpression * buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
      return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_stride_fnct);
    }

    SgExpression * buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
      return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_length_fnct);
    }
    SgExpression * buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
      return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_stride_fnct);
    }

    SgClassSymbol * getLoopContextClass() const { return klt_loop_context_class; }

    void load(const MDCG::Model::model_t & model) {
      klt_loop_context_class = model.lookup<MDCG::Model::class_t>("klt_loop_context_t")->node->symbol;
      assert(klt_loop_context_class != NULL);

      get_loop_lower_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_lower")->node->symbol;
      assert(get_loop_lower_fnct != NULL);

      get_loop_upper_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_upper")->node->symbol;
      assert(get_loop_upper_fnct != NULL);

      get_loop_stride_fnct = model.lookup<MDCG::Model::function_t>("klt_get_loop_stride")->node->symbol;
      assert(get_loop_stride_fnct != NULL);

      get_tile_length_fnct = model.lookup<MDCG::Model::function_t>("klt_get_tile_length")->node->symbol;
      assert(get_tile_length_fnct != NULL);

      get_tile_stride_fnct = model.lookup<MDCG::Model::function_t>("klt_get_tile_stride")->node->symbol;
      assert(get_tile_stride_fnct != NULL);

      load_user(model);
    }
};

template <class Hlang>
struct host_t {
  public:
    struct user_t;
    user_t * user;
    void load_user(const MDCG::Model::model_t & model);

  private:
    SgClassSymbol * kernel_class;
    SgClassSymbol * loop_class;
    SgClassSymbol * tile_class;
    SgClassSymbol * data_class;

    SgVariableSymbol * kernel_param_field;
    SgVariableSymbol * kernel_scalar_field;
    SgVariableSymbol * kernel_data_field;
    SgVariableSymbol * kernel_priv_field;
    SgVariableSymbol * data_ptr_field;
    SgVariableSymbol * data_section_field;
    SgVariableSymbol * section_offset_field;
    SgVariableSymbol * section_length_field;

    SgVariableSymbol * kernel_loop_field;
    SgVariableSymbol * loop_lower_field;
    SgVariableSymbol * loop_upper_field;
    SgVariableSymbol * loop_stride_field;

    SgFunctionSymbol * build_kernel_func;
    SgFunctionSymbol * execute_kernel_func;

    SgType * kernel_func_ptr_type;

  public:
    host_t() :
      user(NULL), kernel_class(NULL), loop_class(NULL), tile_class(NULL),
      kernel_data_field(NULL), kernel_param_field(NULL), kernel_scalar_field(NULL),
      kernel_loop_field(NULL), loop_lower_field(NULL), loop_upper_field(NULL), loop_stride_field(NULL),
      build_kernel_func(NULL), execute_kernel_func(NULL), kernel_func_ptr_type(NULL)
    {}

    ~host_t() { delete user; }

    SgVariableSymbol * insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
      SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                               build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                             ));
      SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
      SageInterface::appendStatement(kernel_decl, scope);

      SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
      assert(kernel_sym != NULL);

      return kernel_sym;
    }
    void insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
      SageInterface::appendStatement(
        SageBuilder::buildFunctionCallStmt(
          SageBuilder::buildFunctionRefExp(execute_kernel_func),
          SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
        ), scope
      );
    }

    SgStatement * buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
    }
    SgStatement * buildScalarAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_scalar_field, SageBuilder::buildIntVal(idx), NULL), rhs));
    }

    SgStatement * buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_ptr_field), rhs));
    }
    SgStatement * buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
               MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
             ), SageBuilder::buildVarRefExp(section_offset_field)), rhs));
    }
    SgStatement * buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
               MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
             ), SageBuilder::buildVarRefExp(section_length_field)), rhs));
    }

    SgStatement * buildPrivatePtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_priv_field, SageBuilder::buildIntVal(idx), data_ptr_field), rhs));
    }
    SgStatement * buildPrivateSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
               MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_priv_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
             ), SageBuilder::buildVarRefExp(section_offset_field)), rhs));
    }
    SgStatement * buildPrivateSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
               MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_priv_field, SageBuilder::buildIntVal(idx), data_section_field), SageBuilder::buildIntVal(dim)
             ), SageBuilder::buildVarRefExp(section_length_field)), rhs));
    }

    SgStatement * buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
    }
    SgStatement * buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
    }
    SgStatement * buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
      return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
    }

    SgType * getKernelFnctPtrType() const { return kernel_func_ptr_type; }

    SgClassSymbol * getKernelClass() const { return kernel_class; }
    SgClassSymbol * getLoopClass() const { return loop_class; }
    SgClassSymbol * getTileClass() const { return tile_class; }
    SgClassSymbol * getDataClass() const { return data_class; }

    void load(const MDCG::Model::model_t & model) {
      MDCG::Model::class_t kernel_class_ = model.lookup<MDCG::Model::class_t>("kernel_t");
      kernel_class = kernel_class_->node->symbol;
      assert(kernel_class != NULL);

        kernel_param_field  = kernel_class_->scope->getField("param" )->node->symbol;
        kernel_scalar_field = kernel_class_->scope->getField("scalar")->node->symbol;
        kernel_data_field   = kernel_class_->scope->getField("data"  )->node->symbol;
        kernel_priv_field   = kernel_class_->scope->getField("priv"  )->node->symbol;
        kernel_loop_field   = kernel_class_->scope->getField("loops" )->node->symbol;

      MDCG::Model::class_t data_class_ = model.lookup<MDCG::Model::class_t>("klt_data_t");
      data_class = data_class_->node->symbol;
      assert(data_class != NULL);

        data_ptr_field     = data_class_->scope->getField("ptr"     )->node->symbol;
        data_section_field = data_class_->scope->getField("sections")->node->symbol;

      MDCG::Model::class_t data_section_class = model.lookup<MDCG::Model::class_t>("klt_data_section_t");

        section_offset_field = data_section_class->scope->getField("offset")->node->symbol;
        section_length_field = data_section_class->scope->getField("length")->node->symbol;

      MDCG::Model::class_t loop_class_ = model.lookup<MDCG::Model::class_t>("klt_loop_t");
      loop_class = loop_class_->node->symbol;
      assert(loop_class != NULL);

        loop_lower_field  = loop_class_->scope->getField("lower" )->node->symbol;
        loop_upper_field  = loop_class_->scope->getField("upper" )->node->symbol;
        loop_stride_field = loop_class_->scope->getField("stride")->node->symbol;

      MDCG::Model::class_t tile_class_ = model.lookup<MDCG::Model::class_t>("klt_tile_t");
      tile_class = tile_class_->node->symbol;
      assert(tile_class != NULL);

      MDCG::Model::function_t build_kernel_func_ = model.lookup<MDCG::Model::function_t>("build_kernel");
      build_kernel_func = build_kernel_func_->node->symbol;
      assert(build_kernel_func != NULL);

      MDCG::Model::function_t execute_kernel_func_ = model.lookup<MDCG::Model::function_t>("execute_kernel");
      execute_kernel_func = execute_kernel_func_->node->symbol;
      assert(execute_kernel_func != NULL);

      MDCG::Model::type_t kernel_func_ptr_type_ = model.lookup<MDCG::Model::type_t>("kernel_func_ptr");
      kernel_func_ptr_type = kernel_func_ptr_type_->node->type;
      assert(kernel_func_ptr_type != NULL);

      load_user(model);
    }
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

  tile_t() : id(0), kind((tile_kind_e)0), nbr_it(0), order(0), iterator_sym(NULL) {}

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

  loop_t(size_t id_, SgExpression * lb_, SgExpression * ub_, SgExpression * stride_) : id(id_), lb(lb_), ub(ub_), stride(stride_) {}
};

} // namespace MDCG::KLT::Descriptor

template <class Hlang_, class Klang_>
struct Runtime {

    typedef Hlang_ Hlang;
    typedef Klang_ Klang;

  // Tile and Loop Descriptions

    typedef Descriptor::tile_t tile_desc_t;
    typedef Descriptor::loop_t loop_desc_t;

  // APIs

    static void loadUserAPI(MDCG::ModelBuilder & model_builder, size_t tilek_model, const std::string & USER_RTL);

    static size_t loadAPI(MDCG::ModelBuilder & model_builder, const std::string & KLT_RTL, const std::string & USER_RTL) {
      size_t tilek_model = model_builder.create();

      model_builder.add(tilek_model, "data",    KLT_RTL + "/include/KLT/RTL", "h");
      model_builder.add(tilek_model, "tile",    KLT_RTL + "/include/KLT/RTL", "h");
      model_builder.add(tilek_model, "loop",    KLT_RTL + "/include/KLT/RTL", "h");
      model_builder.add(tilek_model, "context", KLT_RTL + "/include/KLT/RTL", "h");

      loadUserAPI(model_builder, tilek_model, USER_RTL);

      host_api.load(model_builder.get(tilek_model));
      kernel_api.load(model_builder.get(tilek_model));

      return tilek_model;
    }

    typedef API::kernel_t<Klang> kernel_api_t;
    static kernel_api_t kernel_api;
    static void useSymbolsKernel(MFB::Driver<MFB::Sage> & driver, size_t file_id) {
      driver.useSymbol<SgClassDeclaration>(kernel_api.getLoopContextClass(), file_id);
    }

    typedef API::host_t<Hlang> host_api_t;
    static host_api_t host_api;
    static void useSymbolsHost(MFB::Driver<MFB::Sage> & driver, size_t file_id) {
      driver.useSymbol<SgClassDeclaration>(host_api.getDataClass(),   file_id);
      driver.useSymbol<SgClassDeclaration>(host_api.getTileClass(),   file_id);
      driver.useSymbol<SgClassDeclaration>(host_api.getLoopClass(),   file_id);
      driver.useSymbol<SgClassDeclaration>(host_api.getKernelClass(), file_id);
    }

  // Execution Modes

    enum exec_mode_t { e_default };

  // Extra

    static void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) {}

    static void addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id) {}
};

} // namespace MDCG::KLT

} // namespace MDCG

#endif /* __MDCG_KLT_RUNTIME_HPP__ */

