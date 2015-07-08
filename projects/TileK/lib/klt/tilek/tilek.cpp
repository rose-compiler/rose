
#include "sage3basic.h"

#include "KLT/utils.hpp"

#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-tiler.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/function-declaration.hpp"

#include "KLT/TileK/tilek.hpp"

#include "MDCG/model.hpp"
#include "MDCG/model-class.hpp"
#include "MDCG/model-function.hpp"
#include "MDCG/model-type.hpp"

namespace KLT {

template <>
unsigned long Generator<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK,
  MFB::KLT_Driver
>::createFile() {
  unsigned long file_id = p_sage_driver.create(boost::filesystem::path(p_file_name));
  return file_id;
}

template <>
SgFunctionParameterList * createParameterList<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
>(
  Kernel<
    DLX::KLT::Annotation<DLX::TileK::language_t>,
    Language::None,
    Runtime::TileK
  > * kernel
) {
  const std::list<SgVariableSymbol *> & params = kernel->getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = kernel->getArguments().scalars;
  const std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *> & datas = kernel->getArguments().datas;

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  result->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildIntType()), NULL));	
  result->append_arg(SageBuilder::buildInitializedName("data",  SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));

  assert(Runtime::TileK::kernel_api.context_class != NULL);
  result->append_arg(SageBuilder::buildInitializedName(
    "context", SageBuilder::buildModifierType(SageBuilder::buildPointerType(Runtime::TileK::kernel_api.context_class->get_declaration()->get_type())), NULL
  ));

  return result;
}

namespace Runtime {

TileK::kernel_api_t TileK::kernel_api;

// Build: 'expr'->'array'['idx'] or 'expr'->'array'['idx'].'field'
SgExpression * buildPtrArrElemField(SgExpression * expr, SgVariableSymbol * array, SgExpression * idx, SgVariableSymbol * field = NULL) {
  SgExpression * result = SageBuilder::buildPntrArrRefExp(SageBuilder::buildArrowExp(expr, SageBuilder::buildVarRefExp(array)), idx);
  if (field != NULL) result = SageBuilder::buildDotExp(result, SageBuilder::buildVarRefExp(field));
  return result;
}

// Build: 'ctx'->loops['loop_id'].lower
SgExpression * TileK::kernel_api_t::buildLoopLower(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_lower_field);
}

// Build: 'ctx'->loops['loop_id'].upper
SgExpression * TileK::kernel_api_t::buildLoopUpper(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_upper_field);
}

// Build: 'ctx'->loops['loop_id'].stride
SgExpression * TileK::kernel_api_t::buildLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_loop_field, SageBuilder::buildIntVal(loop_id), context_loop_stride_field);
}

// Build: 'ctx'->tiles['tile_id'].length
SgExpression * TileK::kernel_api_t::buildTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_tile_field, SageBuilder::buildIntVal(tile_id), context_tile_length_field);
}

// Build: 'ctx'->tiles['tile_id'].stride
SgExpression * TileK::kernel_api_t::buildTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return buildPtrArrElemField(SageBuilder::buildVarRefExp(ctx), context_tile_field, SageBuilder::buildIntVal(tile_id), context_tile_stride_field);
}

TileK::host_api_t TileK::host_api;

// Insert: "struct kernel_t * kernel = build_kernel('kernel->id');"
SgVariableSymbol * TileK::host_api_t::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

// Insert: "execute_kernel(kernel);"
void TileK::host_api_t::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

// Build: 'kernel'->params['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->datas['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildDataAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->scalars['idx'] = 'rhs'
SgStatement * TileK::host_api_t::buildScalarAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_scalar_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

// Build: 'kernel'->loops['idx'].lower = 'rhs'
SgStatement * TileK::host_api_t::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

// Build: 'kernel'->loops['idx'].upper = 'rhs'
SgStatement * TileK::host_api_t::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

// Build: 'kernel'->loops['idx'].stride = 'rhs'
SgStatement * TileK::host_api_t::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loop_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}


unsigned TileK::loadAPI(MDCG::ModelBuilder & model_builder, const std::string & headers_path) {
  unsigned tilek_model = model_builder.create();
    model_builder.add(tilek_model, "tile",   headers_path, "h");
    model_builder.add(tilek_model, "loop",   headers_path, "h");
    model_builder.add(tilek_model, "kernel", headers_path, "h");
  loadAPI(model_builder.get(tilek_model));
  return tilek_model;
}

void TileK::loadAPI(const MDCG::Model::model_t & model) {

  MDCG::Model::class_t context_class = model.lookup<MDCG::Model::class_t>("context_t");
  kernel_api.context_class = context_class->node->symbol;
  assert(kernel_api.context_class != NULL);

    kernel_api.context_loop_field = context_class->scope->field_children[0]->node->symbol;
    kernel_api.context_tile_field = context_class->scope->field_children[1]->node->symbol;

  MDCG::Model::class_t kernel_class = model.lookup<MDCG::Model::class_t>("kernel_t");
  host_api.kernel_class = kernel_class->node->symbol;
  assert(host_api.kernel_class != NULL);

    host_api.kernel_data_field  = kernel_class->scope->field_children[1]->node->symbol;
    host_api.kernel_param_field = kernel_class->scope->field_children[2]->node->symbol;
    host_api.kernel_scalar_field = kernel_class->scope->field_children[3]->node->symbol;
    host_api.kernel_loop_field  = kernel_class->scope->field_children[4]->node->symbol;

  MDCG::Model::class_t loop_class = model.lookup<MDCG::Model::class_t>("loop_t");
  kernel_api.context_loop_class = loop_class->node->symbol;
  assert(kernel_api.context_loop_class != NULL);

    kernel_api.context_loop_lower_field  = loop_class->scope->field_children[0]->node->symbol;
    kernel_api.context_loop_upper_field  = loop_class->scope->field_children[1]->node->symbol;
    kernel_api.context_loop_stride_field = loop_class->scope->field_children[2]->node->symbol;

    host_api.loop_lower_field  = loop_class->scope->field_children[0]->node->symbol;
    host_api.loop_upper_field  = loop_class->scope->field_children[1]->node->symbol;
    host_api.loop_stride_field = loop_class->scope->field_children[2]->node->symbol;

  MDCG::Model::class_t tile_class = model.lookup<MDCG::Model::class_t>("tile_t");
  kernel_api.context_tile_class = tile_class->node->symbol;
  assert(kernel_api.context_tile_class != NULL);

    kernel_api.context_tile_length_field = tile_class->scope->field_children[0]->node->symbol;
    kernel_api.context_tile_stride_field = tile_class->scope->field_children[1]->node->symbol;

  MDCG::Model::function_t build_kernel_func = model.lookup<MDCG::Model::function_t>("build_kernel");
  host_api.build_kernel_func = build_kernel_func->node->symbol;
  assert(host_api.build_kernel_func != NULL);

  MDCG::Model::function_t execute_kernel_func = model.lookup<MDCG::Model::function_t>("execute_kernel");
  host_api.execute_kernel_func = execute_kernel_func->node->symbol;
  assert(host_api.execute_kernel_func != NULL);

  MDCG::Model::type_t kernel_func_ptr_type = model.lookup<MDCG::Model::type_t>("kernel_func_ptr");
  host_api.kernel_func_ptr_type = kernel_func_ptr_type->node->type;
  assert(host_api.kernel_func_ptr_type != NULL);
}

void TileK::useSymbolsKernel(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::kernel_api.context_tile_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::kernel_api.context_loop_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::kernel_api.context_class, file_id);
}

void TileK::useSymbolsHost(
  MFB::Driver<MFB::Sage> & driver,
  unsigned long file_id
) {
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::host_api.kernel_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::kernel_api.context_tile_class, file_id);
  driver.useSymbol<SgClassDeclaration>(::KLT::Runtime::TileK::kernel_api.context_loop_class, file_id);
}

template <>
void get_exec_config<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
> (
  TileK::exec_config_t & exec_config,
  const Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK> * kernel
) {}

} // namespace KLT::Runtime

template <>
LoopTiler<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::loop_tiling_t::loop_tiling_t(
  LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t * loop_
) :
  loop(loop_),
  tiles()
{
  std::vector<DLX::KLT::Annotation<DLX::TileK::language_t> >::const_iterator it_annotation;
  for (it_annotation = loop->annotations.begin(); it_annotation != loop->annotations.end(); it_annotation++) {
    if (
      it_annotation->clause->kind == DLX::TileK::language_t::e_clause_tile
    ) {
      Runtime::TileK::tile_desc_t & tile_desc = *(tiles.insert(tiles.end(), Runtime::TileK::tile_desc_t()));
      switch (it_annotation->clause->kind) {
        case DLX::TileK::language_t::e_clause_tile:
        {
          switch (((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.kind) {
            case DLX::Directives::generic_clause_t<DLX::TileK::language_t>::parameters_t<DLX::TileK::language_t::e_clause_tile>::e_static_tile:
              tile_desc.kind = Runtime::TileK::e_static_tile;
              tile_desc.nbr_it = ((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.nbr_it;
              break;
            case DLX::Directives::generic_clause_t<DLX::TileK::language_t>::parameters_t<DLX::TileK::language_t::e_clause_tile>::e_dynamic_tile:
              tile_desc.kind = Runtime::TileK::e_dynamic_tile;
              tile_desc.nbr_it = 0;
              break;
            default:
              assert(false);
          }
          tile_desc.order = ((DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_tile> *)(it_annotation->clause))->parameters.order;
          break;
        }
      }
    }
  }
}

template <>
size_t LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::id_cnt = 0;

template <>
bool LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t::isDistributed() const {
  return false;
}

template <>
bool LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t::isSplitted() const {
  return false;
}

template <>
void printAnnotations<DLX::KLT::Annotation<DLX::TileK::language_t> >(
  const std::vector<DLX::KLT::Annotation<DLX::TileK::language_t> > & annotations,
  std::ostream & out,
  std::string indent
) {
  out << DLX::TileK::language_t::language_label << "(";
  if (!annotations.empty()) { 
    std::vector<DLX::KLT::Annotation<DLX::TileK::language_t> >::const_iterator it_annotation = annotations.begin();
    out << it_annotation->clause->kind;
    it_annotation++;
    for (; it_annotation != annotations.end(); it_annotation++) {
      out << ", ";
      out << it_annotation->clause->kind;
    }
  }
  out << "), " << std::endl;
}

template <>
unsigned long Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::id_cnt = 0;

template <>
unsigned long Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::kernel_desc_t::id_cnt = 0;

template <>
bool Data<DLX::KLT::Annotation<DLX::TileK::language_t> >::isFlowIn() const {
  return true;
}

template <>
bool Data<DLX::KLT::Annotation<DLX::TileK::language_t> >::isFlowOut() const {
  return true;
}

template <>
void DataFlow<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
>::markSplittedData(
  const context_t & context
) const {}

namespace Language {

void None::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) {}

} // namespace KLT::Language

} // namespace KLT
/*
namespace DLX {

namespace KLT {

template <>
void Annotation<TileK::language_t>::parseRegion(std::vector<DLX::KLT::Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT::Annotation<TileK::language_t> & annotation = container.back();
}

template <>
void Annotation<TileK::language_t>::parseData(std::vector<DLX::KLT::Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT::Annotation<TileK::language_t> & annotation = container.back();
}

template <>
void Annotation<TileK::language_t>::parseLoop(std::vector<DLX::KLT::Annotation<TileK::language_t> > & container) {
  parseClause(container);

  DLX::KLT::Annotation<TileK::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case TileK::language_t::e_clause_tile:
      /// \todo 'lvl' and expression parameter
      break;
    default:
      assert(false);
  }
}

} // namespace DLX::KLT

} // namespace DLX
*/
namespace MFB {

KLT<tilek_kernel_t>::object_desc_t::object_desc_t(
  unsigned id_,
  tilek_kernel_t * kernel_,
  unsigned long file_id_
) :
  id(id_),
  kernel(kernel_),
  file_id(file_id_),
  tiling()
{}

SgVariableSymbol * getExistingSymbolOrBuildDecl(
  const std::string & name,
  SgType * type,
  SgScopeStatement * scope,
  SgInitializer * init = NULL
) {
  SgVariableSymbol * sym = scope->lookup_variable_symbol(name);
  if (sym == NULL) {
    SgVariableDeclaration * decl = SageBuilder::buildVariableDeclaration(name, type, init, scope);
    SageInterface::appendStatement(decl, scope);
    sym = scope->lookup_variable_symbol(name);
  }
  assert(sym != NULL);
  return sym;
}

template <>
SgBasicBlock * createLocalDeclarations<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  ::KLT::Language::None,
  ::KLT::Runtime::TileK
>(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  ::KLT::Kernel<
    DLX::KLT::Annotation<DLX::TileK::language_t>,
    ::KLT::Language::None,
    ::KLT::Runtime::TileK
  >::local_symbol_maps_t & local_symbol_maps,
  const ::KLT::Kernel<
    DLX::KLT::Annotation<DLX::TileK::language_t>,
    ::KLT::Language::None,
    ::KLT::Runtime::TileK
  >::arguments_t & arguments,
  const std::map<
    ::KLT::LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t *,
    ::KLT::LoopTiler<DLX::KLT::Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t *
  > & loop_tiling
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_scalar_to_field;
  std::map< ::KLT::Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_field;

  std::map<
    ::KLT::LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t *,
    ::KLT::LoopTiler<DLX::KLT::Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t *
  >::const_iterator it_loop_tiling;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);

  // * Lookup parameter symbols *

  SgVariableSymbol * arg_param_sym = kernel_defn->lookup_variable_symbol("param");
  assert(arg_param_sym != NULL);

  int arg_cnt = 0;
  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SageInterface::prependStatement(
      SageBuilder::buildVariableDeclaration(
        param_name,
        param_sym->get_type(),
        SageBuilder::buildAssignInitializer(SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_param_sym), SageBuilder::buildIntVal(arg_cnt))),
        kernel_body
      ),
      kernel_body
    );
    SgVariableSymbol * new_sym = kernel_body->lookup_variable_symbol(param_name);
    assert(new_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
    arg_cnt++;
  }

  // * Lookup data symbols *

  SgVariableSymbol * arg_data_sym = kernel_defn->lookup_variable_symbol("data");
  assert(arg_data_sym != NULL);

  arg_cnt = 0;
  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Data<DLX::KLT::Annotation<DLX::TileK::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name().getString();

    SgExpression * init = SageBuilder::buildCastExp(
      SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(arg_cnt)),
      SageBuilder::buildPointerType(data->getBaseType())
    );

    SgType * data_type;
    if (data->getSections().size() > 0)
      data_type = SageBuilder::buildPointerType(data->getBaseType());
    else {
      data_type = data->getBaseType();
      init = SageBuilder::buildPointerDerefExp(init);
    }

    SageInterface::prependStatement(
      SageBuilder::buildVariableDeclaration(
        data_name,
        data_type,
        SageBuilder::buildAssignInitializer(init),
        kernel_body
      ),
      kernel_body
    );
    SgVariableSymbol * new_sym = kernel_body->lookup_variable_symbol(data_name);
    assert(new_sym != NULL);

    local_symbol_maps.datas.insert(std::pair< ::KLT::Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *, SgVariableSymbol *>(data, new_sym));
    arg_cnt++;
  }

  // * Create iterator *

  for (it_loop_tiling = loop_tiling.begin(); it_loop_tiling != loop_tiling.end(); it_loop_tiling++) {
    ::KLT::LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::loop_t * loop = it_loop_tiling->first;
    ::KLT::LoopTiler<DLX::KLT::Annotation<DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK>::loop_tiling_t * tiling = it_loop_tiling->second;

    SgVariableSymbol * iter_sym = loop->iterator;
    std::string iter_name = iter_sym->get_name().getString();
    SgType * iter_type = iter_sym->get_type();

    std::ostringstream oss_loop;
    oss_loop << "it_" << loop->id;
    SgVariableSymbol * local_sym = getExistingSymbolOrBuildDecl(oss_loop.str(), iter_type, kernel_body);
    local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));

    size_t tile_cnt = 0;
    std::vector< ::KLT::Runtime::TileK::tile_desc_t>::iterator it_tile;
    for (it_tile = tiling->tiles.begin(); it_tile != tiling->tiles.end(); it_tile++) {
      std::ostringstream oss_tile;
      oss_tile << "it_" << loop->id << "_" << tile_cnt++;
      it_tile->iterator_sym = getExistingSymbolOrBuildDecl(oss_tile.str(), iter_type, kernel_body);
    }
  }

  local_symbol_maps.context = kernel_defn->lookup_variable_symbol("context");
  assert(local_symbol_maps.context != NULL);

  return kernel_body;

}

}

